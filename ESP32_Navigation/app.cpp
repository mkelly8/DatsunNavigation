#include "app.h"
/*
  File: app.cpp
  ----------------------------------------------------
  Application orchestrator — FreeRTOS task architecture.

  Task overview:
    GNSS   (priority 5, core 1) — calls gnss.tick() which blocks on getPVT()
                                   (~1 s per iteration at 1 Hz GNSS rate).
                                   No vTaskDelayUntil — the module paces this task.
    UI     (priority 3, core 1) — snapshots fix under mutex, renders at UI_TICK_MS
    Health (priority 1, core 1) — snapshots state under mutex, logs at HEALTH_TICK_MS

  Shared state access pattern:
    1. Take fixMutex
    2. Copy shared struct to a local variable (fast — just a memcpy)
    3. Release fixMutex
    4. Work with the local copy (rendering, printing, etc.)
  This keeps the mutex held for microseconds, not frame-render time.
*/

#include <Arduino.h>
#include "config.h"
#include "curvature.h"    // curvature_computeRadius/Ay used in healthTaskBody
#include "logger.h"

// ---------------------------------------------------------------
// Construction
// ---------------------------------------------------------------

App::App()
    : fix{},
      diagnostics{},
      navState{},
      curveScan{},
      fixMutex(nullptr),
      gnssTaskHandle(nullptr),
      uiTaskHandle(nullptr),
      healthTaskHandle(nullptr)
{
}

// ---------------------------------------------------------------
// begin() — called once from setup()
// ---------------------------------------------------------------

void App::begin()
{
    Serial.begin(115200);
    delay(300); // Allow USB-serial to enumerate before first print

    // LOG_DEBUG shows everything; change to LOG_INFO to suppress DEBUG lines.
    logger_setLevel(LOG_DEBUG);
    logger_log(LOG_INFO, "APP", "--- Datsun Navigation starting ---");

    fixMutex = xSemaphoreCreateMutex();

    logger_log(LOG_INFO, "APP", "Initialising GNSS...");
    gnss.begin();   // logs success or error internally

    display.begin();
    ui.begin();

    logger_log(LOG_INFO, "APP", "Starting FreeRTOS tasks");
    xTaskCreatePinnedToCore(gnssTask,   "GNSS",   TASK_STACK_GNSS,   this, TASK_PRIORITY_GNSS,   &gnssTaskHandle,   CORE_GNSS);
    xTaskCreatePinnedToCore(uiTask,     "UI",     TASK_STACK_UI,     this, TASK_PRIORITY_UI,     &uiTaskHandle,     CORE_UI);
    xTaskCreatePinnedToCore(healthTask, "Health", TASK_STACK_HEALTH, this, TASK_PRIORITY_HEALTH, &healthTaskHandle, CORE_HEALTH);

    logger_log(LOG_INFO, "APP", "Ready");
}

// ---------------------------------------------------------------
// Static task entry points
// ---------------------------------------------------------------

void App::gnssTask(void* param)
{
    static_cast<App*>(param)->gnssTaskBody();
}

void App::uiTask(void* param)
{
    static_cast<App*>(param)->uiTaskBody();
}

void App::healthTask(void* param)
{
    static_cast<App*>(param)->healthTaskBody();
}

// ---------------------------------------------------------------
// Task bodies
// ---------------------------------------------------------------

void App::gnssTaskBody()
{
    // getPVT() blocks ~1 s when the module is present, naturally yielding CPU.
    // When the module is absent tick() returns immediately — add an explicit
    // delay so this high-priority task does not starve the UI and Health tasks.
    while (true)
    {
        const uint32_t now = millis();
        gnss.tick(now);

        if (!gnss.isDeviceFound())
            vTaskDelay(pdMS_TO_TICKS(100));

        const GnssFix newFix = gnss.getFix();

        // Run the full navigation pipeline outside the mutex — navigator_ reads
        // only ROM route data, so it is safe without holding fixMutex.
        navigator_.update(newFix);

        // Publish updated state under mutex (fast copy only)
        xSemaphoreTake(fixMutex, portMAX_DELAY);
        if (newFix.lastUpdateMs != fix.lastUpdateMs)
        {
            diagnostics.pvtPackets++;
        }
        fix       = newFix;
        navState  = navigator_.getNavState();
        curveScan = navigator_.getCurveScan();
        xSemaphoreGive(fixMutex);
    }
}

void App::uiTaskBody()
{
    TickType_t lastWake = xTaskGetTickCount();

    while (true)
    {
        // Snapshot all shared state — hold mutex only for the copy, not the render
        GnssFix     localFix;
        Diagnostics localDiag;
        NavState    localNav;
        CurveScan   localCurve;
        xSemaphoreTake(fixMutex, portMAX_DELAY);
        localFix   = fix;
        localDiag  = diagnostics;
        localNav   = navState;
        localCurve = curveScan;
        xSemaphoreGive(fixMutex);

        // Drive the UI state machine and render
        ui.tick(millis());
        ui.notifyFix(localFix.valid);

        const ScreenId screen = ui.getActiveScreen();
        display.render(screen, localFix, localDiag, localNav, localCurve, millis());

        xSemaphoreTake(fixMutex, portMAX_DELAY);
        diagnostics.uiFrames++;
        xSemaphoreGive(fixMutex);

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(UI_TICK_MS));
    }
}

void App::healthTaskBody()
{
    TickType_t lastWake = xTaskGetTickCount();

    while (true)
    {
        GnssFix     localFix;
        Diagnostics localDiag;
        NavState    localNav;
        CurveScan   localCurve;
        xSemaphoreTake(fixMutex, portMAX_DELAY);
        localFix   = fix;
        localDiag  = diagnostics;
        localNav   = navState;
        localCurve = curveScan;
        xSemaphoreGive(fixMutex);

        // Compute curvature at current position (reads ROM, no mutex needed)
        const double radius = curvature_computeRadius(localNav.current_index);
        const double ay     = curvature_computeAy((double)localFix.speed_mps, radius);

        // Structured output — parseable by edesto / read_serial.py
        Serial.print("[STATUS]");
        Serial.print(" gnssOk=");  Serial.print(gnss.isDeviceFound() ? "Y" : "N");
        Serial.print(" fix=");     Serial.print(localFix.valid ? "Y" : "N");
        Serial.print(" sats=");    Serial.print(localFix.satellites);
        Serial.print(" pvt=");     Serial.print(localDiag.pvtPackets);
        Serial.print(" frames=");  Serial.print(localDiag.uiFrames);
        Serial.print(" idx=");     Serial.print(localNav.current_index);
        Serial.print(" r=");       Serial.print(radius, 1);
        Serial.print(" ay=");      Serial.print(ay, 3);

        // Upcoming curve look-ahead
        if (localCurve.found)
        {
            const char* dir = (localCurve.segment.direction == TURN_LEFT)  ? "L"
                            : (localCurve.segment.direction == TURN_RIGHT) ? "R"
                            :                                                "S";
            Serial.print(" curve=");  Serial.print(dir);
            Serial.print(" cdist=");  Serial.print(localCurve.distance_m, 0);
            Serial.print("m");
            if (localCurve.time_to_curve_s > 0.0f)
            {
                Serial.print(" ctime="); Serial.print(localCurve.time_to_curve_s, 1);
                Serial.print("s");
            }
            Serial.print(" cminr="); Serial.print(localCurve.segment.min_radius_m, 1);
            Serial.print("m cidx="); Serial.print(localCurve.segment.entry_index);
        }
        else
        {
            Serial.print(" curve=N");
        }
        Serial.println();

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(HEALTH_TICK_MS));
    }
}
