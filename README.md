# ESP32-S3 Dual AS5600 Magnetic Encoder Reader

## Project Overview

This project uses an **ESP32-S3-WROOM1 N16R8** module to read angular position data from **two AS5600 magnetic rotary encoders** simultaneously and transmit the data via UART to another ESP32 module for further processing.

### Why Two I2C Buses?

The AS5600 magnetic encoder has a **fixed I2C address (0x36)** that cannot be changed. To read from two AS5600 sensors simultaneously, this project utilizes the ESP32-S3's capability to support **multiple I2C buses** (Wire and Wire1), with each sensor connected to a separate bus.

## Hardware Components

- **ESP32-S3-WROOM1 N16R8** (44-pin, Type-C, 16MB Flash, 8MB PSRAM)
  - Onboard RGB LED (WS2812) for status indication
  - Dual USB ports: one for programming (COM7), one for UART communication
- **2x AS5600 Magnetic Rotary Position Sensors**
  - 12-bit resolution (0-4095 counts = 0-360°)
  - I2C interface with fixed address 0x36
  - Contactless magnetic angle measurement

## Pin Configuration

### I2C Buses
- **I2C Bus 0** (First AS5600):
  - SDA: GPIO 8
  - SCL: GPIO 9
  
- **I2C Bus 1** (Second AS5600):
  - SDA: GPIO 10
  - SCL: GPIO 11

### UART Serial Communication
- **TX**: GPIO 43 (U0TXD)
- **RX**: GPIO 44 (U0RXD)
- **Baud Rate**: 115200

### Status LED
- **RGB LED**: GPIO 48 (onboard WS2812)

> **Note**: All pin assignments can be modified in `include/config.h`

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

### 5. Debug Mode
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

// I2C pins and frequency
#define I2C_BUS0_SDA_PIN    8
#define I2C_BUS0_SCL_PIN    9
#define I2C_BUS1_SDA_PIN    10
#define I2C_BUS1_SCL_PIN    11

// Serial communication
#define SERIAL_TX_PIN       43
#define SERIAL_RX_PIN       44
#define SERIAL_BAUD_RATE    115200

// Debug mode
#define DEBUG_ENABLED       false   // Set to true for USB debug output
```

## Building and Uploading

### Prerequisites
- [PlatformIO](https://platformio.org/) installed
- ESP32-S3 connected via USB (programming port) to COM7

### Build
```bash
pio run
```

### Upload
```bash
pio run --target upload
```

### Monitor (UART data port, not programming port)
```bash
pio device monitor
```

## Project Structure

```
esp32-as5600/
├── include/
│   └── config.h              # Configuration file (pin assignments, sample rate)
├── src/
│   └── main.cpp              # Main application code
├── platformio.ini            # PlatformIO configuration
└── README.md                 # This file
```

## Dependencies

The following libraries are automatically installed by PlatformIO:

- **Adafruit NeoPixel** (^1.12.0) - RGB LED control
- **AS5600** by RobTillaart (^0.6.1) - AS5600 sensor interface

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
- [ ] Add WiFi/Bluetooth data transmission option
- [ ] Support for more than 2 sensors using I2C multiplexer
- [ ] Add configuration via web interface
- [ ] Store calibration data in non-volatile memory

## License

This project is open-source and available for educational and commercial use.

## Author

Created for dual magnetic encoder position sensing with ESP32-S3.

---

**Last Updated**: January 2026
