#include "screens.h"
/*
  File: screens.cpp
  ----------------------------------------------------
  Screen rendering implementations.

  TFT_eSPI integration is pending. The function bodies below are
  stubs ready for tft.print / tft.drawString calls.

  Serial console output is handled by healthTaskBody ([STATUS] every
  2 s) and the logger ([INFO][NAV] bearing every 1 s).  These screen
  functions intentionally do NOT write to Serial — they run at 50 ms
  (20 FPS) and would flood the monitor.
*/

#include <Arduino.h>

// ---------------------------------------------------------------
// Boot screen — shown once at startup
// ---------------------------------------------------------------
void drawBootScreen(const GnssFix&    /*fix*/,
                    const Diagnostics& /*diag*/,
                    const NavState&    /*nav*/,
                    const CurveScan&   /*curve*/)
{
    // TODO: tft.fillScreen / draw logo / "Initialising..." splash
}

// ---------------------------------------------------------------
// Status screen — GNSS acquisition / health overview
// ---------------------------------------------------------------
void drawStatusScreen(const GnssFix&    /*fix*/,
                      const Diagnostics& /*diag*/,
                      const NavState&    /*nav*/,
                      const CurveScan&   /*curve*/)
{
    // TODO: tft.print satellite count, fix quality, pvt count, frame count
}

// ---------------------------------------------------------------
// Navigation screen — live position, bearing, and curve warning
// ---------------------------------------------------------------
void drawNavigationScreen(const GnssFix&    /*fix*/,
                          const Diagnostics& /*diag*/,
                          const NavState&    /*nav*/,
                          const CurveScan&   /*curve*/)
{
    // TODO: tft.print speed, heading, route index, curve alert box
}
