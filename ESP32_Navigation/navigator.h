#ifndef NAVIGATOR_H
#define NAVIGATOR_H
/*
  File: navigator.h
  ----------------------------------------------------
  Navigation pipeline encapsulation.

  Navigator owns all CPU-intensive navigation work — map matching,
  guidance bearing computation, and forward curve scanning — so that
  App::gnssTaskBody() is responsible only for GNSS I/O and shared-
  state publishing.

  Thread safety:
    update() reads only ROM route data (mapStore, curvature, scanner)
    and is safe to call without holding fixMutex.  It must be called
    from a single task only (the GNSS task).
    getNavState() / getCurveScan() return const references to private
    members; callers in other tasks must copy them under fixMutex
    before use.
*/

#include "nav_state.h"
#include "curve_scanner.h"
#include "types.h"

class Navigator
{
public:
    Navigator();

    // Run the full pipeline for a new GNSS fix.
    // No-op if fix.valid is false.
    void update(const GnssFix& fix);

    const NavState&  getNavState()  const;
    const CurveScan& getCurveScan() const;

private:
    NavState  navState_;
    CurveScan curveScan_;
};

#endif // NAVIGATOR_H
