#include "time_utils.h"
/*
  File: time_utils.cpp
  ----------------------------------------------------
  Implementation of time utility functions.
*/

bool isStale(uint32_t nowMs, uint32_t lastUpdateMs, uint32_t staleThresholdMs)
{
    // Handles millis() rollover naturally with unsigned arithmetic.
    return (uint32_t)(nowMs - lastUpdateMs) > staleThresholdMs;
}