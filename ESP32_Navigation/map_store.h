#ifndef MAP_STORE_H
#define MAP_STORE_H

#include <stdbool.h>
#include <stdint.h>
#include "map_types.h"
#include "route_data.h"

static inline const RoutePoint* mapStore_getRoute(void) {
    return route_points;
}

static inline uint32_t mapStore_getPointCount(void) {
    return ROUTE_POINT_COUNT;
}

static inline bool mapStore_getPoint(uint32_t index, RoutePoint* outPoint) {
    if (outPoint == NULL) return false;
    if (index >= ROUTE_POINT_COUNT) return false;
    *outPoint = route_points[index];
    return true;
}

static inline const RoutePoint* mapStore_getPointPtr(uint32_t index) {
    if (index >= ROUTE_POINT_COUNT) return NULL;
    return &route_points[index];
}

#endif
