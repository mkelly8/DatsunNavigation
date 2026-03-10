#ifndef APP_H
#define APP_H
/*
  File: app.h
  ----------------------------------------------------
  Application orchestrator interface.

  Architecture:
  - app.begin() initializes all modules and spawns three FreeRTOS tasks
  - Each task owns its timing via vTaskDelayUntil (precise periodic wakeup)
  - Shared state (GnssFix, Diagnostics) is protected by fixMutex
  - Tasks never block each other for more than a memcpy

  Tasks:
    GNSS   — polls Serial1, assembles NMEA lines, updates shared fix
    UI     — renders display at ~20 FPS using a local snapshot of fix
    Health — prints a structured [STATUS] heartbeat every 2 seconds
*/

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "types.h"
#include "gnss.h"
#include "display.h"
#include "ui.h"

class App
{
public:
    App();

    // Initializes modules, creates tasks, prints [READY].
    void begin();

private:
    // Modules
    Gnss    gnss;
    Display display;
    UI      ui;

    // Shared state — always access under fixMutex
    GnssFix     fix;
    Diagnostics diagnostics;

    SemaphoreHandle_t fixMutex;

    // Task handles (kept for future suspend/resume or watchdog checks)
    TaskHandle_t gnssTaskHandle;
    TaskHandle_t uiTaskHandle;
    TaskHandle_t healthTaskHandle;

    // Static entry points — FreeRTOS requires a plain function pointer.
    // Each casts param back to App* and calls the corresponding body.
    static void gnssTask  (void* param);
    static void uiTask    (void* param);
    static void healthTask(void* param);

    // Task bodies (infinite loops — never return)
    void gnssTaskBody();
    void uiTaskBody();
    void healthTaskBody();
};

#endif
