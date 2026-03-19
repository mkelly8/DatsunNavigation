#ifndef MAP_STORE_H
#define MAP_STORE_H

#include <stdbool.h>
#include <stdint.h>
#include "map_types.h"

/*
  File: map_store.h
  ----------------------------------------------------
  Route data accessor interface.

  IMPORTANT: This header deliberately does NOT include route_data.h.
  Only map_store.cpp includes route_data.h, ensuring the large static
  route array exists in exactly one translation unit. All other files
  access route data exclusively through these functions.
*/

const RoutePoint* mapStore_getRoute(void);
uint32_t          mapStore_getPointCount(void);
bool              mapStore_getPoint(uint32_t index, RoutePoint* outPoint);
const RoutePoint* mapStore_getPointPtr(uint32_t index);

#endif
