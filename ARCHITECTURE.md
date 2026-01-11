# Code Architecture

## Overview

The project is organized into modular components for better maintainability and extensibility. This structure makes it easy to add new features (like web interface, WiFi, etc.) without cluttering the main application logic.

## Project Structure

```
esp32-as5600/
├── include/                    # Header files
│   ├── config.h               # Configuration (pins, sample rate, debug)
│   ├── rgb_led.h              # RGB LED status manager
│   ├── sensor_manager.h       # AS5600 sensor management
│   └── uart_protocol.h        # Binary UART protocol
├── src/                       # Implementation files
│   ├── main.cpp               # Main application (orchestration)
│   ├── rgb_led.cpp            # RGB LED implementation
│   ├── sensor_manager.cpp     # Sensor management implementation
│   └── uart_protocol.cpp      # UART protocol implementation
├── platformio.ini             # PlatformIO configuration
├── README.md                  # Project documentation
└── PROTOCOL.md                # Binary protocol specification
```

## Module Descriptions

### 1. RgbLed (`rgb_led.h/cpp`)

**Purpose**: Manages the onboard WS2812 RGB LED for visual status indication.

**Key Features**:
- Predefined status colors (INIT, OK, PARTIAL, ERROR, WIFI_AP, WIFI_STA)
- Custom color support (R, G, B)
- Brightness control
- Simple API: `setStatus()`, `setColor()`, `off()`

**Usage Example**:
```cpp
RgbLed led(RGB_LED_PIN, 50);
led.begin();
led.setStatus(RgbLed::OK);        // Green
led.setColor(255, 128, 0);        // Custom orange
```

**Future Extensions**:
- Blinking patterns
- Breathing effects
- Multi-LED support

---

### 2. SensorManager (`sensor_manager.h/cpp`)

**Purpose**: Manages dual AS5600 magnetic encoders on separate I2C buses.

**Key Features**:
- Dual I2C bus initialization
- Automatic sensor detection
- Connection status tracking
- Angle reading (0-4095, 12-bit)
- Magnet detection

**Usage Example**:
```cpp
SensorManager sensors;
sensors.begin(SDA0, SCL0, FREQ0, SDA1, SCL1, FREQ1);

if (sensors.areBothConnected()) {
  uint16_t angle1, angle2;
  sensors.readAngles(angle1, angle2);
}
```

**Future Extensions**:
- Velocity calculation (RPM)
- Angle filtering/smoothing
- Calibration support
- Multi-turn tracking
- Support for more sensors via I2C multiplexer

---

### 3. UartProtocol (`uart_protocol.h/cpp`)

**Purpose**: Handles binary serial communication with receiving ESP32.

**Key Features**:
- 7-byte binary packet format
- Start/end markers (0xAA/0x55)
- XOR checksum for data integrity
- Efficient transmission
- Static checksum method for receiver use

**Packet Structure**:
```
[0xAA][Angle1_H][Angle1_L][Angle2_H][Angle2_L][Checksum][0x55]
```

**Usage Example**:
```cpp
UartProtocol uart(Serial1);
uart.begin(115200, TX_PIN, RX_PIN);
uart.transmit(angle1, angle2);
```

**Future Extensions**:
- CRC16 checksum
- Packet acknowledgment
- Bi-directional communication
- Multiple packet types (config, status, data)
- Compression for higher sample rates

---

### 4. Main Application (`main.cpp`)

**Purpose**: Orchestrates all components and implements the main control loop.

**Responsibilities**:
- Initialize all modules
- Coordinate sensor reading
- Manage timing (sample rate)
- Update status LED based on sensor state

**Current Flow**:
```
setup():
  1. Initialize debug serial (if enabled)
  2. Initialize RGB LED → Yellow (INIT)
  3. Initialize UART protocol
  4. Initialize sensor manager
  5. Update LED based on sensor status
  6. Start main loop

loop():
  1. Check if sample interval elapsed
  2. Read angles from both sensors
  3. Transmit via UART
  4. Small delay for watchdog
```

**Future Extensions**:
- WiFi/Bluetooth initialization
- Web server setup
- OTA updates
- Configuration via web interface
- Data logging to SD card

---

## Configuration System (`config.h`)

Centralized configuration makes it easy to adjust settings without modifying code:

### Pin Assignments
- I2C buses (SDA/SCL for both sensors)
- UART (TX/RX)
- RGB LED

### Performance Settings
- Sample rate (10-100 Hz)
- I2C frequency
- UART baud rate

### Debug Settings
- Enable/disable debug output
- Affects compilation (no runtime overhead when disabled)

---

## Adding New Features

### Example: Adding WiFi Web Interface

1. **Create new module**: `web_server.h/cpp`
   ```cpp
   class WebServer {
   public:
     void begin(const char* ssid, const char* password);
     void handleClient();
     void updateSensorData(uint16_t angle1, uint16_t angle2);
   };
   ```

2. **Add to config.h**:
   ```cpp
   #define WIFI_SSID "YourNetwork"
   #define WIFI_PASSWORD "YourPassword"
   #define WEB_SERVER_PORT 80
   ```

3. **Update main.cpp**:
   ```cpp
   #include "web_server.h"
   WebServer webServer;
   
   void setup() {
     // ... existing setup ...
     webServer.begin(WIFI_SSID, WIFI_PASSWORD);
     led.setStatus(RgbLed::WIFI_STA);
   }
   
   void loop() {
     // ... existing loop ...
     webServer.handleClient();
     webServer.updateSensorData(angle1, angle2);
   }
   ```

### Example: Adding Data Logging

1. **Create new module**: `data_logger.h/cpp`
2. **Add SD card pin configuration to config.h**
3. **Instantiate in main.cpp**
4. **Call logger in loop after reading sensors**

---

## Benefits of This Architecture

### ✅ Modularity
- Each component has a single responsibility
- Easy to test individual modules
- Changes in one module don't affect others

### ✅ Maintainability
- Clear separation of concerns
- Easy to locate and fix bugs
- Self-documenting code structure

### ✅ Extensibility
- Add new features without modifying existing code
- Swap implementations (e.g., different LED types)
- Easy to add new communication protocols

### ✅ Reusability
- Modules can be used in other projects
- Well-defined interfaces
- Minimal dependencies between modules

### ✅ Scalability
- Easy to add more sensors
- Support multiple communication channels
- Can grow from simple to complex without refactoring

---

## Compilation

The modular structure compiles efficiently:
- Only modified modules are recompiled
- Header guards prevent multiple inclusions
- Clean dependency graph

**Build command**: `pio run`

**Current binary size**: ~280 KB (plenty of room for expansion)

---

## Future Architecture Considerations

### Potential Additions

1. **Event System**
   - Decouple modules further
   - Publish/subscribe pattern
   - Example: Sensor error → LED blinks red

2. **Configuration Manager**
   - Load/save settings from EEPROM
   - Runtime configuration changes
   - Factory reset capability

3. **Task Scheduler**
   - FreeRTOS tasks for parallel processing
   - Separate tasks for sensors, UART, web server
   - Better CPU utilization

4. **State Machine**
   - Formal state management
   - States: INIT, RUNNING, ERROR, CALIBRATING, etc.
   - Cleaner transitions and error handling

---

**Last Updated**: January 2026
