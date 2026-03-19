#include "curvature.h"
#include "map_store.h"
#include <math.h>

/*
  File: curvature.cpp
  ----------------------------------------------------
  Curvature calculations for route analysis.

  curvature_computeRadius: uses the circumscribed-circle (Menger curvature)
  formula on three consecutive route points expressed in local Cartesian metres.

  curvature_computeAy: returns lateral acceleration v²/r given speed (m/s)
  and radius (m).
*/

double curvature_computeRadius(int index)
{
    const uint32_t count = mapStore_getPointCount();

    if (count < 3 || index < 1 || index >= (int)count - 1)
    {
        return 1e9; // treat as straight
    }

    const RoutePoint* p0 = mapStore_getPointPtr((uint32_t)(index - 1));
    const RoutePoint* p1 = mapStore_getPointPtr((uint32_t)(index));
    const RoutePoint* p2 = mapStore_getPointPtr((uint32_t)(index + 1));

    if (!p0 || !p1 || !p2) return 1e9;

    // Local Cartesian coordinates (metres)
    const double ax = p0->local.x, ay = p0->local.y;
    const double bx = p1->local.x, by = p1->local.y;
    const double cx = p2->local.x, cy = p2->local.y;

    // Triangle area via cross product
    const double area = fabs((bx - ax) * (cy - ay) - (cx - ax) * (by - ay)) * 0.5;
    if (area < 1e-9) return 1e9; // collinear — effectively infinite radius

    const double ab = sqrt((bx - ax)*(bx - ax) + (by - ay)*(by - ay));
    const double bc = sqrt((cx - bx)*(cx - bx) + (cy - by)*(cy - by));
    const double ca = sqrt((ax - cx)*(ax - cx) + (ay - cy)*(ay - cy));

    // Circumradius R = (a * b * c) / (4 * area)
    return (ab * bc * ca) / (4.0 * area);
}

double curvature_computeAy(double v, double r)
{
    if (r < 1e-9) return 0.0;
    return (v * v) / r; // lateral acceleration [m/s²]
}
