#include "map_store.h"
#include "route_data.h"

/*
  File: map_store.cpp
  ----------------------------------------------------
  The ONLY translation unit that includes route_data.h.

  Keeping route_data.h out of the shared header prevents the 89 KB
  route array from being duplicated in every translation unit that
  needs route access.
*/

const RoutePoint* mapStore_getRoute(void)
{
    return route_points;
}

uint32_t mapStore_getPointCount(void)
{
    return ROUTE_POINT_COUNT;
}

bool mapStore_getPoint(uint32_t index, RoutePoint* outPoint)
{
    if (outPoint == NULL) return false;
    if (index >= ROUTE_POINT_COUNT) return false;
    *outPoint = route_points[index];
    return true;
}

const RoutePoint* mapStore_getPointPtr(uint32_t index)
{
    if (index >= ROUTE_POINT_COUNT) return NULL;
    return &route_points[index];
}
