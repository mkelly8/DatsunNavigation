#include "curve_scanner.h"
#include "curvature.h"
#include "map_store.h"
#include "geo_math.h"
#include <math.h>
#include <string.h>
/*
  File: curve_scanner.cpp
  ----------------------------------------------------
  Forward curve scanner implementation.

  Algorithm:
    1. Clamp scan window to [current+1, min(current+CURVE_SCAN_POINTS, count-2)].
       Upper bound is count-2 because curvature_computeRadius(i) needs i+1 to exist.
    2. Walk each index i in the window:
         radius = curvature_computeRadius(i)
         if radius < CURVE_RADIUS_THRESHOLD_M  -> extend run, track minimum radius
         else if run just ended                -> check if run length >= CURVE_MIN_POINTS
    3. When a valid run is found:
         entry_index    = first index in the run
         exit_index     = last index in the run
         midpoint_index = (entry + exit) / 2
         direction      = cross-product sign of entry->mid and mid->exit vectors
         min_radius_m   = smallest radius seen in the run
         distance_m     = (entry_index - current_index) * ROUTE_POINT_SPACING_M
    4. Return immediately with the first (nearest) valid segment.

  Noise rejection: runs shorter than CURVE_MIN_POINTS are discarded.

  Direction detection uses the same flat-earth geo->metres conversion
  as curvature.cpp (valid for ~10 m inter-point spacing).
*/

// Use shared constants from geo_math.h — no local copies needed.

// ---- Private helpers -----------------------------------------

// Flat-earth direction vector from geo point 'from' to 'to' in metres.
static void cs_geoToMetres(const GeoCoordinate* from,
                            const GeoCoordinate* to,
                            double* dx, double* dy)
{
    const double midlat = (from->latitude + to->latitude) * 0.5 * GEO_DEG_TO_RAD;
    *dx = (to->longitude - from->longitude) * cos(midlat) * GEO_METRES_PER_DEG_LAT;
    *dy = (to->latitude  - from->latitude)  * GEO_METRES_PER_DEG_LAT;
}

/*
  Determine turn direction using 2D cross product.

    A = entry, B = midpoint, C = exit
    vec1 = A->B,  vec2 = A->C  (both relative to A as origin)
    cross = vec1.x * (vec2.y - vec1.y) - vec1.y * (vec2.x - vec1.x)

    cross > 0  => TURN_LEFT  (counter-clockwise)
    cross < 0  => TURN_RIGHT (clockwise)
*/
static TurnDirection cs_computeDirection(uint32_t entry_idx,
                                         uint32_t mid_idx,
                                         uint32_t exit_idx)
{
    const RoutePoint* pA = mapStore_getPointPtr(entry_idx);
    const RoutePoint* pB = mapStore_getPointPtr(mid_idx);
    const RoutePoint* pC = mapStore_getPointPtr(exit_idx);

    if (!pA || !pB || !pC) return TURN_NONE;

    double bx, by, cx, cy;
    cs_geoToMetres(&pA->geo, &pB->geo, &bx, &by); // A -> B
    cs_geoToMetres(&pA->geo, &pC->geo, &cx, &cy); // A -> C

    // Cross product: vec1=(bx,by), vec2-vec1=((cx-bx),(cy-by))
    const double cross = bx * (cy - by) - by * (cx - bx);

    if (cross > 0.0) return TURN_LEFT;
    if (cross < 0.0) return TURN_RIGHT;
    return TURN_NONE;
}

// ---- Public function ------------------------------------------

CurveScan curveScanner_scan(int current_index)
{
    CurveScan result;
    memset(&result, 0, sizeof(result));
    result.found = false;

    const uint32_t count = mapStore_getPointCount();
    if (count < 3 || current_index < 0) return result;

    // Scan window
    const int scan_start  = current_index + 1;
    const int scan_end    = current_index + CURVE_SCAN_POINTS;
    const int clamped_end = (scan_end < (int)count - 2)
                          ? scan_end
                          : (int)count - 2;

    if (scan_start > clamped_end) return result;

    // Run-tracking state
    bool   in_run    = false;
    int    run_start = 0;
    double run_min_r = GEO_STRAIGHT_RADIUS_M;

    for (int i = scan_start; i <= clamped_end; i++)
    {
        const double radius = curvature_computeRadius(i);

        if (radius < CURVE_RADIUS_THRESHOLD_M)
        {
            if (!in_run)
            {
                in_run    = true;
                run_start = i;
                run_min_r = radius;
            }
            else if (radius < run_min_r)
            {
                run_min_r = radius;
            }
        }
        else
        {
            if (in_run)
            {
                const int run_length = (i - 1) - run_start + 1;

                if (run_length >= CURVE_MIN_POINTS)
                {
                    const uint32_t entry_idx = (uint32_t)run_start;
                    const uint32_t exit_idx  = (uint32_t)(i - 1);
                    const uint32_t mid_idx   = (entry_idx + exit_idx) / 2;

                    result.found                  = true;
                    result.segment.entry_index    = entry_idx;
                    result.segment.midpoint_index = mid_idx;
                    result.segment.exit_index     = exit_idx;
                    result.segment.min_radius_m   = run_min_r;
                    result.segment.direction      = cs_computeDirection(entry_idx, mid_idx, exit_idx);
                    result.distance_m             = (float)((run_start - current_index)
                                                   * ROUTE_POINT_SPACING_M);
                    return result; // return the nearest curve immediately
                }

                // Run too short — noise, discard and keep scanning
                in_run    = false;
                run_min_r = GEO_STRAIGHT_RADIUS_M;
            }
        }
    }

    // Handle a run that extends to the edge of the scan window
    if (in_run)
    {
        const int run_length = clamped_end - run_start + 1;

        if (run_length >= CURVE_MIN_POINTS)
        {
            const uint32_t entry_idx = (uint32_t)run_start;
            const uint32_t exit_idx  = (uint32_t)clamped_end;
            const uint32_t mid_idx   = (entry_idx + exit_idx) / 2;

            result.found                  = true;
            result.segment.entry_index    = entry_idx;
            result.segment.midpoint_index = mid_idx;
            result.segment.exit_index     = exit_idx;
            result.segment.min_radius_m   = run_min_r;
            result.segment.direction      = cs_computeDirection(entry_idx, mid_idx, exit_idx);
            result.distance_m             = (float)((run_start - current_index)
                                           * ROUTE_POINT_SPACING_M);
        }
    }

    return result;
}
