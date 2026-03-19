#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H
/*
  File: diagnostics.h
  ----------------------------------------------------
  System diagnostics interface.

  Responsibilities:
  - Track runtime metrics
  - Track error counts
  - Expose health status
*/

#include "types.h"

// Reset all counters to zero.
void diagnostics_reset(Diagnostics* diag);

// Print a structured [DIAG] line over Serial.
void diagnostics_print(const Diagnostics* diag);

#endif
