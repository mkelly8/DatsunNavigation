#include "ui.h"
/*
  File: ui.cpp
  ----------------------------------------------------
  UI state machine implementation.

  Screen transition logic:
    Boot   -> Status     : immediately on begin()
    Status -> Navigation : when GNSS fix becomes valid (notifyFix(true))
    Navigation -> Status : when GNSS fix is lost (notifyFix(false))
*/

UI::UI()
    : activeScreen(ScreenId::Boot),
      redrawRequested(true)
{
}

void UI::begin()
{
    // TEST BRANCH: force navigation screen regardless of fix status
    activeScreen    = ScreenId::Navigation;
    redrawRequested = true;
}

void UI::tick(uint32_t /*nowMs*/)
{
    // Reserved for button input handling and timed transitions.
}

void UI::notifyFix(bool /*fixValid*/)
{
    // TEST BRANCH: ignore fix status — stay on navigation screen always
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
