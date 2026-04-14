#include "screens.h"
/*
  File: screens.cpp
  ----------------------------------------------------
  Screen rendering — ILI9341 landscape (320 x 240) via TFT_eSPI.

  Flicker-free strategy:
    Each screen does a full fillScreen ONCE (on first draw or after a
    screen transition), draws static labels, then returns.
    On every subsequent frame only the value regions are erased and
    redrawn — the static chrome stays untouched.
*/

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "curvature.h"

extern TFT_eSPI tft;   // defined in display.cpp

// Tracks which screen last painted the background so each screen
// knows whether it needs to do a full initialisation pass.
static ScreenId s_activeScreen = ScreenId::Boot;


// ---------------------------------------------------------------
// Boot screen — shown once at startup
// ---------------------------------------------------------------
void drawBootScreen(const GnssFix&     /*fix*/,
                    const Diagnostics& /*diag*/,
                    const NavState&    /*nav*/,
                    const CurveScan&   /*curve*/)
{
    tft.fillScreen(TFT_BLACK);
    s_activeScreen = ScreenId::Boot;

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
//
// Static chrome : header, divider, row labels
// Dynamic values: satellite count, fix type, HDOP, PVT packets
// ---------------------------------------------------------------
void drawStatusScreen(const GnssFix&     fix,
                      const Diagnostics& diag,
                      const NavState&    /*nav*/,
                      const CurveScan&   /*curve*/)
{
    char buf[40];
    const bool fresh = (s_activeScreen != ScreenId::Status);

    if (fresh)
    {
        s_activeScreen = ScreenId::Status;
        tft.fillScreen(TFT_BLACK);

        // Static chrome
        tft.setTextDatum(TL_DATUM);
        tft.setTextColor(TFT_CYAN, TFT_BLACK);
        tft.setTextSize(2);
        tft.drawString("GPS STATUS", 5, 5);

        tft.drawFastHLine(0, 28, 320, TFT_DARKGREY);

        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        tft.drawString("Satellites:", 5, 38);
        tft.drawString("Fix:",        5, 62);
        tft.drawString("HDOP:",       5, 86);
        tft.drawString("PVT Pkts:",   5, 110);

        tft.setTextDatum(MC_DATUM);
        tft.setTextSize(1);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.drawString("Searching for satellite signal...", 160, 190);
    }

    // ---- Dynamic values -----------------------------------------
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);

    // Values are drawn with a black background fill so old digits
    // are overwritten without needing to erase the whole screen.
    snprintf(buf, sizeof(buf), "%-4d", fix.satellites);
    tft.drawString(buf, 195, 38);

    const char* fixLabel = (fix.fixQuality == 3) ? "3D" :
                           (fix.fixQuality == 2) ? "2D" :
                           (fix.fixQuality == 4) ? "DR" : "--";
    snprintf(buf, sizeof(buf), "%-4s", fixLabel);
    tft.drawString(buf, 195, 62);

    snprintf(buf, sizeof(buf), "%-6.1f", fix.hdop);
    tft.drawString(buf, 195, 86);

    snprintf(buf, sizeof(buf), "%-8lu", (unsigned long)diag.pvtPackets);
    tft.drawString(buf, 195, 110);

    // Frame counter (top-right, small)
    tft.setTextDatum(TR_DATUM);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.setTextSize(1);
    snprintf(buf, sizeof(buf), "F:%-6lu", (unsigned long)diag.uiFrames);
    tft.drawString(buf, 315, 10);
}


// ---------------------------------------------------------------
// Navigation screen
//
// Layout (320 x 240 landscape):
//   Top bar  : SAT count (left) | flashing CURVE warning (right)
//   Centre   : Speed value (large) + static "km/h" label
//   Mid      : Distance to curve (when found)
//   Lower    : Direction arrow + LEFT / RIGHT label (when found)
//   Bottom   : Ay lateral-g value
//
// Static chrome (drawn once): dividers, "km/h" label
// Dynamic regions (erased + redrawn each frame):
//   [0,0  120x30] SAT value
//   [228,0  92x30] warning box
//   [40,38 240x62] speed digits
//   [0,120 320x22] distance text
//   [60,148 200x62] arrow + direction label
//   [60,214 200x22] Ay value
// ---------------------------------------------------------------
void drawNavigationScreen(const GnssFix&     fix,
                          const Diagnostics& /*diag*/,
                          const NavState&    nav,
                          const CurveScan&   curve)
{
    char buf[48];
    const bool fresh = (s_activeScreen != ScreenId::Navigation);

    if (fresh)
    {
        s_activeScreen = ScreenId::Navigation;
        tft.fillScreen(TFT_BLACK);

        // Static chrome
        tft.drawFastHLine(0, 32,  320, TFT_DARKGREY);
        tft.drawFastHLine(0, 210, 320, TFT_DARKGREY);

        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        tft.setTextSize(2);
        tft.drawString("km/h", 160, 108);
    }

    // ---- Compute Ay at current map position ---------------------
    const double radius = curvature_computeRadius(nav.current_index);
    const float  ay     = (float)curvature_computeAy((double)fix.speed_mps, radius);

    // ---- SAT count (top-left) -----------------------------------
    tft.fillRect(0, 0, 120, 30, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    snprintf(buf, sizeof(buf), "SAT:%d", fix.satellites);
    tft.drawString(buf, 5, 6);

    // ---- Flashing warning (top-right) ---------------------------
    if (curve.found && ((millis() % 800) < 400))
    {
        tft.fillRect(228, 2, 90, 26, TFT_RED);
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(TFT_WHITE, TFT_RED);
        tft.setTextSize(2);
        tft.drawString("CURVE!", 273, 15);
    }
    else
    {
        tft.fillRect(228, 2, 90, 26, TFT_BLACK);
    }

    // ---- Speed value (large, centred) ---------------------------
    tft.fillRect(40, 38, 240, 62, TFT_BLACK);
    const int speedKmh = (int)(fix.speed_mps * 3.6f);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(6);
    snprintf(buf, sizeof(buf), "%d", speedKmh);
    tft.drawString(buf, 160, 72);

    // ---- Distance to curve --------------------------------------
    tft.fillRect(0, 120, 320, 22, TFT_BLACK);
    if (curve.found)
    {
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.setTextSize(2);
        snprintf(buf, sizeof(buf), "%.0f m to curve", curve.distance_m);
        tft.drawString(buf, 160, 132);
    }

    // ---- Direction arrow + label --------------------------------
    tft.fillRect(60, 148, 200, 62, TFT_BLACK);
    if (curve.found)
    {
        const bool     isLeft     = (curve.segment.direction == TURN_LEFT);
        const uint16_t arrowColor = isLeft ? TFT_CYAN : TFT_ORANGE;

        // Shaft (2 px thick)
        tft.drawFastHLine(100, 163, 120, arrowColor);
        tft.drawFastHLine(100, 164, 120, arrowColor);

        if (isLeft)
            tft.fillTriangle(80, 163, 108, 146, 108, 181, arrowColor);
        else
            tft.fillTriangle(240, 163, 212, 146, 212, 181, arrowColor);

        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(arrowColor, TFT_BLACK);
        tft.setTextSize(3);
        tft.drawString(isLeft ? "LEFT" : "RIGHT", 160, 196);
    }

    // ---- Ay value (bottom centre) -------------------------------
    tft.fillRect(60, 214, 200, 22, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(2);
    snprintf(buf, sizeof(buf), "Ay: %.2f g", ay);
    tft.drawString(buf, 160, 224);
}
