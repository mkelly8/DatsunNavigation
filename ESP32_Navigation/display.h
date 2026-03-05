#ifndef DISPLAY_H
#define DISPLAY_H
/*
  File: display.h
  ----------------------------------------------------
  TFT display interface.

  Responsibilities:
  - Initialize display hardware
  - Provide a render entry point

  NOTE:
  - This is a stub for minimum compile.
  - TFT_eSPI integration will be added later.
*/

#include <stdint.h>
#include "types.h"

class Display
{
public:
    Display();

    void begin();

    // Render based on current UI screen selection
    void render(ScreenId screen,
                const GnssFix& fix,
                const Diagnostics& diag,
                uint32_t nowMs);

private:
    uint32_t lastRenderMs;
};

#endif