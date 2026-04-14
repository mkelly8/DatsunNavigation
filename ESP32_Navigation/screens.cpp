#include "screens.h"
/*
  File: screens.cpp
  ----------------------------------------------------
  Screen rendering — ILI9341 landscape (320 x 240) via TFT_eSPI.

  Layout summary:
    Boot       : splash logo + "Initialising..."
    Status     : GPS health (satellites, fix quality, HDOP, packet counts)
    Navigation : large speed + compass heading, route index, curve alert box
*/

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "config.h"

extern TFT_eSPI tft;   // defined in display.cpp

// ---------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------

static const char* compassDir(float deg)
{
    static const char* dirs[] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" };
    int idx = (int)((deg + 22.5f) / 45.0f) % 8;
    return dirs[idx < 0 ? idx + 8 : idx];
}


// ---------------------------------------------------------------
// Boot screen — shown once at startup
// ---------------------------------------------------------------
void drawBootScreen(const GnssFix&     /*fix*/,
                    const Diagnostics& /*diag*/,
                    const NavState&    /*nav*/,
                    const CurveScan&   /*curve*/)
{
    tft.fillScreen(TFT_BLACK);

    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.drawString("DATSUN NAV", 160, 95);

    tft.setTextSize(2);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.drawString("Initialising...", 160, 145);
}


// ---------------------------------------------------------------
// Status screen — GNSS acquisition / health overview
// ---------------------------------------------------------------
void drawStatusScreen(const GnssFix&     fix,
                      const Diagnostics& diag,
                      const NavState&    /*nav*/,
                      const CurveScan&   /*curve*/)
{
    tft.fillScreen(TFT_BLACK);

    char buf[40];

    // Header
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawString("GPS STATUS", 5, 5);

    // Frame count (top-right)
    tft.setTextDatum(TR_DATUM);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.setTextSize(1);
    snprintf(buf, sizeof(buf), "F:%lu", (unsigned long)diag.uiFrames);
    tft.drawString(buf, 315, 10);

    tft.drawFastHLine(0, 28, 320, TFT_DARKGREY);

    // Stats
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);

    snprintf(buf, sizeof(buf), "Satellites:  %d", fix.satellites);
    tft.drawString(buf, 5, 38);

    const char* fixLabel = (fix.fixQuality == 3) ? "3D" :
                           (fix.fixQuality == 2) ? "2D" :
                           (fix.fixQuality == 4) ? "DR" : "--";
    snprintf(buf, sizeof(buf), "Fix:         %s", fixLabel);
    tft.drawString(buf, 5, 62);

    snprintf(buf, sizeof(buf), "HDOP:        %.1f", fix.hdop);
    tft.drawString(buf, 5, 86);

    snprintf(buf, sizeof(buf), "PVT Packets: %lu", (unsigned long)diag.pvtPackets);
    tft.drawString(buf, 5, 110);

    // Acquiring message
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(1);
    tft.drawString("Searching for satellite signal...", 160, 190);
}


// ---------------------------------------------------------------
// Navigation screen — live position, bearing, and curve warning
// ---------------------------------------------------------------
void drawNavigationScreen(const GnssFix&     fix,
                          const Diagnostics& /*diag*/,
                          const NavState&    nav,
                          const CurveScan&   curve)
{
    tft.fillScreen(TFT_BLACK);

    char buf[48];

    // ---- Speed (large, top-left) --------------------------------
    int speedKmh = (int)(fix.speed_mps * 3.6f);
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(5);
    snprintf(buf, sizeof(buf), "%3d", speedKmh);
    tft.drawString(buf, 5, 5);

    tft.setTextSize(2);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.drawString("km/h", 12, 55);

    // ---- Compass heading (top-right) ----------------------------
    tft.setTextDatum(TR_DATUM);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(3);
    tft.drawString(compassDir(fix.course_deg), 315, 8);

    tft.setTextSize(1);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    snprintf(buf, sizeof(buf), "%.0f deg", fix.course_deg);
    tft.drawString(buf, 315, 50);

    // ---- Divider ------------------------------------------------
    tft.drawFastHLine(0, 76, 320, TFT_DARKGREY);

    // ---- Route info ---------------------------------------------
    const char* dirStr = (nav.direction == TRAVEL_FORWARD)  ? "FWD" :
                         (nav.direction == TRAVEL_BACKWARD) ? "REV" : "---";
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    snprintf(buf, sizeof(buf), "Route: %d   Dir: %s", nav.current_index, dirStr);
    tft.drawString(buf, 5, 83);

    // ---- Curve alert box ----------------------------------------
    if (curve.found)
    {
        const char* turnStr = (curve.segment.direction == TURN_LEFT)  ? "LEFT"  :
                              (curve.segment.direction == TURN_RIGHT) ? "RIGHT" : "CURVE";

        tft.fillRoundRect(5, 103, 310, 78, 6, TFT_NAVY);
        tft.drawRoundRect(5, 103, 310, 78, 6, TFT_YELLOW);

        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(TFT_YELLOW, TFT_NAVY);
        tft.setTextSize(2);
        snprintf(buf, sizeof(buf), "CURVE AHEAD - %s", turnStr);
        tft.drawString(buf, 160, 124);

        tft.setTextColor(TFT_WHITE, TFT_NAVY);
        if (curve.time_to_curve_s > 0.0f && curve.distance_m <= NAV_TIME_DISPLAY_DIST_M)
        {
            snprintf(buf, sizeof(buf), "%.0f m   |   %.0f s", curve.distance_m, curve.time_to_curve_s);
        }
        else
        {
            snprintf(buf, sizeof(buf), "%.0f m", curve.distance_m);
        }
        tft.drawString(buf, 160, 156);
    }
}
