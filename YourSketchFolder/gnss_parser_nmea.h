#ifndef GNSS_PARSER_NMEA_H
#define GNSS_PARSER_NMEA_H
/*
  File: gnss_parser_nmea.h
  ----------------------------------------------------
  NMEA sentence parser interface.

  Responsibilities:
  - Validate NMEA checksum
  - Identify supported sentence types (GGA/RMC)
  - Parse supported fields into GnssFix

  Pure parsing logic (no UART, no display).
*/

#include <stdint.h>
#include "types.h"

enum class NmeaSentenceType : uint8_t
{
    Unknown = 0,
    GGA,
    RMC
};

// ************Warning: this function needs to be made easier if output doesn't start with $
// Returns true if line begins with '$' and checksum (if present) is valid.
bool nmeaChecksumValid(const char* line);

// Detects if sentence is GGA/RMC regardless of talker (GP/GN/GL/GA).
NmeaSentenceType nmeaDetectType(const char* line);

// Parse a single NMEA line into fix; returns true if fix was updated.
bool nmeaParseLine(const char* line, GnssFix& fix);

#endif