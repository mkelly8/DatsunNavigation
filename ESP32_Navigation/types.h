#ifndef TYPES_H
#define TYPES_H
/*
  File: types.h
  ----------------------------------------------------
  Shared data structures used across modules.

  Responsibilities:
  - GNSS fix structure (populated from UBX-NAV-PVT via SparkFun library)
  - Diagnostics structure
  - UI state definitions
  - Shared enums and typedefs

  Contains only plain data types.
*/

#include <stdint.h>

/* ----------------------------------------------------
   GNSS Fix Data Structure
   Populated from UBX-NAV-PVT fields — see gnss.cpp for field mapping.
---------------------------------------------------- */

struct GnssFix
{
    bool    valid      = false;   // True if gnssFixOk (module has a valid fix)
    uint8_t fixQuality = 0;       // 0=no fix, 2=2D, 3=3D, 4=GNSS+dead reckoning
    uint8_t satellites = 0;       // Satellites in view (SIV)
    float   hdop       = 99.9f;   // Horizontal dilution of precision

    double latitude    = 0.0;     // Decimal degrees
    double longitude   = 0.0;     // Decimal degrees

    float speed_mps    = 0.0f;    // Ground speed, metres per second
    float course_deg   = 0.0f;    // Heading, degrees true

    uint32_t lastUpdateMs = 0;    // millis() timestamp of last successful PVT read
};


/* ----------------------------------------------------
   Diagnostics Structure
---------------------------------------------------- */

struct Diagnostics
{
    uint32_t pvtPackets = 0;   // Successful UBX-NAV-PVT reads
    uint32_t uiFrames   = 0;   // Display frames rendered
};


/* ----------------------------------------------------
   UI State
---------------------------------------------------- */

enum class ScreenId
{
    Boot,
    Status,
    Navigation
};

#endif
