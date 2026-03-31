#ifndef UI_H
#define UI_H
/*
  File: ui.h
  ----------------------------------------------------
  UI state machine interface.

  Responsibilities:
  - Track active screen
  - Handle transitions
  - Trigger redraw decisions

  Currently a stub for minimum compile.
*/

#include <stdint.h>
#include "types.h"

class UI
{
public:
    UI();

    void begin();
    void tick(uint32_t nowMs);

    // Called each UI frame with the current fix validity so the state
    // machine can transition Boot -> Status -> Navigation automatically.
    void notifyFix(bool fixValid);

    ScreenId getActiveScreen() const;
    bool consumeRedrawRequested();

private:
    ScreenId activeScreen;
    bool redrawRequested;
};

#endif