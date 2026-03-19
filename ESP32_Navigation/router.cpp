#include "router.h"
#include "map_store.h"

/*
  File: router.cpp
  ----------------------------------------------------
  Simple sequential route follower.

  Returns the index of the next route point after current_index.
  Returns -1 when the end of the route is reached.
*/

int router_getNextPoint(int current_index)
{
    const uint32_t count = mapStore_getPointCount();
    if (count == 0 || current_index < 0) return -1;

    const int next = current_index + 1;
    if ((uint32_t)next >= count) return -1; // end of route

    return next;
}
