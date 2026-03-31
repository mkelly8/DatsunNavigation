#include "map_matcher.h"
#include "map_index.h"

/*
  File: map_matcher.cpp
  ----------------------------------------------------
  Map-matching: snaps a GPS coordinate to the nearest route point.

  Maintains a static MapIndex (initialised once on first call) and
  delegates the search to mapIndex_findNearest().

  Thread safety: the static `initialized` flag is set once during
  first call and never cleared.  This function must only be called
  from a single task (the GNSS task via nav_state.cpp / navigator.cpp)
  to avoid a race on initialization.  All subsequent calls are
  read-only on the MapIndex, which is safe.
*/

int mapMatcher_match(double lat, double lon)
{
    static MapIndex index;
    static bool     initialized = false;

    if (!initialized)
    {
        mapIndex_init(&index);
        initialized = true;
    }

    return mapIndex_findNearest(&index, lat, lon);
}
