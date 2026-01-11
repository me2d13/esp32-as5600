#include "sensor_manager.h"
#include "config.h"

// ============================================================================
// Constructor
// ============================================================================
SensorManager::SensorManager()
  : _sensor1(&Wire), _sensor2(&Wire1),
    _sensor1Connected(false), _sensor2Connected(false) {
}

// ============================================================================
// Initialize I2C Buses and Sensors
// ============================================================================
void SensorManager::begin(uint8_t sda0, uint8_t scl0, uint32_t freq0,
                          uint8_t sda1, uint8_t scl1, uint32_t freq1) {
  // Initialize I2C Bus 0
  Wire.begin(sda0, scl0, freq0);
  
  #if DEBUG_ENABLED
    Serial.printf("I2C Bus 0: SDA=%d, SCL=%d, Freq=%d Hz\n", sda0, scl0, freq0);
  #endif

  // Initialize I2C Bus 1
  Wire1.begin(sda1, scl1, freq1);
  
  #if DEBUG_ENABLED
    Serial.printf("I2C Bus 1: SDA=%d, SCL=%d, Freq=%d Hz\n", sda1, scl1, freq1);
  #endif

  // Initialize sensors
  _sensor1Connected = initSensor(_sensor1, "Sensor 1 (Bus 0)");
  _sensor2Connected = initSensor(_sensor2, "Sensor 2 (Bus 1)");
}

// ============================================================================
// Initialize Individual Sensor
// ============================================================================
bool SensorManager::initSensor(AS5600& sensor, const char* name) {
  sensor.begin();
  bool connected = sensor.isConnected();
  
  #if DEBUG_ENABLED
    Serial.printf("%s: %s\n", name, connected ? "CONNECTED" : "NOT FOUND");
    if (connected) {
      Serial.printf("  - Magnet detected: %s\n", 
                    sensor.detectMagnet() ? "YES" : "NO");
    }
  #endif

  return connected;
}

// ============================================================================
// Read Angle from Sensor 1
// ============================================================================
uint16_t SensorManager::readAngle1() {
  if (_sensor1Connected) {
    return _sensor1.readAngle();
  }
  return 0;
}

// ============================================================================
// Read Angle from Sensor 2
// ============================================================================
uint16_t SensorManager::readAngle2() {
  if (_sensor2Connected) {
    return _sensor2.readAngle();
  }
  return 0;
}

// ============================================================================
// Read Both Angles
// ============================================================================
void SensorManager::readAngles(uint16_t& angle1, uint16_t& angle2) {
  angle1 = readAngle1();
  angle2 = readAngle2();
}

// ============================================================================
// Check Magnet Detection
// ============================================================================
bool SensorManager::isMagnet1Detected() {
  if (_sensor1Connected) {
    return _sensor1.detectMagnet();
  }
  return false;
}

bool SensorManager::isMagnet2Detected() {
  if (_sensor2Connected) {
    return _sensor2.detectMagnet();
  }
  return false;
}
