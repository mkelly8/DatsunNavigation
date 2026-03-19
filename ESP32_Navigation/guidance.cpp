#include "guidance.h"
#include "map_store.h"
#include "router.h"
#include <Arduino.h>
#include <math.h>

/*
  File: guidance.cpp
  ----------------------------------------------------
  Guidance computation — bearing from current to next route point.

  Uses local Cartesian coordinates (metres) already stored in each
  RoutePoint to compute the bearing angle. atan2(east, north) gives
  a compass bearing where 0° = north, 90° = east.

  Output is printed over Serial until a display layer is available.
*/

void guidance_compute(int current_index)
{
    const int next_index = router_getNextPoint(current_index);
    if (next_index < 0) return; // end of route

    const RoutePoint* current = mapStore_getPointPtr((uint32_t)current_index);
    const RoutePoint* next    = mapStore_getPointPtr((uint32_t)next_index);
    if (!current || !next) return;

    const double dx = next->local.x - current->local.x; // east delta [m]
    const double dy = next->local.y - current->local.y; // north delta [m]

    double bearing_deg = atan2(dx, dy) * (180.0 / M_PI);
    if (bearing_deg < 0.0) bearing_deg += 360.0;

    Serial.print("[GUIDANCE] bearing=");
    Serial.print(bearing_deg, 1);
    Serial.print(" deg  next_idx=");
    Serial.println(next_index);
}
