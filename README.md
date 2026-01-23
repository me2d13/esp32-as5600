# ESP32-S3 Dual AS5600 Magnetic Encoder Reader

## Project Overview

This project uses an **ESP32-S3-Zero** (Waveshare) module to read angular position data from **two AS5600 magnetic rotary encoders** simultaneously and transmit the data via UART to another ESP32 module for further processing.

### Why Two I2C Buses?

The AS5600 magnetic encoder has a **fixed I2C address (0x36)** that cannot be changed. To read from two AS5600 sensors simultaneously, this project utilizes the ESP32-S3's capability to support **multiple I2C buses** (Wire and Wire1), with each sensor connected to a separate bus.

## Hardware Components

- **ESP32-S3-Zero** (Waveshare)
  - ESP32-S3FH4R2 chip (4MB Flash, 2MB PSRAM)
  - Onboard WS2812 RGB LED on GPIO21 for status indication
  - USB Type-C for programming and power (COM9)
  - Compact form factor with castellated holes
  - **Note**: GPIO33-37 are reserved for PSRAM and not available
- **2x AS5600 Magnetic Rotary Position Sensors**
  - 12-bit resolution (0-4095 counts = 0-360°)
  - I2C interface with fixed address 0x36
  - Contactless magnetic angle measurement

## Pin Configuration

### I2C Buses
- **I2C Bus 0** (First AS5600):
  - SDA: GPIO 1
  - SCL: GPIO 2
  
- **I2C Bus 1** (Second AS5600):
  - SDA: GPIO 3
  - SCL: GPIO 4

### UART Serial Communication
- **TX**: GPIO 43 (U0TXD - default UART0 TX)
- **RX**: GPIO 44 (U0RXD - default UART0 RX)
- **Baud Rate**: 115200

### Status LED
- **RGB LED**: GPIO 21 (onboard WS2812)

> **Note**: All pin assignments can be modified in `include/config.h`
> 
> **Important**: GPIO33-37 are NOT available on ESP32-S3-Zero (used for PSRAM). Alternative I2C pins: GPIO5-18, GPIO38-42.

## Features

### 1. Dual Sensor Reading
- Reads angular position from two AS5600 sensors independently
- Each sensor connected to its own I2C bus to avoid address conflicts
- 12-bit resolution (4096 positions per revolution)

### 2. Configurable Sample Rate
- Default: 50 Hz (20ms interval)
- Adjustable from 10-100 Hz in `config.h`
- Prevents overloading I2C and UART buses

### 3. Binary Serial Protocol
- Efficient 7-byte packet format for fast transmission
- Includes start/end markers and checksum for data integrity
- Optimized for ESP32-to-ESP32 communication

### 4. RGB LED Status Indication
- **Yellow**: Initializing
- **Green**: Both sensors connected and working
- **Blue**: Only one sensor connected
- **Red**: No sensors detected or error

### 5. WiFi Web Server with Real-Time Monitoring
- **Optional WiFi connectivity** - Enable by setting SSID in `config.h`
- **Web-based dashboard** - Beautiful, responsive interface for monitoring sensors
- **WebSocket updates** - Real-time sensor data streaming (configurable update rate)
- **Auto-reconnect** - Automatic reconnection if WiFi connection is lost
- **Conditional compilation** - WiFi disabled if SSID is empty (no overhead)

### 6. OTA (Over-The-Air) Updates
- **Wireless firmware updates** - Upload new firmware without USB cable
- **Password protected** - Secure OTA updates with authentication
- **Visual feedback** - RGB LED indicates update progress and status
- **Automatic activation** - OTA enabled automatically when WiFi is connected
- **Easy switching** - Use `pio run -e ota --target upload` for wireless updates

### 7. Debug Mode
- Optional debug output via USB Serial (programming port)
- Can be enabled/disabled in `config.h` without code changes

## Binary Data Protocol

Each data packet is **7 bytes** in the following format:

```
Byte 0:     Start Marker (0xAA)
Byte 1-2:   Sensor 1 Angle (uint16_t, little-endian, 0-4095)
Byte 3-4:   Sensor 2 Angle (uint16_t, little-endian, 0-4095)
Byte 5:     Checksum (XOR of all angle bytes)
Byte 6:     End Marker (0x55)
```

