/*
  File: ESP32_Navigation.ino
  ----------------------------------------------------
  Project entry point.

  Responsibilities:
  - Call app.begin() which initializes modules and spawns FreeRTOS tasks
  - Suspend the Arduino loop task — all work happens in named tasks

  All system behavior is delegated to the App module.
*/

#include "app.h"

static App app;

void setup()
{
    app.begin();
}

void loop()
{
    // All work is done in FreeRTOS tasks created by app.begin().
    // Suspend this task indefinitely to avoid wasting CPU.
    vTaskDelay(portMAX_DELAY);
}
