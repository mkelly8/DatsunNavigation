#ifndef SCREENS_H
#define SCREENS_H
/*
  File: screens.h
  ----------------------------------------------------
  Screen layout definitions.

  Responsibilities:
  - Declare screen rendering functions
  - Keep layout decisions separated from system logic

  Currently a stub for minimum compile.
*/

#include <stdint.h>
#include "types.h"

// NOTE: We intentionally avoid including TFT_eSPI here for now.
// Later these functions will accept a Display/TFT reference.

void drawBootScreen(const GnssFix& fix, const Diagnostics& diag);
void drawStatusScreen(const GnssFix& fix, const Diagnostics& diag);
void drawNavigationScreen(const GnssFix& fix, const Diagnostics& diag);

#endif