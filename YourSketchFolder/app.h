#ifndef APP_H
#define APP_H
/*
  File: app.h
  ----------------------------------------------------
  Application orchestrator interface.

  Responsibilities:
  - Initialize subsystems
  - Coordinate module updates
  - Manage system scheduling

  Acts as system "brain".
*/

#include <stdint.h>
#include "types.h"
#include "gnss.h"
#include "display.h"
#include "ui.h"

class App
{
public:
    App();

    void begin();
    void tick(uint32_t nowMs);

private:
    // Modules
    Gnss gnss;
    Display display;
    UI ui;

    // Shared state
    GnssFix fix;
    Diagnostics diagnostics;

    // Timing control
    uint32_t lastGnssTickMs;
    uint32_t lastUiTickMs;
    uint32_t lastHealthTickMs;

    // Internal helpers
    void tickGnss(uint32_t nowMs);
    void tickUi(uint32_t nowMs);
    void tickHealth(uint32_t nowMs);
};

#endif