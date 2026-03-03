#ifndef TIME_UTILS_H
#define TIME_UTILS_H
/*
  File: time_utils.h
  ----------------------------------------------------
  Time-related utility functions.

  Responsibilities:
  - Stale fix checks
  - Simple time formatting helpers (optional)
*/

#include <stdint.h>

// Returns true if (nowMs - lastUpdateMs) exceeds staleThresholdMs.
bool isStale(uint32_t nowMs, uint32_t lastUpdateMs, uint32_t staleThresholdMs);

#endif