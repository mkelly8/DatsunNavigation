#include "map_index.h"
#include "map_store.h"

/*
  File: map_index.cpp
  ----------------------------------------------------
  Brute-force spatial index for route points.

  mapIndex_init: caches the route point count.
  mapIndex_findNearest: linear scan over all route points using
  squared degree-distance (sufficient for nearest-neighbour at
  navigation scale — avoids the cost of a haversine per point).
*/

void mapIndex_init(MapIndex* index)
{
    if (!index) return;
    index->point_count = mapStore_getPointCount();
}

int mapIndex_findNearest(MapIndex* index, double lat, double lon)
{
    if (!index || index->point_count == 0) return -1;

    int    nearest  = 0;
    double bestDist = 1e18;

    for (uint32_t i = 0; i < index->point_count; i++)
    {
        const RoutePoint* p = mapStore_getPointPtr(i);
        if (!p) continue;

        const double dlat = p->geo.latitude  - lat;
        const double dlon = p->geo.longitude - lon;
        const double dist = dlat * dlat + dlon * dlon;

        if (dist < bestDist)
        {
            bestDist = dist;
            nearest  = (int)i;
        }
    }

    return nearest;
}
