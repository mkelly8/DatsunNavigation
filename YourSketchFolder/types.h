#ifndef TYPES_H
#define TYPES_H
/*
  File: types.h
  ----------------------------------------------------
  Shared data structures used across modules.

  Responsibilities:
  - GNSS fix structure
  - Diagnostics structure
  - UI state definitions
  - Shared enums and typedefs

  Contains only plain data types.
*/

#include <stdint.h>

/* ----------------------------------------------------
   GNSS Fix Data Structure
---------------------------------------------------- */

struct GnssFix
{
    bool valid = false;          // True if fix is valid
    uint8_t fixQuality = 0;      // 0 = invalid, 1 = GPS fix, etc.
    uint8_t satellites = 0;      // Number of satellites in view
    float hdop = 99.9f;          // Horizontal dilution of precision

    double latitude = 0.0;       // Decimal degrees
    double longitude = 0.0;      // Decimal degrees

    float speed_mps = 0.0f;      // Speed in meters per second
    float course_deg = 0.0f;     // Course over ground

    uint32_t lastUpdateMs = 0;   // Timestamp of last valid update
};


/* ----------------------------------------------------
   Diagnostics Structure
---------------------------------------------------- */

struct Diagnostics
{
    uint32_t uartBytes = 0;        // Total UART bytes received
    uint32_t sentencesParsed = 0;  // Valid NMEA sentences parsed
    uint32_t checksumFailures = 0; // Failed checksum count
    uint32_t uiFrames = 0;         // Number of display frames rendered
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