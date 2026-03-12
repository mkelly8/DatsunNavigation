#ifndef MAP_TYPES_H
#define MAP_TYPES_H

#include <stdint.h>

/* -----------------------------
   Map and Route Configuration
------------------------------*/

#define ROUTE_POINT_SPACING_M 10      // Route points spaced every 10 meters
#define MAX_ROUTE_POINTS      50000   // Maximum route size

/* -----------------------------
   Geographic Coordinate
------------------------------*/

typedef struct
{
    double latitude;      // degrees
    double longitude;     // degrees
} GeoCoordinate;


/* -----------------------------
   Local Cartesian Coordinate
------------------------------*/

typedef struct
{
    double x;   // meters east
    double y;   // meters north
} LocalPoint;


/* -----------------------------
   Route Point
------------------------------*/

typedef struct
{
    GeoCoordinate geo;    // original GPS coordinate
    LocalPoint    local;  // converted local coordinate (meters)
} RoutePoint;


/* -----------------------------
   Route Container
------------------------------*/

typedef struct
{
    RoutePoint points[MAX_ROUTE_POINTS];
    uint32_t point_count;
} Route;


/* -----------------------------
   Curve Direction
------------------------------*/

typedef enum
{
    TURN_NONE = 0,
    TURN_LEFT = 1,
    TURN_RIGHT = -1

} TurnDirection;


/* -----------------------------
   Curve Segment
------------------------------*/

typedef struct
{
    uint32_t entry_index;
    uint32_t midpoint_index;
    uint32_t exit_index;

    TurnDirection direction;

    double min_radius_m;

} CurveSegment;

#endif