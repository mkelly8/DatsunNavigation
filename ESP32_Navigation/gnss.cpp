#include "gnss.h"
/*
  File: gnss.cpp
  ----------------------------------------------------
  GNSS driver implementation.

  Current state:
  - Uses Serial1 with fixed pins/baud (TODO: move to config.h)
  - Assembles NMEA lines into a fixed buffer
  - Validates checksum + sentence type via nmeaParseLine()
  - Updates GnssFix when parser returns true (parser currently stubbed)

  NOTE:
  - Once you implement actual GGA/RMC parsing in nmeaParseLine(),
    fix.valid / lat / lon / sats will start updating.
*/

#include <Arduino.h>
#include "config.h"
#include "gnss_parser_nmea.h"

Gnss::Gnss()
    : lineIndex(0)
{
    // Ensure buffer starts empty
    lineBuf[0] = '\0';
}

void Gnss::begin()
{
    // Initialize GNSS UART.
    // Serial1.begin(baud, config, rxPin, txPin)
    Serial1.begin(GNSS_BAUD, SERIAL_8N1, GNSS_RX_PIN, GNSS_TX_PIN);

    // Reset line assembly state
    lineIndex = 0;
    lineBuf[0] = '\0';

    // Reset fix (defaults from types.h)
    fix = GnssFix{};
}

void Gnss::tick(uint32_t nowMs)
{
    while (Serial1.available() > 0)
    {
        const char c = (char)Serial1.read();
        processByte(c, nowMs);
    }
}

const GnssFix& Gnss::getFix() const
{
    return fix;
}

void Gnss::processByte(char c, uint32_t nowMs)
{
    // NMEA lines end with \r\n typically.
    // We ignore '\r' and treat '\n' as end-of-line.
    if (c == '\r') return;

    if (c == '\n')
    {
        // Terminate and process the line if we have something.
        if (lineIndex > 0)
        {
            lineBuf[lineIndex] = '\0';
            processLine(nowMs);
        }

        // Reset for next line
        lineIndex = 0;
        lineBuf[0] = '\0';
        return;
    }

    // Regular character: append if space remains
    if (lineIndex < (LINE_BUF_SIZE - 1))
    {
        lineBuf[lineIndex++] = c;
    }
    else
    {
        // Overflow protection: drop the line
        lineIndex = 0;
        lineBuf[0] = '\0';
    }
}

void Gnss::processLine(uint32_t nowMs)
{
    // Parser validates checksum and sentence type; returns true if fix updated.
    // Currently nmeaParseLine() is stubbed and returns false, so fix won't change yet.
    if (nmeaParseLine(lineBuf, fix))
    {
        fix.lastUpdateMs = nowMs;
    }
}