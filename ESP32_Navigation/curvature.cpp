#include "curvature.h"
#include "map_store.h"
#include <math.h>

/*
  File: curvature.cpp
  ----------------------------------------------------
  Curvature calculations for route analysis.

  Uses a flat-earth approximation to convert lat/lon degrees to metres.
  This is valid because consecutive route points are only ~10 m apart.

  curvature_computeRadius: circumscribed-circle (Menger curvature) radius
  from three consecutive route points.

  curvature_computeAy: lateral acceleration v²/r given speed (m/s)
  and radius (m).

  NOTE: route_data.h only stores geo coordinates; local.x/y are not
  populated, so all distance calculations use geo lat/lon converted
  to metres on the fly.
*/

// Flat-earth conversion constants
#define DEG_TO_RAD  (3.14159265358979323846 / 180.0)
#define METRES_PER_DEG_LAT  111320.0   // ~constant globally

static void geoToMetres(const GeoCoordinate* from,
                        const GeoCoordinate* to,
                        double* dx, double* dy)
{
    const double midlat = (from->latitude + to->latitude) * 0.5 * DEG_TO_RAD;
    *dx = (to->longitude - from->longitude) * cos(midlat) * METRES_PER_DEG_LAT;
    *dy = (to->latitude  - from->latitude)  * METRES_PER_DEG_LAT;
}

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

    // Convert to metres relative to p0
    double bx, by, cx, cy;
    geoToMetres(&p0->geo, &p1->geo, &bx, &by);
    geoToMetres(&p0->geo, &p2->geo, &cx, &cy);

    // Triangle area via cross product (p0 is origin)
    const double area = fabs(bx * cy - cx * by) * 0.5;
    if (area < 1e-9) return 1e9; // collinear — effectively infinite radius

    const double ab = sqrt(bx*bx + by*by);
    const double bc = sqrt((cx-bx)*(cx-bx) + (cy-by)*(cy-by));
    const double ca = sqrt(cx*cx + cy*cy);

    // Circumradius R = (a * b * c) / (4 * area)
    return (ab * bc * ca) / (4.0 * area);
}

double curvature_computeAy(double v, double r)
{
    if (r < 1e-9) return 0.0;
    return (v * v) / r; // lateral acceleration [m/s²]
}
