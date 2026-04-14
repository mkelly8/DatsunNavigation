#include "screens.h"
/*
  File: screens.cpp
  ----------------------------------------------------
  Screen rendering — ILI9341 portrait (240 x 320) via TFT_eSPI.
  setRotation(0) → width=240, height=320, centre X=120.

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

// Tracks which screen last painted the background.
static ScreenId s_activeScreen = ScreenId::Boot;


// ---------------------------------------------------------------
// Boot screen — shown once at startup
// ---------------------------------------------------------------
void drawBootScreen(const GnssFix&     /*fix*/,
                    const Diagnostics& /*diag*/,
                    const NavState&    /*nav*/,
                    const CurveScan&   /*curve*/)
{
    s_activeScreen = ScreenId::Boot;
    tft.fillScreen(TFT_BLACK);

    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.drawString("DATSUN NAV", 120, 140);

    tft.setTextSize(2);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.drawString("Initialising...", 120, 190);
}


// ---------------------------------------------------------------
// Status screen — GNSS acquisition / health overview
//
// 240 x 320 portrait layout:
//   Header + divider (static)
//   Row labels (static): Satellites, Fix, HDOP, PVT Pkts
//   Values (dynamic): right-column, overwritten each frame
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

        tft.drawFastHLine(0, 28, 240, TFT_DARKGREY);

        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        tft.drawString("Satellites:", 5, 38);
        tft.drawString("Fix:",        5, 62);
        tft.drawString("HDOP:",       5, 86);
        tft.drawString("PVT Pkts:",   5, 110);

        tft.setTextDatum(MC_DATUM);
        tft.setTextSize(1);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.drawString("Searching for signal...", 120, 220);
    }

    // ---- Dynamic values (drawn with black bg to erase old digits) ---
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);

    snprintf(buf, sizeof(buf), "%-4d",  fix.satellites);
    tft.drawString(buf, 150, 38);

    const char* fixLabel = (fix.fixQuality == 3) ? "3D" :
                           (fix.fixQuality == 2) ? "2D" :
                           (fix.fixQuality == 4) ? "DR" : "--";
    snprintf(buf, sizeof(buf), "%-4s",  fixLabel);
    tft.drawString(buf, 150, 62);

    snprintf(buf, sizeof(buf), "%-6.1f", fix.hdop);
    tft.drawString(buf, 150, 86);

    snprintf(buf, sizeof(buf), "%-8lu", (unsigned long)diag.pvtPackets);
    tft.drawString(buf, 150, 110);

    // Frame counter top-right
    tft.setTextDatum(TR_DATUM);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.setTextSize(1);
    snprintf(buf, sizeof(buf), "F:%-6lu", (unsigned long)diag.uiFrames);
    tft.drawString(buf, 235, 10);
}


// ---------------------------------------------------------------
// Navigation screen — 240 x 320 portrait
//
// Zone layout (total 320 px):
//   [y=  0- 33]  Top bar : SAT (left) | CURVE warning (right)
//   [y= 34     ]  Divider
//   [y= 35-129]  Speed value (size 6, centred) + "km/h" label
//   [y=130-162]  Distance to curve
//   [y=163-282]  Direction arrow + LEFT/RIGHT label
//   [y=283     ]  Divider
//   [y=284-320]  Ay value
//
// Static chrome (once): dividers, "km/h"
// Dynamic regions erased per frame:
//   SAT      fillRect(  0,  0, 140,  34)
//   Warning  fillRect(148,  0,  92,  34)
//   Speed    fillRect(  0, 35, 240,  92)
//   Distance fillRect(  0,130, 240,  32)
//   Arrow    fillRect(  0,163, 240, 120)
//   Ay       fillRect(  0,284, 240,  36)
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
        tft.drawFastHLine(0,  34, 240, TFT_DARKGREY);
        tft.drawFastHLine(0, 283, 240, TFT_DARKGREY);

        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        tft.setTextSize(2);
        tft.drawString("km/h", 120, 120);
    }

    // ---- TEST OVERRIDES — remove before merging to Test---AL ----
    const float   ay           = 3.5f;
    CurveScan     testCurve    = curve;
    testCurve.found             = true;
    testCurve.segment.direction = TURN_RIGHT;
    testCurve.distance_m        = 50.0f;
    const CurveScan& disp       = testCurve;
    // ---- end overrides ------------------------------------------

    // ---- SAT count (top-left) -----------------------------------
    tft.fillRect(0, 0, 140, 34, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    snprintf(buf, sizeof(buf), "SAT:%d", fix.satellites);
    tft.drawString(buf, 5, 9);

    // ---- Flashing warning (top-right) ---------------------------
    if (disp.found && ((millis() % 800) < 400))
    {
        tft.fillRect(148, 2, 90, 28, TFT_RED);
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(TFT_WHITE, TFT_RED);
        tft.setTextSize(2);
        tft.drawString("CURVE!", 193, 16);
    }
    else
    {
        tft.fillRect(148, 2, 90, 28, TFT_BLACK);
    }

    // ---- Speed value (large, centred) ---------------------------
    tft.fillRect(0, 35, 240, 78, TFT_BLACK);
    const int speedKmh = (int)(fix.speed_mps * 3.6f);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(6);
    snprintf(buf, sizeof(buf), "%d", speedKmh);
    tft.drawString(buf, 120, 82);

    // ---- Distance to curve --------------------------------------
    tft.fillRect(0, 130, 240, 32, TFT_BLACK);
    if (disp.found)
    {
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.setTextSize(2);
        snprintf(buf, sizeof(buf), "%.0f m to curve", disp.distance_m);
        tft.drawString(buf, 120, 146);
    }

    // ---- Direction arrow + label --------------------------------
    tft.fillRect(0, 163, 240, 120, TFT_BLACK);
    if (disp.found)
    {
        const bool     isLeft     = (disp.segment.direction == TURN_LEFT);
        const uint16_t arrowColor = isLeft ? TFT_CYAN : TFT_ORANGE;

        // Shaft (2 px thick), centred vertically at y=218
        tft.drawFastHLine(45, 218, 150, arrowColor);
        tft.drawFastHLine(45, 219, 150, arrowColor);

        if (isLeft)
        {
            // Tip at left: (25,218), base corners (52,200) (52,237)
            tft.fillTriangle(25, 218, 52, 200, 52, 237, arrowColor);
        }
        else
        {
            // Tip at right: (215,218), base corners (188,200) (188,237)
            tft.fillTriangle(215, 218, 188, 200, 188, 237, arrowColor);
        }

        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(arrowColor, TFT_BLACK);
        tft.setTextSize(3);
        tft.drawString(isLeft ? "LEFT" : "RIGHT", 120, 260);
    }

    // ---- Ay value (bottom centre) -------------------------------
    tft.fillRect(0, 284, 240, 36, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(2);
    snprintf(buf, sizeof(buf), "Ay: %.2f g", ay);
    tft.drawString(buf, 120, 302);
}
