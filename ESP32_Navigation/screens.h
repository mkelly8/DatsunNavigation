#ifndef SCREENS_H
#define SCREENS_H
/*
  File: screens.h
  ----------------------------------------------------
  Screen layout definitions.

  Responsibilities:
  - Declare screen rendering functions
  - Keep layout decisions separated from system logic

  Each function receives the full navigation context it needs so
  that when TFT_eSPI is integrated, all data is already available
  without changing any call sites.
*/

#include <stdint.h>
#include "types.h"
#include "nav_state.h"
#include "curve_scanner.h"

void drawBootScreen      (const GnssFix& fix, const Diagnostics& diag,
                          const NavState& nav, const CurveScan& curve);

void drawStatusScreen    (const GnssFix& fix, const Diagnostics& diag,
                          const NavState& nav, const CurveScan& curve);

void drawNavigationScreen(const GnssFix& fix, const Diagnostics& diag,
                          const NavState& nav, const CurveScan& curve);

#endif
