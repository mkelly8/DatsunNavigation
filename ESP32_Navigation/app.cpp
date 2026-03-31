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
#include "nav_state.h"
#include "guidance.h"
#include "curvature.h"

// ---------------------------------------------------------------
// Construction
// ---------------------------------------------------------------

App::App()
    : fix{},
      diagnostics{},
      navState{},
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

    fixMutex = xSemaphoreCreateMutex();

    gnss.begin();
    display.begin();
    ui.begin();

    xTaskCreatePinnedToCore(gnssTask,   "GNSS",   TASK_STACK_GNSS,   this, TASK_PRIORITY_GNSS,   &gnssTaskHandle,   CORE_GNSS);
    xTaskCreatePinnedToCore(uiTask,     "UI",     TASK_STACK_UI,     this, TASK_PRIORITY_UI,     &uiTaskHandle,     CORE_UI);
    xTaskCreatePinnedToCore(healthTask, "Health", TASK_STACK_HEALTH, this, TASK_PRIORITY_HEALTH, &healthTaskHandle, CORE_HEALTH);

    // Signal to any serial listener that the device is ready
    Serial.println("[READY]");
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
    // No vTaskDelayUntil here — gnss.tick() calls getPVT() which blocks
    // internally until the module delivers a fresh NAV-PVT packet (~1 s at 1 Hz).
    NavState localNav = {};

    while (true)
    {
        const uint32_t now = millis();
        gnss.tick(now);

        const GnssFix newFix = gnss.getFix();

        // Run the full navigation pipeline outside the mutex — these are
        // CPU-intensive but read-only on route data (flash), so they are
        // safe to run here without holding fixMutex.
        if (newFix.valid)
        {
            navState_update(&localNav,
                            newFix.latitude,
                            newFix.longitude,
                            (double)newFix.speed_mps);

            guidance_compute(localNav.current_index);
        }

        // Publish updated fix and nav state under mutex (fast copy only)
        xSemaphoreTake(fixMutex, portMAX_DELAY);
        if (newFix.lastUpdateMs != fix.lastUpdateMs)
        {
            diagnostics.pvtPackets++;
        }
        fix      = newFix;
        navState = localNav;
        xSemaphoreGive(fixMutex);
    }
}

void App::uiTaskBody()
{
    TickType_t lastWake = xTaskGetTickCount();

    while (true)
    {
        ui.tick(millis());

        // Snapshot shared state — hold mutex only for the copy, not the render
        GnssFix     localFix;
        Diagnostics localDiag;
        NavState    localNav;
        xSemaphoreTake(fixMutex, portMAX_DELAY);
        localFix  = fix;
        localDiag = diagnostics;
        localNav  = navState;
        xSemaphoreGive(fixMutex);
        (void)localNav; // available for display rendering when TFT is integrated

        const ScreenId screen = ui.getActiveScreen();
        display.render(screen, localFix, localDiag, millis());

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
        xSemaphoreTake(fixMutex, portMAX_DELAY);
        localFix  = fix;
        localDiag = diagnostics;
        localNav  = navState;
        xSemaphoreGive(fixMutex);

        // Compute curvature for the current route index (reads ROM, no mutex needed)
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
        Serial.print(" ay=");      Serial.println(ay, 3);

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(HEALTH_TICK_MS));
    }
}
