#include "guidance.h"
#include "map_store.h"
#include "router.h"
#include "geo_math.h"
#include "logger.h"
#include <math.h>

/*
  File: guidance.cpp
  ----------------------------------------------------
  Guidance computation — bearing from current to next route point.

  Uses the forward azimuth (great-circle bearing) formula from
  current geo position to the next route point:

    bearing = atan2(sin(Δlon)·cos(lat2),
                    cos(lat1)·sin(lat2) − sin(lat1)·cos(lat2)·cos(Δlon))

  NOTE: route_data.h only stores geo coordinates; local.x/y are not
  populated, so bearing is computed from lat/lon directly.

  Output is printed over Serial until a display layer is available.
*/

// Use shared constants from geo_math.h — no local copies needed.

void guidance_compute(int current_index, TravelDirection direction)
{
    const int next_index = router_getNextPoint(current_index, direction);
    if (next_index < 0) return; // end of route

    const RoutePoint* current = mapStore_getPointPtr((uint32_t)current_index);
    const RoutePoint* next    = mapStore_getPointPtr((uint32_t)next_index);
    if (!current || !next) return;

    const double lat1 = current->geo.latitude  * GEO_DEG_TO_RAD;
    const double lon1 = current->geo.longitude * GEO_DEG_TO_RAD;
    const double lat2 = next->geo.latitude     * GEO_DEG_TO_RAD;
    const double lon2 = next->geo.longitude    * GEO_DEG_TO_RAD;
    const double dlon = lon2 - lon1;

    const double y = sin(dlon) * cos(lat2);
    const double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dlon);

    double bearing_deg = atan2(y, x) * GEO_RAD_TO_DEG;
    if (bearing_deg < 0.0) bearing_deg += 360.0;

    logger_logf(LOG_INFO, "NAV", "bearing=%.1f deg  next_idx=%d",
                bearing_deg, next_index);
}
