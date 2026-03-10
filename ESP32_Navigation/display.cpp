#include "display.h"
/*
  File: display.cpp
  ----------------------------------------------------
  TFT display driver — ILI9341 via TFT_eSPI.

  Backlight:
    Controlled via ESP32-S3 LEDC (hardware PWM).
    Kept off during init to prevent a white flash on power-up,
    then brought to full brightness once the first frame is ready.

  Render strategy:
    - Full fillScreen() only on screen transitions (expensive: ~50 ms at 27 MHz).
    - Within a screen, individual regions are overwritten using
      setTextColor(fg, bg) which fills the character background automatically,
      preventing artefacts without a full clear every frame.

  Pin assignments are defined in config.h and must match User_Setup.h
  in the TFT_eSPI library folder.
*/

#include <Arduino.h>
#include "config.h"
#include "screens.h"

// ---------------------------------------------------------------
// Construction
// ---------------------------------------------------------------

Display::Display()
    : currentScreen(ScreenId::Boot),
      initialised(false)
{
}

// ---------------------------------------------------------------
// begin()
// ---------------------------------------------------------------

void Display::begin()
{
    // Configure LEDC backlight — off during init to avoid white flash
    // ESP32 Arduino core v3+: ledcAttach() replaces ledcSetup() + ledcAttachPin()
    ledcAttach(TFT_BL_PIN, TFT_BL_FREQ_HZ, TFT_BL_RES_BITS);
    ledcWrite(TFT_BL_PIN, 0);

    tft.init();
    tft.setRotation(1);          // Landscape: 320 × 240
    tft.fillScreen(TFT_BLACK);

    currentScreen = ScreenId::Boot;
    initialised   = true;

    // Turn backlight on now that the screen is cleared
    setBacklight(255);
}

// ---------------------------------------------------------------
// setBacklight()
// ---------------------------------------------------------------

void Display::setBacklight(uint8_t brightness)
{
    ledcWrite(TFT_BL_PIN, brightness);
}

// ---------------------------------------------------------------
// render()
// ---------------------------------------------------------------

void Display::render(ScreenId screen,
                     const GnssFix& fix,
                     const Diagnostics& diag,
                     uint32_t nowMs)
{
    if (!initialised) return;

    // Full clear only when transitioning between screens
    if (screen != currentScreen)
    {
        tft.fillScreen(TFT_BLACK);
        currentScreen = screen;
    }

    switch (screen)
    {
        case ScreenId::Boot:
            drawBootScreen(tft, fix, diag);
            break;

        case ScreenId::Status:
            drawStatusScreen(tft, fix, diag);
            break;

        case ScreenId::Navigation:
            drawNavigationScreen(tft, fix, diag);
            break;

        default:
            drawStatusScreen(tft, fix, diag);
            break;
    }

    (void)nowMs; // Reserved for future partial-redraw timing logic
}
