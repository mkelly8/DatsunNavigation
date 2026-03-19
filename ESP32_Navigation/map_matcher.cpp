#include "map_matcher.h"
#include "map_index.h"

/*
  File: map_matcher.cpp
  ----------------------------------------------------
  Map-matching: snaps a GPS coordinate to the nearest route point.

  Maintains a static MapIndex (initialised once on first call) and
  delegates the search to mapIndex_findNearest().
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
