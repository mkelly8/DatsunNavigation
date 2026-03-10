#include "screens.h"
/*
  File: screens.cpp
  ----------------------------------------------------
  Screen rendering implementations.

  Layout reference (ILI9341, landscape 320 × 240):
    x: 0 (left) → 319 (right)
    y: 0 (top)  → 239 (bottom)

  Font sizes (TFT_eSPI built-in):
    Font 1:  8 px  — tiny labels
    Font 2: 16 px  — body text
    Font 4: 26 px  — headings
    Font 6: 48 px  — large numerics (digits + colon only)
    Font 7: 48 px  — 7-segment style numerics
*/

#include <stdio.h>

// ---------------------------------------------------------------
// Boot screen — shown during startup / GNSS acquisition
// ---------------------------------------------------------------

void drawBootScreen(TFT_eSPI& tft, const GnssFix& fix, const Diagnostics& diag)
{
    tft.setTextDatum(MC_DATUM); // Middle-centre anchor
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.drawString("Datsun Navigation", tft.width() / 2, 100, 4);

    // Show GNSS acquisition progress
    const char* status = fix.valid ? "Fix acquired" : "Acquiring GNSS...";
    tft.setTextColor(fix.valid ? TFT_GREEN : TFT_YELLOW, TFT_BLACK);
    tft.drawString(status, tft.width() / 2, 145, 2);

    // Satellite count
    char buf[32];
    snprintf(buf, sizeof(buf), "Sats: %d", fix.satellites);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString(buf, tft.width() / 2, 168, 2);
}

// ---------------------------------------------------------------
// Status screen — diagnostic / GNSS data view
// ---------------------------------------------------------------

void drawStatusScreen(TFT_eSPI& tft, const GnssFix& fix, const Diagnostics& diag)
{
    // Always set both fg and bg so text overwrites cleanly each frame
    tft.setTextDatum(TL_DATUM); // Top-left anchor for all rows
    char buf[48];

    // Row 1 — fix status + satellites
    snprintf(buf, sizeof(buf), "Fix: %-3s  Sats: %2d  HDOP: %.1f",
             fix.valid ? "YES" : "NO", fix.satellites, fix.hdop);
    tft.setTextColor(fix.valid ? TFT_GREEN : TFT_RED, TFT_BLACK);
    tft.drawString(buf, 8, 8, 2);

    // Divider
    tft.drawFastHLine(0, 30, tft.width(), TFT_DARKGREY);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    if (fix.valid)
    {
        // Row 2 — latitude
        snprintf(buf, sizeof(buf), "Lat:  %.6f", fix.latitude);
        tft.drawString(buf, 8, 38, 2);

        // Row 3 — longitude
        snprintf(buf, sizeof(buf), "Lon: %.6f", fix.longitude);
        tft.drawString(buf, 8, 60, 2);

        // Row 4 — speed
        snprintf(buf, sizeof(buf), "Speed: %.1f km/h", fix.speed_mps * 3.6f);
        tft.drawString(buf, 8, 82, 2);

        // Row 5 — heading
        snprintf(buf, sizeof(buf), "Heading: %.1f deg", fix.course_deg);
        tft.drawString(buf, 8, 104, 2);
    }
    else
    {
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.drawString("Waiting for GNSS fix...", 8, 38, 2);
    }

    // Divider
    tft.drawFastHLine(0, 128, tft.width(), TFT_DARKGREY);

    // Row 6 — diagnostics footer
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    snprintf(buf, sizeof(buf), "PVT pkts: %lu   Frames: %lu",
             (unsigned long)diag.pvtPackets,
             (unsigned long)diag.uiFrames);
    tft.drawString(buf, 8, 136, 1);
}

// ---------------------------------------------------------------
// Navigation screen — stub, pending NavState from teammate
// ---------------------------------------------------------------

void drawNavigationScreen(TFT_eSPI& tft, const GnssFix& fix, const Diagnostics& diag)
{
    // TODO: implement once NavState struct is agreed with teammate.
    // For now fall through to status view so the screen is not blank.
    drawStatusScreen(tft, fix, diag);
}
