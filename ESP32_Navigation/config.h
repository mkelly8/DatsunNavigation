#ifndef CONFIG_H
#define CONFIG_H
/*
  File: config.h
  ----------------------------------------------------
  Central configuration header.

  Responsibilities:
  - GPIO pin assignments
  - Baud rates
  - Timing constants
  - FreeRTOS task configuration
*/

// ---- GNSS I2C (Qwiic) ----------------------------------------
// u-blox modules default to address 0x42.
// SDA=8 / SCL=9 are the ESP32-S3 Arduino defaults.
#define GNSS_I2C_ADDR  0x42
#define GNSS_SDA_PIN   8
#define GNSS_SCL_PIN   9
#define GNSS_I2C_FREQ  400000   // 400 kHz fast-mode

// ---- Timing (ms) ---------------------------------------------
// Note: no GNSS_TICK_MS — gnss.tick() calls getPVT() which blocks
// internally until the module delivers a packet (~1 s at 1 Hz).
#define UI_TICK_MS      50      // Display frame period (~20 FPS)
#define HEALTH_TICK_MS  2000    // Heartbeat interval

// ---- FreeRTOS task priorities (higher number = more urgent) --
#define TASK_PRIORITY_GNSS    5
#define TASK_PRIORITY_UI      3
#define TASK_PRIORITY_HEALTH  1

// ---- FreeRTOS task stack sizes (bytes) -----------------------
#define TASK_STACK_GNSS    2048
#define TASK_STACK_UI      4096   // Extra headroom for future TFT rendering
#define TASK_STACK_HEALTH  1024

// ---- Core assignments (0 = protocol core, 1 = app core) ------
#define CORE_GNSS    1
#define CORE_UI      1
#define CORE_HEALTH  1

// ---- Display SPI (ILI9341 via TFT_eSPI) ----------------------
// These must match User_Setup.h in the TFT_eSPI library folder.
// Avoids: GPIO 0 (BOOT), 8/9 (Qwiic I2C), 19/20 (USB), 48 (RGB LED)
#define TFT_MISO_PIN  13
#define TFT_MOSI_PIN  11
#define TFT_SCLK_PIN  12
#define TFT_CS_PIN    10
#define TFT_DC_PIN     7
// TFT_RST tied to 3.3V — no GPIO needed

// Backlight PWM (LEDC)
#define TFT_BL_PIN          6
#define TFT_BL_FREQ_HZ   5000
#define TFT_BL_RES_BITS     8   // 0–255 duty cycle

#endif
