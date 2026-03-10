#ifndef DISPLAY_H
#define DISPLAY_H
/*
  File: display.h
  ----------------------------------------------------
  TFT display driver interface.

  Responsibilities:
  - Own the TFT_eSPI instance (one owner: the UI FreeRTOS task)
  - Initialise hardware and backlight
  - Detect screen transitions and issue full clears
  - Dispatch per-frame render to screen-specific functions in screens.cpp

  Do NOT call any method from outside the UI task.
*/

#include <stdint.h>
#include <TFT_eSPI.h>
#include "types.h"

class Display
{
public:
    Display();

    // Initialises TFT and LEDC backlight. Call once from UI task before render().
    void begin();

    // Dispatches to the correct screen draw function.
    // Full screen clear is issued only on screen transitions.
    void render(ScreenId screen,
                const GnssFix& fix,
                const Diagnostics& diag,
                uint32_t nowMs);

    // 0 = backlight off, 255 = full brightness.
    void setBacklight(uint8_t brightness);

private:
    TFT_eSPI tft;
    ScreenId currentScreen;
    bool     initialised;
};

#endif
