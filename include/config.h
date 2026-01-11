#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// ESP32-S3 Dual AS5600 Magnetic Encoder Reader - Configuration
// ============================================================================

// ----------------------------------------------------------------------------
// I2C Bus 0 Configuration (First AS5600 Sensor)
// ----------------------------------------------------------------------------
#define I2C_BUS0_SDA_PIN    8       // GPIO 8 - I2C Bus 0 SDA
#define I2C_BUS0_SCL_PIN    9       // GPIO 9 - I2C Bus 0 SCL
#define I2C_BUS0_FREQ       400000  // 400kHz (Fast Mode)

// ----------------------------------------------------------------------------
// I2C Bus 1 Configuration (Second AS5600 Sensor)
// ----------------------------------------------------------------------------
#define I2C_BUS1_SDA_PIN    10      // GPIO 10 - I2C Bus 1 SDA
#define I2C_BUS1_SCL_PIN    11      // GPIO 11 - I2C Bus 1 SCL
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
#define RGB_LED_PIN         48      // GPIO 48 - Onboard RGB LED
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
// Debug Configuration
// ----------------------------------------------------------------------------
// Set to true to enable debug output on Serial (USB programming port)
// WARNING: This will slow down the system and affect timing
#define DEBUG_ENABLED       false

#endif // CONFIG_H
