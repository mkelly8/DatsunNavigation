#ifndef SCREENS_H
#define SCREENS_H
/*
  File: screens.h
  ----------------------------------------------------
  Screen rendering function declarations.

  Each function owns one full screen layout.
  TFT_eSPI& is passed in so screens.cpp has no global state.

  Rules:
  - Use setTextColor(fg, bg) — always set a background colour so
    text rewrites cleanly without artefacts or leftover pixels.
  - Do NOT call fillScreen() — display.cpp handles full clears
    on screen transitions.
  - Use char[] buffers for formatted strings, not String objects.
*/

#include <TFT_eSPI.h>
#include "types.h"

void drawBootScreen      (TFT_eSPI& tft, const GnssFix& fix, const Diagnostics& diag);
void drawStatusScreen    (TFT_eSPI& tft, const GnssFix& fix, const Diagnostics& diag);
void drawNavigationScreen(TFT_eSPI& tft, const GnssFix& fix, const Diagnostics& diag);

#endif
