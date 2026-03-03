# ESP32-S3 GNSS Navigation System

Embedded navigation system built on the **ESP32-S3 DevKitC-1 (N8R8)** that:

- Reads GNSS data over UART  
- Parses NMEA sentences (GGA/RMC)  
- Renders real-time position data on a TFT display via TFT_eSPI  
- Uses a modular firmware architecture for scalability  
- Designed for enclosure integration and system-level validation  

---

## Project Overview

This project implements a structured, non-blocking embedded system capable of:

- Reliable GNSS acquisition
- Real-time display rendering
- Deterministic scheduling
- System diagnostics and health monitoring
- Future enclosure deployment

The architecture separates hardware interfaces, parsing logic, UI, and system orchestration to maintain scalability and maintainability.

---

## Hardware

### Microcontroller
- ESP32-S3 DevKitC-1 (N8R8)
  - 8MB Flash
  - 8MB PSRAM

### GNSS Module
- UART-based GNSS module outputting NMEA sentences

### Display
- SPI TFT display driven using TFT_eSPI

---

## Development Environment

- Arduino IDE 2.x
- Board: **ESP32S3 Dev Module**
- USB CDC On Boot: Enabled
- PSRAM: Enabled
- Upload Mode: USB CDC

### Required Libraries

- TFT_eSPI

---

## Repository Structure
/YourSketchFolder
  YourSketchFolder.ino

  config.h
  types.h
  time_utils.h/.cpp

  gnss.h/.cpp
  gnss_parser_nmea.h/.cpp

  display.h/.cpp
  screens.h/.cpp
  ui.h/.cpp

  app.h/.cpp
  logger.h/.cpp
  diagnostics.h/.cpp
  
  
### Module Responsibilities

| Module | Responsibility |
|---------|---------------|
| `.ino` | Minimal entrypoint (setup + loop only) |
| `app` | System scheduler and orchestrator |
| `gnss` | UART interface and GNSS state management |
| `gnss_parser_nmea` | NMEA sentence parsing |
| `display` | TFT driver wrapper |
| `screens` | Rendering layouts |
| `ui` | UI state machine |
| `diagnostics` | Health counters and error tracking |
| `logger` | Structured debug logging |

---

## Firmware Design Principles

- Non-blocking scheduling using `millis()`
- Clear separation of concerns
- No heavy logic inside interrupt context
- GNSS parsing independent of display logic
- Minimal logic inside `.ino`
- Deterministic update intervals

---

## GNSS Handling

- Reads raw NMEA sentences via UART
- Validates checksum
- Parses:
  - GGA (fix quality, satellites, HDOP)
  - RMC (latitude, longitude, speed, course)
- Maintains a single authoritative `GnssFix` structure
- Detects stale fixes via timestamp comparison

---

## Display Rendering

- TFT initialized via TFT_eSPI
- Frame-rate limited updates
- Screen rendering separated from logic
- UI state machine controls active screen

---

## Diagnostics

Tracks:

- UART byte count
- Parsed sentence count
- Checksum failures
- Frame count
- Fix validity
- Last GNSS update timestamp

Designed to support soak testing and integration validation.

---

## Functional Validation Plan

### Electrical Validation
- UART stress test
- SPI stability validation
- PWM verification (if backlight controlled)
- Power consumption measurement

### Firmware Validation
- 30–60 minute runtime soak test
- GNSS disconnect recovery
- Stale fix detection
- Memory stability monitoring

### System Integration
- Simultaneous GNSS + display operation
- Frame timing verification
- Long-duration runtime test

---

## Enclosure Integration (Planned)

- Secure PCB mounting
- Cable strain relief
- Thermal considerations
- Serviceability access
- Environmental durability review

---

## Build Instructions

1. Clone repository
2. Open project folder in Arduino IDE
3. Select board: **ESP32S3 Dev Module**
4. Configure board options
5. Upload firmware
6. Open Serial Monitor at 115200 baud

---

## Development Roadmap

Phase 1  
- GNSS UART pipeline validated  
- NMEA parsing confirmed  

Phase 2  
- Display integration  
- Live GNSS rendering  

Phase 3  
- UI state machine  
- Diagnostics integration  

Phase 4  
- Enclosure CAD Rev A  
- Thermal and power validation  

Phase 5  
- Functional validation matrix  
- Long-duration system testing  
- Final documentation  

---

## Known Risks

- UART blocking display updates  
- Memory fragmentation from dynamic allocation  
- SPI timing conflicts  
- Thermal buildup in enclosure  
- Power rail instability under load  

Mitigation strategies are incorporated into sprint planning and validation testing.

---

## Future Enhancements

- UBX binary protocol support  
- Map rendering  
- Waypoint storage  
- SD card logging  
- WiFi telemetry  
- OTA updates  

---

## License

TBD