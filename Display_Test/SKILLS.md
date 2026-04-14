# Embedded Development: ESP32-S3

You are developing firmware for a ESP32-S3 connected via USB.

## Hardware
- Board: ESP32-S3
- Port: COM4
- Framework: arduino
- Baud rate: 115200

## Setup

Before compiling, ensure your toolchain is configured:

```
arduino-cli core install esp32:esp32 --additional-urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

## Commands

Compile:
```
arduino-cli compile --fqbn esp32:esp32:esp32s3:UploadSpeed=115200 .
```

Flash:
```
arduino-cli upload --fqbn esp32:esp32:esp32s3:UploadSpeed=115200 --port COM4 .
```
Monitor:
```
arduino-cli monitor --port COM4 --config baudrate=115200
```

## Development Loop

Every time you change code, follow this exact sequence:

1. Edit your firmware source files
2. Compile: `arduino-cli compile --fqbn esp32:esp32:esp32s3:UploadSpeed=115200 .`
3. If compile fails, read the errors, fix them, and recompile. Do NOT flash broken code.
4. Flash: `arduino-cli upload --fqbn esp32:esp32:esp32s3:UploadSpeed=115200 --port COM4 .`
5. Wait 3 seconds for the board to reboot.
6. **Validate your changes** using the debugging methods below. Pick the right tool for what you're checking.
7. If validation fails, go back to step 1 and iterate.

## Debugging

Use the right tool for the problem:
- **Serial** — bidirectional communication: read application output and send commands to trigger firmware behavior

### Serial Communication

This is how you verify your code is actually working on the device. Always validate after flashing.

Use this Python snippet to capture serial output from the board:

```python
import serial, time, sys

try:
    ser = serial.Serial('COM4', 115200, timeout=1)
except serial.SerialException as err:
    print("Could not open COM4: " + str(err))
    print("Check: is another process using the port? (serial monitor, screen, another script)")
    sys.exit(1)

time.sleep(3)  # Wait for boot
lines = []
start = time.time()
while time.time() - start < 10:  # Read for up to 10 seconds
    line = ser.readline().decode('utf-8', errors='ignore').strip()
    if line:
        lines.append(line)
        print(line)
        if line == '[DONE]':
            break
ser.close()
```

Save this as `read_serial.py` and run with `python read_serial.py`. Parse the output to check if your firmware is behaving correctly. Adapt the timeout and read duration as needed — some operations (WiFi connect, sensor warm-up) take longer than 10 seconds.

#### Sending Commands

Use this Python snippet to send commands to the board and read the response:

```python
import serial, time, sys

try:
    ser = serial.Serial('COM4', 115200, timeout=1)
except serial.SerialException as err:
    print("Could not open COM4: " + str(err))
    sys.exit(1)

time.sleep(3)  # Wait for boot

# Wait for [READY] before sending
start = time.time()
while time.time() - start < 10:
    line = ser.readline().decode('utf-8', errors='ignore').strip()
    if line == '[READY]':
        break
else:
    print("Timed out waiting for [READY]")
    ser.close()
    sys.exit(1)

# Send a command (newline-terminated)
command = "read_sensor"
ser.write((command + "\n").encode())
ser.flush()

# Read response until [DONE] or timeout
lines = []
start = time.time()
while time.time() - start < 10:
    line = ser.readline().decode('utf-8', errors='ignore').strip()
    if line:
        lines.append(line)
        print(line)
        if line == '[DONE]':
            break
ser.close()
```

Save this as `send_command.py` and run with `python send_command.py`. Adapt the command, timeout, and read duration as needed.

**Important serial conventions for your firmware:**
- Configure your serial port at 115200 baud
- Send complete lines (newline-terminated) so each message can be parsed
- Print `[READY]` when initialization is complete
- Print `[ERROR] <description>` for any error conditions
- Use tags for structured output: `[SENSOR] temp=23.4`, `[STATUS] running`
- Print `[DONE]` when a test sequence finishes (allows the reader to exit early)
- Echo received commands with `[CMD] <command>` so the agent can confirm what was received
- Print `[OK]` after successful command execution
- Command flow example: send `read_sensor\n` → firmware prints `[CMD] read_sensor`, `[SENSOR] temp=23.4`, `[OK]`, `[DONE]`

### edesto CLI Tools

Use these commands instead of writing Python serial scripts:

| Command | Description |
|---|---|
| `edesto serial read` | Read serial output (auto-parses tags, logs to `.edesto/`) |
| `edesto serial send <CMD>` | Send a command and read response (auto-detects markers) |
| `edesto serial monitor` | Stream serial output continuously |
| `edesto serial ports` | List available serial ports with board labels |
| `edesto debug scan` | Scan source files for logging APIs, markers, ISR zones |
| `edesto debug instrument <FILE:LINE> --expr <EXPR> [--fmt <FMT>]` | Insert a debug print at a specific line |
| `edesto debug instrument --function <NAME>` | Add entry/exit logging to a function |
| `edesto debug instrument --gpio <FILE:LINE>` | Insert GPIO toggle for timing measurement |
| `edesto debug clean [--dry-run]` | Remove all instrumentation (EDESTO_TEMP_DEBUG markers) |
| `edesto debug status [--json]` | Show diagnostic snapshot (log analysis, tools, device state) |
| `edesto debug reset` | Clear all debug state files |
| `edesto config <KEY> <VALUE>` | Set a config value in edesto.toml |
| `edesto config <KEY>` | Get a config value |
| `edesto config --list` | Show all config values |

**Serial command options:** `--port`, `--baud` (override edesto.toml), `--json` (structured output), `--duration`, `--until` (stop on marker).

**Instrumentation safety rules:**
- All inserted lines are marked with `// EDESTO_TEMP_DEBUG` for guaranteed cleanup
- Instrumentation is refused in ISR/danger zones (use `--gpio` for timing in ISRs, or `--force` to override)
- `edesto debug clean` removes ALL instrumented lines — always run before committing
- Configure debug GPIO pin: `edesto config debug.gpio <PIN>`