### Checksum Calculation
```cpp
checksum = (angle1_high ^ angle1_low ^ angle2_high ^ angle2_low)
```

### Example Packet
```
0xAA 0x12 0x34 0x56 0x78 0x5E 0x55
     └─────┘   └─────┘   └──┘
     Angle1    Angle2    Checksum
```

## Configuration

All configuration is centralized in `include/config.h`:

```cpp
// Sampling frequency
#define SAMPLE_RATE_HZ      50      // 10-100 Hz recommended

// I2C pins and frequency (ESP32-S3-Zero)
#define I2C_BUS0_SDA_PIN    1
#define I2C_BUS0_SCL_PIN    2
#define I2C_BUS1_SDA_PIN    3
#define I2C_BUS1_SCL_PIN    4

// Serial communication
#define SERIAL_TX_PIN       43
#define SERIAL_RX_PIN       44
#define SERIAL_BAUD_RATE    115200

// RGB LED (WS2812 on GPIO21)
#define RGB_LED_PIN         21

// Debug mode
#define DEBUG_ENABLED       false   // Set to true for USB debug output

// WiFi configuration (optional)
#define WIFI_SSID           ""      // Your WiFi SSID (leave empty to disable WiFi)
#define WIFI_PASSWORD       ""      // Your WiFi password
#define WEB_SERVER_PORT     80      // Web server port
#define WEBSOCKET_UPDATE_MS 100     // WebSocket update interval (milliseconds)
```

### WiFi Web Interface

To enable the web interface:

1. **Configure WiFi credentials** using one of these methods:

   **Method 1: Using secrets.h (Recommended for git repos)**
   ```bash
   # Copy the template
   cp include/secrets.h.template include/secrets.h
   
   # Edit include/secrets.h with your credentials
   # This file is gitignored and won't be committed
   ```
   
   **Method 2: Direct configuration**
   ```cpp
   // Edit include/config.h (not recommended for public repos)
   #define WIFI_SSID           "YourWiFiNetwork"
   #define WIFI_PASSWORD       "YourPassword"
   ```

2. **Build and upload** the firmware

3. **Check the serial monitor** for the assigned IP address:
   ```
   === WiFi Configuration ===
   SSID: YourWiFiNetwork
   Connecting to WiFi.....
   WiFi connected!
   IP Address: 192.168.1.100
   Web server started on port 80
   WebSocket server started on port 81
   ```

4. **Open your browser** and navigate to the IP address (e.g., `http://192.168.1.100`)

The web interface displays:
- Real-time angle values in degrees (0-360°)
- Raw sensor values (0-4095)
- Visual progress bars for each sensor
- Connection status indicator
- Automatic reconnection on disconnect


## Building and Uploading

