#ifndef NAV_STATE_H
#define NAV_STATE_H

#include "map_types.h"

typedef struct {
    int current_index;
    double latitude;
    double longitude;
    double speed;
} NavState;

void navState_update(NavState* state, double lat, double lon, double speed);

#endif
