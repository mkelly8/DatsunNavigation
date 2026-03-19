#include "map_renderer.h"
#include "map_store.h"
#include <Arduino.h>

/*
  File: map_renderer.cpp
  ----------------------------------------------------
  Map renderer — draws the route around the current position.

  TFT_eSPI integration is pending (display.h/.cpp stub must be completed
  first). Until then, the current map position is emitted over Serial
  so host-side tools can visualise the route during development.
*/

void mapRenderer_draw(int current_index)
{
    const RoutePoint* p = mapStore_getPointPtr((uint32_t)current_index);
    if (!p) return;

    Serial.print("[MAP] index=");
    Serial.print(current_index);
    Serial.print(" lat=");
    Serial.print(p->geo.latitude, 6);
    Serial.print(" lon=");
    Serial.println(p->geo.longitude, 6);
}