### Prerequisites
- [PlatformIO](https://platformio.org/) installed
- ESP32-S3-Zero connected via USB Type-C to COM9
- **Important**: Press and hold BOOT button (GPIO0) before connecting USB to enter programming mode

### Environments

This project has two PlatformIO environments:

- **`com`** - Serial upload via USB (COM9)
- **`ota`** - Wireless upload via WiFi (OTA - Over The Air)

### Initial Upload (via USB)

For the first upload, you must use the USB connection:

```bash
# Build the project
pio run -e com

# Upload via USB
pio run -e com --target upload
```

### OTA Upload (Wireless)

Once WiFi is configured and the device is connected to your network, you can upload wirelessly:

```bash
# Upload via OTA
pio run -e ota --target upload
```

**OTA Configuration:**
- **Hostname**: `esp32-as5600`
- **Password**: `esp32-as5600`
- **IP Address**: Configure in `platformio.ini` under `[env:ota]` → `upload_port`

**Note**: OTA is only available when WiFi is enabled and connected. The device must be on the same network as your computer.

### OTA LED Status Indicators

During OTA updates, the RGB LED provides visual feedback:
- **Purple (blinking)**: OTA update in progress
- **White**: OTA update completed successfully
- **Red**: OTA update failed (check password and network connection)

After a successful OTA update, the device will automatically reboot and resume normal operation.

### Monitor (UART data port, not programming port)
```bash
pio device monitor
```

### Troubleshooting OTA

**OTA not working:**
- Ensure WiFi credentials are configured in `config.h` or `secrets.h`
- Verify the device is connected to WiFi (check serial monitor for IP address)
- Confirm the IP address in `platformio.ini` matches the device's IP
- Check that the OTA password matches (`esp32-as5600` by default)
- Ensure your computer and ESP32 are on the same network
- Try pinging the device: `ping 192.168.1.29`

**Upload fails with authentication error:**
- The password in `platformio.ini` (`--auth=esp32-as5600`) must match the password in `main.cpp`


> **Note**: For detailed code architecture and project structure, see [ARCHITECTURE.md](ARCHITECTURE.md)

## Dependencies

The following libraries are automatically installed by PlatformIO:

- **Adafruit NeoPixel** (^1.12.0) - RGB LED control
- **AS5600** by RobTillaart (^0.6.1) - AS5600 sensor interface
- **ArduinoJson** (^6.21.3) - JSON serialization for WebSocket data
- **WebSockets** by Links2004 (^2.4.1) - WebSocket server for real-time updates

## Usage Workflow

1. **Power up** the ESP32-S3 module
2. **RGB LED shows yellow** during initialization
3. **RGB LED changes color** based on sensor detection:
   - Green: Both sensors OK
   - Blue: One sensor detected
   - Red: No sensors detected
4. **Data transmission begins** automatically at configured sample rate
5. **Receiving ESP32** reads binary packets from UART and processes angle data

## Troubleshooting

### No Sensors Detected (Red LED)
- Check I2C wiring (SDA, SCL, VCC, GND)
- Verify sensors are powered (3.3V or 5V depending on module)
- Ensure magnets are positioned correctly over sensors
- Enable `DEBUG_ENABLED` in config.h to see detailed I2C status

### Only One Sensor Detected (Blue LED)
- Check wiring for the non-detected sensor
- Verify both I2C buses are properly initialized
- Check for loose connections

### No Serial Data Received
- Ensure correct UART pins are connected (TX→RX, RX→TX)
- Verify baud rate matches on both sender and receiver (115200)
- Check that you're using the UART USB port, not the programming port

### Compilation Errors
- Run `pio lib install` to ensure all dependencies are installed
- Check that `config.h` is in the `include/` directory
- Verify PlatformIO platform is up to date: `pio platform update`

## Future Enhancements

Potential improvements for this project:

- [ ] Add CRC16 checksum for better error detection
- [ ] Implement velocity calculation (RPM)
- [x] ~~Add WiFi/Bluetooth data transmission option~~ (WiFi implemented)
- [x] ~~Add OTA update support~~ (OTA implemented)
- [ ] Support for more than 2 sensors using I2C multiplexer
- [ ] Add configuration via web interface
- [ ] Store calibration data in non-volatile memory
- [ ] Add mDNS support for easier device discovery (esp32-as5600.local)

## References

### ESP32-S3-Zero Documentation
- [ESP32-S3-Zero Wiki](https://www.waveshare.com/wiki/ESP32-S3-Zero) - Official Waveshare documentation
- [WS2812 RGB LED Datasheet](https://files.waveshare.com/wiki/ESP32-S3-Zero/XL-0807RGBC-WS2812B.pdf) - Onboard LED specifications
- [ESP32-S3 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf) - Complete chip documentation
- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf) - Hardware specifications

### AS5600 Sensor Documentation
- [AS5600 Datasheet](https://ams.com/documents/20143/36005/AS5600_DS000365_5-00.pdf) - Magnetic encoder specifications
- [AS5600 Arduino Library](https://github.com/RobTillaart/AS5600) - RobTillaart's library documentation

### PlatformIO & Development
- [PlatformIO ESP32 Platform](https://docs.platformio.org/en/latest/platforms/espressif32.html) - Platform documentation
- [Arduino-ESP32 Documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/) - Arduino framework for ESP32

## License

This project is open-source and available for educational and commercial use.

## Author

Created for dual magnetic encoder position sensing with ESP32-S3.

---

**Last Updated**: January 2026
