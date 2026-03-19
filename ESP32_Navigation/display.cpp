#include "display.h"
/*
  File: display.cpp
  ----------------------------------------------------
  Display implementation.

  Currently a stub for minimum compile.
  TFT_eSPI rendering will be added later.
*/

#include <Arduino.h>
#include "screens.h"

Display::Display()
    : lastRenderMs(0)
{
}

void Display::begin()
{
    // Placeholder: initialize TFT later.
    lastRenderMs = millis();
}

void Display::render(ScreenId screen,
                     const GnssFix& fix,
                     const Diagnostics& diag,
                     uint32_t nowMs)
{
    lastRenderMs = nowMs;

    // Dispatch to screen-specific functions.
    switch (screen)
    {
        case ScreenId::Boot:
            drawBootScreen(fix, diag);
            break;

        case ScreenId::Status:
            drawStatusScreen(fix, diag);
            break;

        case ScreenId::Navigation:
            drawNavigationScreen(fix, diag);
            break;

        default:
            drawStatusScreen(fix, diag);
            break;
    }
}