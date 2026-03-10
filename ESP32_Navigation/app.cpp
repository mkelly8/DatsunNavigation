#include "app.h"
/*
  File: app.cpp
  ----------------------------------------------------
  Application orchestrator — FreeRTOS task architecture.

  Task overview:
    GNSS   (priority 5, core 1) — polls UART at GNSS_TICK_MS, copies fix under mutex
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

// ---------------------------------------------------------------
// Construction
// ---------------------------------------------------------------

App::App()
    : fix{},
      diagnostics{},
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
    // vTaskDelayUntil gives precise periodic wakeups unaffected by
    // execution time, unlike vTaskDelay which restarts the timer each cycle.
    TickType_t lastWake = xTaskGetTickCount();

    while (true)
    {
        gnss.tick(millis());

        // Copy new fix into shared state
        xSemaphoreTake(fixMutex, portMAX_DELAY);
        fix = gnss.getFix();
        xSemaphoreGive(fixMutex);

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(GNSS_TICK_MS));
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
        xSemaphoreTake(fixMutex, portMAX_DELAY);
        localFix  = fix;
        localDiag = diagnostics;
        xSemaphoreGive(fixMutex);

        const ScreenId screen = ui.getActiveScreen();
        display.render(screen, localFix, localDiag, millis());

        // Increment frame counter back under mutex
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
        xSemaphoreTake(fixMutex, portMAX_DELAY);
        localFix  = fix;
        localDiag = diagnostics;
        xSemaphoreGive(fixMutex);

        // Structured output — parseable by edesto / read_serial.py
        Serial.print("[STATUS]");
        Serial.print(" screen=");    Serial.print((int)ui.getActiveScreen());
        Serial.print(" fix=");       Serial.print(localFix.valid ? "Y" : "N");
        Serial.print(" sats=");      Serial.print(localFix.satellites);
        Serial.print(" uartBytes="); Serial.print(localDiag.uartBytes);
        Serial.print(" parsed=");    Serial.print(localDiag.sentencesParsed);
        Serial.print(" frames=");    Serial.println(localDiag.uiFrames);

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(HEALTH_TICK_MS));
    }
}
