#include "gnss_parser_nmea.h"
/*
  File: gnss_parser_nmea.cpp
  ----------------------------------------------------
  Implementation of NMEA parsing logic.

  Current state:
  - Checksum validation implemented
  - Sentence type detection implemented
  - Field parsing is stubbed (returns false) for now
*/

#include <ctype.h>
#include <string.h>

// Convert a single hex character to value (0-15), returns 255 on error.
static uint8_t hexNibble(char c)
{
    if (c >= '0' && c <= '9') return (uint8_t)(c - '0');
    c = (char)toupper((unsigned char)c);
    if (c >= 'A' && c <= 'F') return (uint8_t)(10 + (c - 'A'));
    return 255;
}

bool nmeaChecksumValid(const char* line)
{
    if (!line) return false;
    if (line[0] != '$') return false;

    const char* star = strchr(line, '*');
    if (!star)
    {
        // Some streams omit checksum; treat as invalid for safety.
        // If you later decide to accept checksum-less sentences, change this.
        return false;
    }

    // XOR of characters between '$' and '*'
    uint8_t cs = 0;
    for (const char* p = line + 1; p < star; ++p)
    {
        cs ^= (uint8_t)(*p);
    }

    // Expect two hex digits after '*'
    if (star[1] == '\0' || star[2] == '\0') return false;

    const uint8_t hi = hexNibble(star[1]);
    const uint8_t lo = hexNibble(star[2]);
    if (hi == 255 || lo == 255) return false;

    const uint8_t expected = (uint8_t)((hi << 4) | lo);
    return cs == expected;
}

NmeaSentenceType nmeaDetectType(const char* line)
{
    if (!line) return NmeaSentenceType::Unknown;
    if (line[0] != '$') return NmeaSentenceType::Unknown;

    // Typical headers:
    // $GPGGA, $GNGGA, $GLGGA, $GAGGA
    // $GPRMC, $GNRMC, ...
    //
    // We only care about the last 3 chars of the 5-char talker+type:
    // e.g., "GGA" or "RMC"
    //
    // Format: $TTXXX,...
    // where TT = talker (2 chars), XXX = sentence type (3 chars)
    if (strlen(line) < 6) return NmeaSentenceType::Unknown;

    // line[1..5] includes talker+type
    const char t1 = line[3];
    const char t2 = line[4];
    const char t3 = line[5];

    if (t1 == 'G' && t2 == 'G' && t3 == 'A') return NmeaSentenceType::GGA;
    if (t1 == 'R' && t2 == 'M' && t3 == 'C') return NmeaSentenceType::RMC;

    return NmeaSentenceType::Unknown;
}

bool nmeaParseLine(const char* line, GnssFix& /*fix*/)
{
    if (!line) return false;

    // Validate checksum first
    if (!nmeaChecksumValid(line)) return false;

    // Only accept sentences we recognize
    const NmeaSentenceType type = nmeaDetectType(line);
    if (type == NmeaSentenceType::Unknown) return false;

    // Stub: parsing will be implemented next (GGA/RMC field extraction).
    // Return false for now (no fix updates).
    return false;
}