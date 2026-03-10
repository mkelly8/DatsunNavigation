#include "gnss.h"
/*
  File: gnss.cpp
  ----------------------------------------------------
  GNSS driver — SparkFun u-blox GNSS v3, UBX over I2C (Qwiic).

  UBX-NAV-PVT field mapping to GnssFix:
    getGnssFixOk()      → valid          (bool)
    getFixType()        → fixQuality     (0=no fix, 2=2D, 3=3D, 4=GNSS+DR)
    getSIV()            → satellites     (uint8_t)
    getHorizontalDOP()  → hdop           (uint16_t ×0.01 → float)
    getLatitude()       → latitude       (int32_t deg×1e-7 → double)
    getLongitude()      → longitude      (int32_t deg×1e-7 → double)
    getGroundSpeed()    → speed_mps      (int32_t mm/s → float m/s)
    getHeading()        → course_deg     (int32_t deg×1e-5 → float)

  Note: gnss_parser_nmea.h/.cpp are no longer used — UBX replaces NMEA parsing.
*/

#include <Arduino.h>
#include <Wire.h>
#include "config.h"

Gnss::Gnss()
    : fix{},
      deviceFound(false)
{
}

void Gnss::begin()
{
    Wire.begin(GNSS_SDA_PIN, GNSS_SCL_PIN);
    Wire.setClock(GNSS_I2C_FREQ);

    deviceFound = gnssModule.begin(Wire);

    if (!deviceFound)
    {
        Serial.println("[ERROR] GNSS module not found on I2C bus (addr 0x42)");
        return;
    }

    // Output only UBX protocol — disables NMEA output on the I2C port
    gnssModule.setI2COutput(COM_TYPE_UBX);

    // 2 Hz update rate — PVT arrives every 500 ms instead of 1 s
    gnssModule.setNavigationFrequency(2);

    // Auto-push PVT: module sends each solution unprompted.
    // getPVT() then returns immediately when new data is ready,
    // rather than polling and blocking for a full update cycle.
    gnssModule.setAutoPVT(true);

    // Save only the I/O port config to flash — not all settings.
    // saveConfiguration() would also bake in nav frequency etc.
    gnssModule.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);

    fix = GnssFix{};
}

void Gnss::tick(uint32_t nowMs)
{
    if (!deviceFound) return;

    // getPVT() blocks until a fresh NAV-PVT packet arrives (~500 ms at 2 Hz).
    // getInvalidLlh() rejects packets where lat/lon are flagged invalid by the module.
    // The GNSS FreeRTOS task has no vTaskDelayUntil — getPVT() is the pacing mechanism.
    if (!gnssModule.getPVT() || gnssModule.getInvalidLlh()) return;

    fix.valid      = gnssModule.getGnssFixOk();
    fix.fixQuality = gnssModule.getFixType();
    fix.satellites = gnssModule.getSIV();
    fix.hdop       = gnssModule.getHorizontalDOP() * 0.01f;

    fix.latitude   = gnssModule.getLatitude()  * 1e-7;
    fix.longitude  = gnssModule.getLongitude() * 1e-7;

    fix.speed_mps  = gnssModule.getGroundSpeed() * 0.001f; // mm/s → m/s
    fix.course_deg = gnssModule.getHeading()     * 1e-5f;  // deg×1e-5 → deg

    fix.lastUpdateMs = nowMs;
}

const GnssFix& Gnss::getFix() const
{
    return fix;
}

bool Gnss::isDeviceFound() const
{
    return deviceFound;
}
