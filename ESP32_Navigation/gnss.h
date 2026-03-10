#ifndef GNSS_H
#define GNSS_H
/*
  File: gnss.h
  ----------------------------------------------------
  GNSS driver interface — SparkFun u-blox GNSS v3 over I2C (Qwiic).

  Responsibilities:
  - Initialize Wire and the u-blox module
  - Poll UBX-NAV-PVT packets and populate GnssFix
  - Expose device-found status for health reporting

  Protocol: UBX binary via I2C (not raw NMEA).
  The SparkFun library handles all UBX framing internally.

  Does not render UI.
*/

#include <stdint.h>
#include <SparkFun_u-blox_GNSS_v3.h>
#include "types.h"

class Gnss
{
public:
    Gnss();

    // Initialises Wire and the u-blox module.
    // Sets deviceFound = true if the module ACKs on the bus.
    void begin();

    // Requests a fresh UBX-NAV-PVT packet and updates the internal GnssFix.
    // Blocks until the module delivers a packet (~1 s at 1 Hz default rate).
    // Call from the dedicated GNSS FreeRTOS task — blocking is fine there.
    void tick(uint32_t nowMs);

    const GnssFix& getFix() const;

    // True after begin() if the module was detected on the I2C bus.
    bool isDeviceFound() const;

private:
    SFE_UBLOX_GNSS gnssModule;
    GnssFix        fix;
    bool           deviceFound;
};

#endif
