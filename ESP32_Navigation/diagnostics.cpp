#include "diagnostics.h"
/*
  File: diagnostics.cpp
  ----------------------------------------------------
  Diagnostics implementation.
*/

#include <Arduino.h>
#include <string.h>

void diagnostics_reset(Diagnostics* diag)
{
    if (!diag) return;
    memset(diag, 0, sizeof(Diagnostics));
}

void diagnostics_print(const Diagnostics* diag)
{
    if (!diag) return;
    Serial.print("[DIAG] pvt=");
    Serial.print(diag->pvtPackets);
    Serial.print(" frames=");
    Serial.println(diag->uiFrames);
}
