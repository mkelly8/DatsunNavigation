#ifndef MAP_INDEX_H
#define MAP_INDEX_H

#include "map_types.h"

typedef struct {
    int dummy;
} MapIndex;

void mapIndex_init(MapIndex* index);
int mapIndex_findNearest(MapIndex* index, double lat, double lon);

#endif
