#ifndef GEO_MATH_H
#define GEO_MATH_H
/*
  File: geo_math.h
  ----------------------------------------------------
  Shared geographic and mathematical constants.

  Single definition point for constants used across curvature.cpp,
  curve_scanner.cpp, and guidance.cpp.  Include this header instead
  of defining local copies.
*/

// Angle conversion
#define GEO_DEG_TO_RAD         (3.14159265358979323846 / 180.0)
#define GEO_RAD_TO_DEG         (180.0 / 3.14159265358979323846)

// Flat-earth approximation: metres per degree of latitude (~constant globally)
#define GEO_METRES_PER_DEG_LAT  111320.0

// Sentinel radius returned when three points are collinear (straight road)
#define GEO_STRAIGHT_RADIUS_M   1.0e9

#endif // GEO_MATH_H
