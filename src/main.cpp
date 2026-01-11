#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <AS5600.h>
#include <Wire.h>
#include "config.h"

// ============================================================================
// Global Objects
// ============================================================================

// RGB LED for status indication
Adafruit_NeoPixel rgb(1, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

// AS5600 sensor objects
AS5600 sensor1(&Wire);   // First sensor on I2C Bus 0
AS5600 sensor2(&Wire1);  // Second sensor on I2C Bus 1

// ============================================================================
// Status LED Colors
// ============================================================================
#define COLOR_INIT      rgb.Color(255, 255, 0)   // Yellow - Initializing
#define COLOR_OK        rgb.Color(0, 255, 0)     // Green - Both sensors OK
#define COLOR_ERROR     rgb.Color(255, 0, 0)     // Red - Sensor error
#define COLOR_PARTIAL   rgb.Color(0, 0, 255)     // Blue - Only one sensor OK

// ============================================================================
// Global Variables
// ============================================================================
unsigned long lastSampleTime = 0;
bool sensor1Connected = false;
bool sensor2Connected = false;

// Data packet structure for binary transmission
struct SensorData {
  uint8_t startByte;      // 0xAA
  uint16_t angle1;        // Sensor 1 angle (0-4095, 12-bit)
  uint16_t angle2;        // Sensor 2 angle (0-4095, 12-bit)
  uint8_t checksum;       // Simple XOR checksum
  uint8_t endByte;        // 0x55
} __attribute__((packed));

// ============================================================================
// Function Prototypes
// ============================================================================
void initI2C();
void initSensors();
void setStatusLED(uint32_t color);
void readAndTransmitData();
uint8_t calculateChecksum(uint16_t angle1, uint16_t angle2);

// ============================================================================
// Setup Function
// ============================================================================
void setup() {
  // Initialize USB Serial for debugging (if enabled)
  #if DEBUG_ENABLED
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=================================");
    Serial.println("ESP32-S3 Dual AS5600 Reader");
    Serial.println("=================================");
  #endif

  // Initialize RGB LED
  rgb.begin();
  rgb.setBrightness(RGB_LED_BRIGHTNESS);
  setStatusLED(COLOR_INIT);

  // Initialize UART Serial for data transmission
  Serial1.begin(SERIAL_BAUD_RATE, SERIAL_8N1, SERIAL_RX_PIN, SERIAL_TX_PIN);
  
  #if DEBUG_ENABLED
    Serial.printf("Serial1 initialized: TX=%d, RX=%d, Baud=%d\n", 
                  SERIAL_TX_PIN, SERIAL_RX_PIN, SERIAL_BAUD_RATE);
  #endif

  // Initialize I2C buses
  initI2C();

  // Initialize AS5600 sensors
  initSensors();

  // Set initial status LED based on sensor connection
  if (sensor1Connected && sensor2Connected) {
    setStatusLED(COLOR_OK);
  } else if (sensor1Connected || sensor2Connected) {
    setStatusLED(COLOR_PARTIAL);
  } else {
    setStatusLED(COLOR_ERROR);
  }

  #if DEBUG_ENABLED
    Serial.printf("Sample rate: %d Hz (%d ms interval)\n", 
                  SAMPLE_RATE_HZ, SAMPLE_INTERVAL_MS);
    Serial.println("Starting main loop...\n");
  #endif

  lastSampleTime = millis();
}

// ============================================================================
// Main Loop
// ============================================================================
void loop() {
  unsigned long currentTime = millis();

  // Check if it's time to sample
  if (currentTime - lastSampleTime >= SAMPLE_INTERVAL_MS) {
    lastSampleTime = currentTime;
    readAndTransmitData();
  }

  // Small delay to prevent watchdog issues
  delay(1);
}

// ============================================================================
// Initialize I2C Buses
// ============================================================================
void initI2C() {
  // Initialize I2C Bus 0 (Wire)
  Wire.begin(I2C_BUS0_SDA_PIN, I2C_BUS0_SCL_PIN, I2C_BUS0_FREQ);
  
  #if DEBUG_ENABLED
    Serial.printf("I2C Bus 0: SDA=%d, SCL=%d, Freq=%d Hz\n", 
                  I2C_BUS0_SDA_PIN, I2C_BUS0_SCL_PIN, I2C_BUS0_FREQ);
  #endif

  // Initialize I2C Bus 1 (Wire1)
  Wire1.begin(I2C_BUS1_SDA_PIN, I2C_BUS1_SCL_PIN, I2C_BUS1_FREQ);
  
  #if DEBUG_ENABLED
    Serial.printf("I2C Bus 1: SDA=%d, SCL=%d, Freq=%d Hz\n", 
                  I2C_BUS1_SDA_PIN, I2C_BUS1_SCL_PIN, I2C_BUS1_FREQ);
  #endif
}

// ============================================================================
// Initialize AS5600 Sensors
// ============================================================================
void initSensors() {
  // Initialize Sensor 1
  sensor1.begin();
  sensor1Connected = sensor1.isConnected();
  
  #if DEBUG_ENABLED
    Serial.printf("Sensor 1 (Bus 0): %s\n", 
                  sensor1Connected ? "CONNECTED" : "NOT FOUND");
    if (sensor1Connected) {
      Serial.printf("  - Magnet detected: %s\n", 
                    sensor1.magnetDetected() ? "YES" : "NO");
    }
  #endif

  // Initialize Sensor 2
  sensor2.begin();
  sensor2Connected = sensor2.isConnected();
  
  #if DEBUG_ENABLED
    Serial.printf("Sensor 2 (Bus 1): %s\n", 
                  sensor2Connected ? "CONNECTED" : "NOT FOUND");
    if (sensor2Connected) {
      Serial.printf("  - Magnet detected: %s\n", 
                    sensor2.magnetDetected() ? "YES" : "NO");
    }
  #endif
}

// ============================================================================
// Set Status LED Color
// ============================================================================
void setStatusLED(uint32_t color) {
  rgb.setPixelColor(0, color);
  rgb.show();
}

// ============================================================================
// Read Sensors and Transmit Data
// ============================================================================
void readAndTransmitData() {
  SensorData packet;
  
  // Read raw angles from sensors (0-4095, 12-bit resolution)
  uint16_t angle1 = 0;
  uint16_t angle2 = 0;

  if (sensor1Connected) {
    angle1 = sensor1.readAngle();
  }

  if (sensor2Connected) {
    angle2 = sensor2.readAngle();
  }

  // Build packet
  packet.startByte = PACKET_START_BYTE;
  packet.angle1 = angle1;
  packet.angle2 = angle2;
  packet.checksum = calculateChecksum(angle1, angle2);
  packet.endByte = PACKET_END_BYTE;

  // Transmit binary packet via Serial1
  Serial1.write((uint8_t*)&packet, sizeof(packet));

  #if DEBUG_ENABLED
    Serial.printf("TX: Angle1=%4d (%.2f°), Angle2=%4d (%.2f°), Checksum=0x%02X\n",
                  angle1, angle1 * 360.0 / 4096.0,
                  angle2, angle2 * 360.0 / 4096.0,
                  packet.checksum);
  #endif
}

// ============================================================================
// Calculate Simple XOR Checksum
// ============================================================================
uint8_t calculateChecksum(uint16_t angle1, uint16_t angle2) {
  uint8_t checksum = 0;
  checksum ^= (angle1 >> 8) & 0xFF;   // High byte of angle1
  checksum ^= angle1 & 0xFF;          // Low byte of angle1
  checksum ^= (angle2 >> 8) & 0xFF;   // High byte of angle2
  checksum ^= angle2 & 0xFF;          // Low byte of angle2
  return checksum;
}
