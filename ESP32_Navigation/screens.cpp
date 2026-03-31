#include "screens.h"
/*
  File: screens.cpp
  ----------------------------------------------------
  Screen rendering implementations.

  TFT_eSPI hardware calls are not yet integrated. Until then, each
  function emits a structured Serial line so the vehicle data is
  visible and testable over USB.  When TFT_eSPI is added, replace
  the Serial.print blocks with tft.print / tft.drawString calls —
  the function signatures and data passed in are already correct.

  Output rate is controlled by the UI task (every UI_TICK_MS = 50 ms
  → ~20 FPS).  Serial lines are prefixed by screen type so a host
  tool can parse them independently of [STATUS] / [GUIDANCE] lines.
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
    // TFT: draw logo / "Initialising…" splash.
    // Serial stub intentionally silent — boot is a one-shot event and
    // the [READY] line from app.cpp already marks it.
}

// ---------------------------------------------------------------
// Status screen — GNSS acquisition / health overview
// ---------------------------------------------------------------
void drawStatusScreen(const GnssFix&    fix,
                      const Diagnostics& diag,
                      const NavState&    nav,
                      const CurveScan&   /*curve*/)
{
    // TFT: draw satellite count, fix quality, pvt count, frame count.
    Serial.print("[SCREEN:STATUS]");
    Serial.print(" fix=");    Serial.print(fix.valid   ? "Y" : "N");
    Serial.print(" sats=");   Serial.print(fix.satellites);
    Serial.print(" hdop=");   Serial.print(fix.hdop, 1);
    Serial.print(" pvt=");    Serial.print(diag.pvtPackets);
    Serial.print(" frames="); Serial.print(diag.uiFrames);
    Serial.print(" idx=");    Serial.println(nav.current_index);
}

// ---------------------------------------------------------------
// Navigation screen — live position, bearing, and curve warning
// ---------------------------------------------------------------
void drawNavigationScreen(const GnssFix&    fix,
                          const Diagnostics& /*diag*/,
                          const NavState&    nav,
                          const CurveScan&   curve)
{
    // TFT: draw speed, heading, route index, and curve alert box.
    Serial.print("[SCREEN:NAV]");
    Serial.print(" lat=");    Serial.print(fix.latitude,  6);
    Serial.print(" lon=");    Serial.print(fix.longitude, 6);
    Serial.print(" spd=");    Serial.print(fix.speed_mps * 3.6f, 1); // m/s -> km/h
    Serial.print("km/h hdg=");Serial.print(fix.course_deg, 1);
    Serial.print(" idx=");    Serial.print(nav.current_index);

    if (curve.found)
    {
        const char* dir = (curve.segment.direction == TURN_LEFT)  ? "LEFT"
                        : (curve.segment.direction == TURN_RIGHT) ? "RIGHT"
                        :                                           "STRAIGHT";

        Serial.print(" >> CURVE ");
        Serial.print(dir);
        Serial.print(" in ");
        Serial.print(curve.distance_m, 0);
        Serial.print("m");

        // Speed-based time warning
        if (curve.time_to_curve_s > 0.0f)
        {
            Serial.print(" (");
            Serial.print(curve.time_to_curve_s, 1);
            Serial.print("s)");

            // Urgency indicator
            if      (curve.time_to_curve_s < 10.0f) Serial.print(" !!BRAKE!!");
            else if (curve.time_to_curve_s < 20.0f) Serial.print(" !PREPARE!");
        }

        Serial.print(" r=");
        Serial.print(curve.segment.min_radius_m, 1);
        Serial.print("m");
    }
    else
    {
        Serial.print(" >> clear 3km");
    }

    Serial.println();
}
