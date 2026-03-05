/*
  File: ESP32_Navigation.ino
  ----------------------------------------------------
  Project entry point.

  Responsibilities:
  - Initialize system (setup)
  - Call application scheduler (loop)
  - Contain no business logic

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
    app.tick(millis());
}