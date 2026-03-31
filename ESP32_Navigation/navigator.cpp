#include "navigator.h"
#include "nav_state.h"
#include "guidance.h"
#include "curve_scanner.h"
#include "config.h"

/*
  File: navigator.cpp
  ----------------------------------------------------
  Navigation pipeline implementation.

  Called once per GNSS fix from the GNSS task.  All operations read
  from ROM route data and require no mutex.
*/

Navigator::Navigator()
    : navState_{}, curveScan_{}
{
}

void Navigator::update(const GnssFix& fix)
{
    if (!fix.valid) return;

    navState_update(&navState_,
                    fix.latitude,
                    fix.longitude,
                    (double)fix.speed_mps);

    guidance_compute(navState_.current_index);

    curveScan_ = curveScanner_scan(navState_.current_index);

    if (curveScan_.found && fix.speed_mps > NAV_MIN_SPEED_MPS_FOR_ETA)
    {
        curveScan_.time_to_curve_s = curveScan_.distance_m / fix.speed_mps;
    }
    else
    {
        curveScan_.time_to_curve_s = 0.0f;
    }
}

const NavState& Navigator::getNavState() const
{
    return navState_;
}

const CurveScan& Navigator::getCurveScan() const
{
    return curveScan_;
}
