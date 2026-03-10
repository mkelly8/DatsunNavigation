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

// ---- GNSS UART -----------------------------------------------
#define GNSS_RX_PIN  18
#define GNSS_TX_PIN  17
#define GNSS_BAUD    9600

// ---- Timing (ms) ---------------------------------------------
#define GNSS_TICK_MS    5       // GNSS polling interval
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

// ---- Display (placeholder — fill in when TFT_eSPI is wired) --
// #define TFT_BACKLIGHT_PIN      xxx
// #define TFT_BACKLIGHT_FREQ_HZ  5000
// #define TFT_BACKLIGHT_RES_BITS 8

#endif
