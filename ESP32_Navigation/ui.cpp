#include "ui.h"
/*
  File: ui.cpp
  ----------------------------------------------------
  UI state machine implementation.

  Currently a stub for minimum compile.
*/

UI::UI()
    : activeScreen(ScreenId::Boot),
      redrawRequested(true)
{
}

void UI::begin()
{
    // Placeholder: initialize UI state, inputs, etc.
    activeScreen = ScreenId::Status;
    redrawRequested = true;
}

void UI::tick(uint32_t /*nowMs*/)
{
    // Placeholder: later handle button inputs / screen switching.
}

ScreenId UI::getActiveScreen() const
{
    return activeScreen;
}

bool UI::consumeRedrawRequested()
{
    const bool wasRequested = redrawRequested;
    redrawRequested = false;
    return wasRequested;
}