#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// ESP32-S3-Zero Dual AS5600 Magnetic Encoder Reader - Configuration
// ============================================================================
// Board: Waveshare ESP32-S3-Zero
// Note: GPIO33-GPIO37 are NOT available (used for Octal PSRAM)

// ----------------------------------------------------------------------------
// I2C Bus 0 Configuration (First AS5600 Sensor)
// ----------------------------------------------------------------------------
#define I2C_BUS0_SDA_PIN    5       // GPIO 5 - I2C Bus 0 SDA
#define I2C_BUS0_SCL_PIN    6       // GPIO 6 - I2C Bus 0 SCL
#define I2C_BUS0_FREQ       400000  // 400kHz (Fast Mode)

// ----------------------------------------------------------------------------
// I2C Bus 1 Configuration (Second AS5600 Sensor)
// ----------------------------------------------------------------------------
#define I2C_BUS1_SDA_PIN    7       // GPIO 7 - I2C Bus 1 SDA
#define I2C_BUS1_SCL_PIN    8       // GPIO 8 - I2C Bus 1 SCL
#define I2C_BUS1_FREQ       400000  // 400kHz (Fast Mode)

// ----------------------------------------------------------------------------
// Serial Communication Configuration
// ----------------------------------------------------------------------------
// Using Serial1 (UART1) for data transmission to another ESP32
// Note: Serial0 (USB) is on the programming port, Serial1 is on the UART port
#define SERIAL_TX_PIN       43      // GPIO 43 - UART TX (U0TXD)
#define SERIAL_RX_PIN       44      // GPIO 44 - UART RX (U0RXD)
#define SERIAL_BAUD_RATE    115200  // Baud rate for serial communication

// ----------------------------------------------------------------------------
// RGB LED Configuration (for status indication)
// ----------------------------------------------------------------------------
#define RGB_LED_PIN         21      // GPIO 21 - Onboard WS2812 RGB LED
#define RGB_LED_BRIGHTNESS  50      // Brightness (0-255)

// ----------------------------------------------------------------------------
// Sampling Configuration
// ----------------------------------------------------------------------------
#define SAMPLE_RATE_HZ      50      // Samples per second (50 Hz = 20ms interval)
                                    // Recommended range: 10-100 Hz
                                    // Lower values = less CPU/I2C/UART load
                                    // Higher values = more responsive but higher load

// Calculate sampling interval in milliseconds
#define SAMPLE_INTERVAL_MS  (1000 / SAMPLE_RATE_HZ)

// ----------------------------------------------------------------------------
// AS5600 Sensor Configuration
// ----------------------------------------------------------------------------
#define AS5600_I2C_ADDRESS  0x36    // Fixed I2C address for AS5600

// ----------------------------------------------------------------------------
// Data Protocol Configuration
// ----------------------------------------------------------------------------
// Binary protocol format for serial transmission:
// [START_BYTE][SENSOR1_HIGH][SENSOR1_LOW][SENSOR2_HIGH][SENSOR2_LOW][CHECKSUM][END_BYTE]
// Total: 7 bytes per packet
#define PACKET_START_BYTE   0xAA    // Packet start marker
#define PACKET_END_BYTE     0x55    // Packet end marker

// ----------------------------------------------------------------------------
// WiFi Configuration
// ----------------------------------------------------------------------------
// WiFi credentials can be configured in two ways:
// 1. Create "secrets.h" in this directory with WIFI_SSID_SECRET and WIFI_PASSWORD_SECRET
//    (recommended - secrets.h is gitignored)
// 2. Or modify the default values below (not recommended for public repos)
//
// To use secrets.h:
//   - Copy secrets.h.template to secrets.h
//   - Edit secrets.h with your credentials
//   - secrets.h will be automatically used if it exists

// Try to include secrets.h if it exists (this file is gitignored)
#if __has_include("secrets.h")
  #include "secrets.h"
  #define WIFI_SSID           WIFI_SSID_SECRET
  #define WIFI_PASSWORD       WIFI_PASSWORD_SECRET
#else
  // Default values (leave empty to disable WiFi)
  #define WIFI_SSID           ""          // Your WiFi SSID
  #define WIFI_PASSWORD       ""          // Your WiFi password
#endif

#define WEB_SERVER_PORT     80          // Web server port (default: 80)
#define WEBSOCKET_UPDATE_MS 100         // WebSocket update interval in milliseconds

// ----------------------------------------------------------------------------
// Logging Configuration
// ----------------------------------------------------------------------------
// Number of log messages to keep in memory for web interface display
// DEBUG level logs are not stored, only INFO, WARN, and ERROR
#define LOG_BUFFER_SIZE     20

// ----------------------------------------------------------------------------
// Debug Configuration
// ----------------------------------------------------------------------------
// Set to true to enable debug output on Serial (USB programming port)
// WARNING: This will slow down the system and affect timing
#define DEBUG_ENABLED       true

#endif // CONFIG_H