**Debug workflow:**
1. `edesto debug scan` — analyze the project (auto-runs on first `serial read`)
2. `edesto serial read` / `edesto serial send <CMD>` — observe behavior
3. `edesto debug instrument ...` — add targeted debug output if needed
4. Compile and flash, then `edesto serial read` to check
5. `edesto debug clean` — remove instrumentation when done
6. `edesto debug status` — get a structured diagnostic snapshot for complex issues

## Troubleshooting

**Upload fails / "connection timeout":**
- Close any serial monitors or scripts that have the port open. Only one process can use `COM4` at a time.
- Try unplugging and re-plugging the USB cable.
- Some boards require holding the BOOT button during upload — check the pitfalls section below.

**No serial output after flashing:**
- Verify the baud rate in your firmware matches 115200.
- Ensure your firmware actually prints to serial (e.g., `Serial.begin(115200)` or equivalent).
- Wait at least 3 seconds after flashing — the board needs time to reboot.

**Garbage characters on serial:**
- Almost always a baud rate mismatch. Ensure both firmware and the read script use 115200.

**"Permission denied" on serial port:**
- Linux: add your user to the `dialout` group: `sudo usermod -aG dialout $USER` (logout/login required).
- macOS: install the USB-serial driver for your board's chip (CP2102, CH340, etc.).

## Datasheets

Before writing or debugging firmware, check for datasheets in this project.

### Finding Datasheets

1. **Check `datasheets/` folder first** — if it exists, read any relevant PDFs.
2. **Check `docs/` and `hardware/` folders** for reference manuals, errata, and application notes.
3. **Search the project root and all subfolders** for any `.pdf` files that may be component datasheets or reference manuals.

### Extracting Key Information

When implementing a peripheral driver or configuring hardware, extract and use:

- **Register maps and bit-field definitions** — get the exact register addresses, field positions, and reset values. Never guess register addresses or bit positions.
- **Pin configurations and alternate functions** — verify which GPIO pins support the peripheral you need and what alternate-function mapping is required.
- **Timing diagrams and protocol specifications** — check setup/hold times, clock polarity/phase, and protocol framing.
- **Electrical characteristics** — voltage levels, maximum current per pin, absolute maximum ratings, and recommended operating conditions.
- **Initialization sequences** — many peripherals require a specific power-up or configuration sequence. Follow the datasheet order exactly.
- **Clock tree and prescaler settings** — verify the peripheral clock source and required divider values for your target frequency.

### Cross-Referencing Documents

If the project contains multiple documents (e.g., a reference manual AND a datasheet AND an errata sheet), use all of them:

- **Reference manual** — detailed register descriptions and peripheral operation.
- **Datasheet** — pinout, electrical characteristics, package information, and ordering codes.
- **Errata / silicon bugs** — known hardware issues that may require software workarounds. Always check for errata before finalizing a driver.
- **Application notes** — recommended configurations and design patterns from the manufacturer.

### Citing Datasheet Sections

When writing code based on datasheet information, add comments referencing the source:

```c
// See RM0090 Section 8.3.3 — SPI CR1 register
SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_1;  // Master mode, fPCLK/8
```

This makes it easy to verify the code against the documentation later.

### ESP32 Technical Reference Structure

ESP32 documentation is organized as:

- **Technical Reference Manual (TRM)** — register-level details for each peripheral. Chapters are organized by peripheral (GPIO, SPI, I2C, etc.) with register description tables.
- **Datasheet** — pin definitions, electrical characteristics, RF specifications, and module pinouts.
- **Errata** — known issues per chip revision.
- **ESP-IDF Programming Guide** — higher-level API documentation and configuration options (complements the TRM).

Key sections to look for: "IO MUX and GPIO Matrix", "Register Summary" tables at the end of each peripheral chapter.

## RTOS

This project runs on **FreeRTOS**. All application code executes inside FreeRTOS tasks.

