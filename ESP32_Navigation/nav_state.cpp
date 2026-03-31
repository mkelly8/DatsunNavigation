#include "nav_state.h"
#include "map_matcher.h"

/*
  File: nav_state.cpp
  ----------------------------------------------------
  Navigation state updater.

  Updates position, speed, and snaps the current route index
  to the nearest map-matched point.
*/

void navState_update(NavState* state, double lat, double lon, double speed)
{
    if (!state) return;

    state->latitude  = lat;
    state->longitude = lon;
    state->speed     = speed;

    const int matched = mapMatcher_match(lat, lon);
    if (matched >= 0)
    {
        state->current_index = matched;
    }
    // matched == -1 means route not loaded; keep previous index.
}
