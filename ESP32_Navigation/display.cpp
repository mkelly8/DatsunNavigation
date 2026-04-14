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
    // ---- Pin / config dump ----
    Serial.println("[DISP] begin()");
    Serial.print("[DISP] pins: MOSI="); Serial.print(TFT_MOSI);
    Serial.print(" SCLK=");            Serial.print(TFT_SCLK);
    Serial.print(" CS=");              Serial.print(TFT_CS);
    Serial.print(" DC=");              Serial.print(TFT_DC);
    Serial.print(" RST=");             Serial.print(TFT_RST);
    Serial.print(" BL=");              Serial.print(TFT_BL_PIN);
#ifdef TFT_MISO
    Serial.print(" MISO=");            Serial.print(TFT_MISO);
#else
    Serial.print(" MISO=<undef>");
#endif
    Serial.print(" freq=");            Serial.print(SPI_FREQUENCY);
#ifdef USE_FSPI_PORT
    Serial.println(" port=FSPI");
#else
    Serial.println(" port=SPI(default)");
#endif

    // ---- Init sequence ----
    Serial.println("[DISP] calling tft.init()...");
    tft.init();
    Serial.println("[DISP] tft.init() returned");

    tft.setRotation(0);
    Serial.println("[DISP] setRotation(0) done");

    tft.invertDisplay(true);
    Serial.println("[DISP] invertDisplay(true) done");

    tft.fillScreen(TFT_BLACK);
    Serial.println("[DISP] fillScreen(BLACK) done");

    // ---- Backlight ----
    ledcAttach(TFT_BL_PIN, TFT_BL_FREQ_HZ, TFT_BL_RES_BITS);
    Serial.println("[DISP] ledcAttach done");
    ledcWrite(TFT_BL_PIN, 220);
    Serial.println("[DISP] ledcWrite(220) done — backlight should be ON");

    // ---- 3-second colour flash ----
    const uint16_t testColors[]  = { TFT_RED,   TFT_GREEN, TFT_BLUE  };
    const char*    colorNames[]  = { "RED",     "GREEN",   "BLUE"    };
    for (int i = 0; i < 3; i++) {
        Serial.print("[DISP] fillScreen("); Serial.print(colorNames[i]); Serial.print(") ... ");
        const uint32_t t0 = millis();
        tft.fillScreen(testColors[i]);
        Serial.print(millis() - t0); Serial.println("ms");
        delay(1000);
    }
    tft.fillScreen(TFT_BLACK);
    Serial.println("[DISP] colour flash done");

    lastRenderMs = millis();
    Serial.println("[DISP] begin() complete");
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
