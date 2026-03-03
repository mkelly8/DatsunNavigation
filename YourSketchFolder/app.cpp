#include "app.h"
/*
  File: app.cpp
  ----------------------------------------------------
  Application orchestrator implementation.

  Handles:
  - Module initialization
  - Periodic task execution
  - System health checks
*/

#include <Arduino.h>

App::App()
    : lastGnssTickMs(0),
      lastUiTickMs(0),
      lastHealthTickMs(0)
{
}

void App::begin()
{
    // Serial for bring-up logging (fine for skeleton phase).
    Serial.begin(115200);
    delay(300);

    // Initialize modules (currently stubs).
    gnss.begin();
    display.begin();
    ui.begin();

    // Initialize timestamps.
    const uint32_t now = millis();
    lastGnssTickMs = now;
    lastUiTickMs = now;
    lastHealthTickMs = now;

    Serial.println("App begin: skeleton running (GNSS + Display + UI initialized).");
}

void App::tick(uint32_t nowMs)
{
    tickGnss(nowMs);
    tickUi(nowMs);
    tickHealth(nowMs);
}

void App::tickGnss(uint32_t nowMs)
{
    // Run frequently (placeholder schedule: every 5 ms)
    if (nowMs - lastGnssTickMs < 5) return;
    lastGnssTickMs = nowMs;

    gnss.tick(nowMs);
    fix = gnss.getFix(); // snapshot
}

void App::tickUi(uint32_t nowMs)
{
    // Frame limit (placeholder: 20 FPS)
    if (nowMs - lastUiTickMs < 50) return;
    lastUiTickMs = nowMs;

    // Update UI state machine (buttons later).
    ui.tick(nowMs);

    // For now, always render; later use ui.consumeRedrawRequested().
    const ScreenId screen = ui.getActiveScreen();
    display.render(screen, fix, diagnostics, nowMs);

    diagnostics.uiFrames++;
}

void App::tickHealth(uint32_t nowMs)
{
    // Heartbeat every 2 seconds
    if (nowMs - lastHealthTickMs < 2000) return;
    lastHealthTickMs = nowMs;

    Serial.print("Heartbeat | screen=");
    Serial.print((int)ui.getActiveScreen());
    Serial.print(" validFix=");
    Serial.print(fix.valid ? "Y" : "N");
    Serial.print(" sats=");
    Serial.print(fix.satellites);
    Serial.print(" uartBytes=");
    Serial.print(diagnostics.uartBytes);
    Serial.print(" parsed=");
    Serial.print(diagnostics.sentencesParsed);
    Serial.print(" uiFrames=");
    Serial.println(diagnostics.uiFrames);
}