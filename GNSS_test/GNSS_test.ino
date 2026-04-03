#include <Wire.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

SFE_UBLOX_GNSS gnss;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // I2C default on ESP32-S3: SDA=8, SCL=9
  Wire.begin(8, 9); // blue, yellow

  Serial.println("[STATUS] Initializing SAM-M8Q...");

  if (!gnss.begin(Wire)) {
    Serial.println("[ERROR] SAM-M8Q not detected on I2C. Check wiring.");
    while (1) { delay(1000); }
  }

  // Output UBX protocol only over I2C
  gnss.setI2COutput(COM_TYPE_UBX);

  // Save port settings to flash + BBR
  gnss.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);

  Serial.println("[STATUS] SAM-M8Q online");
  Serial.println("[READY]");
}

void loop() {
  if (!gnss.getPVT()) return;

  uint8_t fixType = gnss.getFixType();
  bool fixOk      = gnss.getGnssFixOk();
  uint8_t siv     = gnss.getSIV();

  // Always print so we can track satellite acquisition
  Serial.print("[GNSS] fix="); Serial.print(fixType);
  Serial.print(" ok=");        Serial.print(fixOk ? "Y" : "N");
  Serial.print(" siv=");       Serial.print(siv);

  if (fixOk) {
    // lat/lon: degrees * 1e-7
    int32_t lat   = gnss.getLatitude();
    int32_t lon   = gnss.getLongitude();
    // speed: mm/s
    int32_t speed = gnss.getGroundSpeed();
    // heading: degrees * 1e-5
    int32_t hdg   = gnss.getHeading();
    // HDOP: scaled by 10
    uint16_t hdop = gnss.getHorizontalDOP();

    Serial.print(" lat=");       Serial.print(lat / 10000000.0, 7);
    Serial.print(" lon=");       Serial.print(lon / 10000000.0, 7);
    Serial.print(" speed_mps="); Serial.print(speed / 1000.0, 3);
    Serial.print(" hdg_deg=");   Serial.print(hdg / 100000.0, 2);
    Serial.print(" hdop=");      Serial.print(hdop / 10.0, 1);
  }

  Serial.println();
}
