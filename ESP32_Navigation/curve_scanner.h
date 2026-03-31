#ifndef CURVE_SCANNER_H
#define CURVE_SCANNER_H
/*
  File: curve_scanner.h
  ----------------------------------------------------
  Forward curve scanner for route-ahead analysis.

  Scans up to CURVE_SCAN_POINTS route points ahead of the
  current position, groups consecutive tight-radius points into
  CurveSegment records, and returns the nearest one together with
  its distance in metres.
*/

#include "map_types.h"

// ---- Tuning constants ----------------------------------------

// How many route points to scan ahead (~300 * 10 m = 3 km).
#define CURVE_SCAN_POINTS         300

// Circumscribed-circle radius below which a point is considered
// part of a curve (metres).
#define CURVE_RADIUS_THRESHOLD_M  75.0

// Minimum consecutive tight-radius points to form a valid segment
// (filters GPS noise / isolated anomalies).
#define CURVE_MIN_POINTS          3


// ---- Result type ---------------------------------------------

typedef struct
{
    bool         found;       // true if a curve was detected ahead
    CurveSegment segment;     // populated only when found == true
    float        distance_m;  // metres from current_index to segment.entry_index
} CurveScan;


// ---- Public API ----------------------------------------------

/*
  curveScanner_scan
  -----------------
  Scans forward from (current_index + 1) up to CURVE_SCAN_POINTS
  route points and returns the first (nearest) upcoming curve.

  Returns CurveScan.found == false if no curve exists within the
  look-ahead window or the index is invalid.

  Thread safety: reads only from mapStore (const / ROM data) and
  calls curvature_computeRadius which does the same.  Safe to call
  from any task without holding fixMutex.
*/
CurveScan curveScanner_scan(int current_index);

#endif
