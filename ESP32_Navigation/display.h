#ifndef DISPLAY_H
#define DISPLAY_H
/*
  File: display.h
  ----------------------------------------------------
  TFT display interface.

  Responsibilities:
  - Initialize display hardware
  - Provide a render entry point that accepts the full navigation context

  NOTE: TFT_eSPI integration is pending. Until then, render() delegates
  to screen functions that emit structured Serial lines.
*/

#include <stdint.h>
#include <TFT_eSPI.h>
#include "types.h"
#include "nav_state.h"
#include "curve_scanner.h"

class Display
{
public:
    Display();

    void begin();

    // Render the current screen. All navigation data is passed in so
    // the screen functions have everything they need when TFT is added.
    void render(ScreenId           screen,
                const GnssFix&     fix,
                const Diagnostics& diag,
                const NavState&    nav,
                const CurveScan&   curve,
                uint32_t           nowMs);

private:
    uint32_t lastRenderMs;
};

#endif // DISPLAY_H
