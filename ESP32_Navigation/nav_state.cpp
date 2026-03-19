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

    state->current_index = mapMatcher_match(lat, lon);
}
