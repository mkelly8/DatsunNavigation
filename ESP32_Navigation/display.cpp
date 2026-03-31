#include "display.h"
/*
  File: display.cpp
  ----------------------------------------------------
  Display implementation.

  Dispatches to screen-specific functions which currently emit
  structured Serial output.  When TFT_eSPI is integrated, the
  screen functions will draw to hardware — no changes needed here.
*/

#include <Arduino.h>
#include "screens.h"

Display::Display()
    : lastRenderMs(0)
{
}

void Display::begin()
{
    // Placeholder: initialize TFT_eSPI hardware here when ready.
    // e.g. tft.init(); tft.setRotation(1); ledcAttach(...);
    lastRenderMs = millis();
}

void Display::render(ScreenId           screen,
                     const GnssFix&     fix,
                     const Diagnostics& diag,
                     const NavState&    nav,
                     const CurveScan&   curve,
                     uint32_t           nowMs)
{
    lastRenderMs = nowMs;

    switch (screen)
    {
        case ScreenId::Boot:
            drawBootScreen(fix, diag, nav, curve);
            break;

        case ScreenId::Status:
            drawStatusScreen(fix, diag, nav, curve);
            break;

        case ScreenId::Navigation:
            drawNavigationScreen(fix, diag, nav, curve);
            break;

        default:
            drawStatusScreen(fix, diag, nav, curve);
            break;
    }
}
