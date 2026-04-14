#include "display.h"
/*
  File: display.cpp
  ----------------------------------------------------
  Display implementation.

  Dispatches to screen-specific functions which draw to the
  ILI9341 TFT via TFT_eSPI (landscape, 320 x 240).

  tft is defined here as a global so that screens.cpp can
  access it via extern without changing the screens API.
*/

#include <Arduino.h>
#include "screens.h"
#include "config.h"

TFT_eSPI tft;

Display::Display()
    : lastRenderMs(0)
{
}

void Display::begin()
{
    tft.init();
    tft.setRotation(0);
    tft.invertDisplay(true);         // landscape: 320 x 240
    tft.fillScreen(TFT_BLACK);

    // Backlight PWM — full brightness
    ledcAttach(TFT_BL_PIN, TFT_BL_FREQ_HZ, TFT_BL_RES_BITS);
    ledcWrite(TFT_BL_PIN, 220);

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
