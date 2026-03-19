#ifndef GNSS_PARSER_NMEA_H
#define GNSS_PARSER_NMEA_H
/*
  File: gnss_parser_nmea.h
  ----------------------------------------------------
  DEPRECATED — not used by the active codebase.

  The GNSS driver (gnss.cpp) now uses the SparkFun u-blox library
  with UBX-NAV-PVT over I2C. NMEA parsing over Serial1 is no longer
  needed. This file and its companion gnss_parser_nmea.cpp are kept
  for reference only and must not be included in new code.
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