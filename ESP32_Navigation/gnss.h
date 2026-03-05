#ifndef GNSS_H
#define GNSS_H
/*
  File: gnss.h
  ----------------------------------------------------
  GNSS driver interface.

  Responsibilities:
  - Initialize GNSS UART
  - Read UART byte stream non-blocking
  - Assemble NMEA lines into a fixed buffer
  - Invoke NMEA parser to update latest GnssFix

  Does not render UI.
*/

#include <stdint.h>
#include "types.h"

class Gnss
{
public:
    Gnss();

    // Keeps signature compatible with current App (no args).
    void begin();

    // Call frequently from App tick.
    void tick(uint32_t nowMs);

    const GnssFix& getFix() const;

private:
    GnssFix fix;

    // NMEA line assembly (fixed-size to avoid heap fragmentation)
    static const uint16_t LINE_BUF_SIZE = 128;
    char lineBuf[LINE_BUF_SIZE];
    uint16_t lineIndex;

    void processByte(char c, uint32_t nowMs);
    void processLine(uint32_t nowMs);
};

#endif