### Core Concepts

- **Tasks** are the basic unit of execution — each task has its own stack, priority, and state (running, ready, blocked, suspended).
- The **scheduler** is preemptive by default: a higher-priority task that becomes ready will immediately preempt a lower-priority one.
- **Tick rate** (`configTICK_RATE_HZ`, typically 1000) determines the time resolution for delays and timeouts.
- Priority numbers: higher number = higher priority. `tskIDLE_PRIORITY` (0) is the lowest.

### Task Creation

```c
// Basic task creation
xTaskCreate(
    task_function,    // Function pointer
    "TaskName",       // Debug name
    2048,             // Stack size (bytes on ESP32, words on other ports)
    NULL,             // Parameter passed to task
    5,                // Priority
    &task_handle      // Handle (can be NULL if not needed)
);

// ESP32-specific: pin task to a core (0 or 1)
xTaskCreatePinnedToCore(
    task_function, "TaskName", 2048, NULL, 5, &task_handle,
    1  // Core ID: 0 = protocol core, 1 = application core
);
```

### Synchronization Primitives

**Semaphores** — signaling between tasks or from ISRs:
```c
SemaphoreHandle_t sem = xSemaphoreCreateBinary();
xSemaphoreGive(sem);                              // Signal
xSemaphoreTake(sem, pdMS_TO_TICKS(1000));         // Wait (up to 1s)
```

**Mutexes** — protect shared resources (supports priority inheritance):
```c
SemaphoreHandle_t mtx = xSemaphoreCreateMutex();
xSemaphoreTake(mtx, portMAX_DELAY);  // Lock
// ... critical section ...
xSemaphoreGive(mtx);                 // Unlock
```

**Queues** — pass data between tasks:
```c
QueueHandle_t q = xQueueCreate(10, sizeof(int));
int val = 42;
xQueueSend(q, &val, portMAX_DELAY);               // Send
xQueueReceive(q, &val, pdMS_TO_TICKS(500));        // Receive
```

### Timers

```c
TimerHandle_t timer = xTimerCreate(
    "MyTimer", pdMS_TO_TICKS(1000), pdTRUE,  // 1s, auto-reload
    NULL, timer_callback
);
xTimerStart(timer, 0);
```

### ISR Rules

**Critical:** ISR functions must use `FromISR` variants of all FreeRTOS API calls:
```c
void IRAM_ATTR my_isr(void *arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
```
- Never call `xSemaphoreTake()`, `xQueueSend()`, or any blocking function in an ISR — use `xSemaphoreGiveFromISR()`, `xQueueSendFromISR()`, etc.
- Always check `xHigherPriorityTaskWoken` and call `portYIELD_FROM_ISR()` to trigger a context switch if needed.

### Common Pitfalls

- **Stack overflow** — enable `configCHECK_FOR_STACK_OVERFLOW` (set to 2) during development. A stack overflow silently corrupts memory and causes random crashes.
- **Priority inversion** — use mutexes (not binary semaphores) for resource protection; mutexes support priority inheritance.
- **Watchdog starvation** — on ESP32, the idle task feeds the watchdog. If a high-priority task never blocks, the watchdog triggers. Always include a `vTaskDelay()` or blocking call in long-running loops.
- **`vTaskDelay()` vs `vTaskDelayUntil()`** — `vTaskDelay()` delays *from now*; `vTaskDelayUntil()` delays *from the last wake time*, giving more precise periodic timing.
- **Forgetting `portMAX_DELAY`** — passing `0` as timeout means "don't wait" and returns immediately if the resource isn't available.

## ESP32-S3-Specific Information

### Capabilities
- Wifi: `#include <WiFi.h>`
- Ble
- Http Server: `#include <WebServer.h>`
- Ota: `#include <ArduinoOTA.h>`
- Spiffs: `#include <SPIFFS.h>`
- Preferences: `#include <Preferences.h>`
- Usb Native

### Pin Reference
- GPIO 48: RGB LED (WS2812-style, not a simple HIGH/LOW LED)
- GPIO 19/20: USB D-/D+ — do not use for general I/O
- GPIO 0: Boot button — do not use for general I/O
- ADC1: GPIO 1-10 (works alongside WiFi)
- ADC2: GPIO 11-20 (does NOT work when WiFi is active)
- I2C default: SDA=8, SCL=9
- SPI default: MOSI=11, MISO=13, SCK=12, SS=10

### Common Pitfalls
- ADC2 pins do not work when WiFi is active. Use ADC1 pins (1-10) if you need analog reads with WiFi.
- GPIO 19/20 are USB pins. Do not use them for general I/O.
- RGB LED on GPIO 48 requires NeoPixel-style protocol, not simple digitalWrite.
- If upload fails, hold BOOT and press RST, then release BOOT after upload starts.
- delay() blocks the entire core. Use millis() for non-blocking timing.
- String concatenation in loops causes heap fragmentation. Use char[] buffers for repeated operations.