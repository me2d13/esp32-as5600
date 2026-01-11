#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <AS5600.h>
#include <Wire.h>
#include <stdint.h>

// ============================================================================
// AS5600 Sensor Manager
// ============================================================================

class SensorManager {
public:
  // Constructor
  SensorManager();

  // Initialize I2C buses and sensors
  void begin(uint8_t sda0, uint8_t scl0, uint32_t freq0,
             uint8_t sda1, uint8_t scl1, uint32_t freq1);

  // Check if sensors are connected
  bool isSensor1Connected() const { return _sensor1Connected; }
  bool isSensor2Connected() const { return _sensor2Connected; }
  bool areBothConnected() const { return _sensor1Connected && _sensor2Connected; }
  bool isAnyConnected() const { return _sensor1Connected || _sensor2Connected; }

  // Read angles from sensors (0-4095)
  uint16_t readAngle1();
  uint16_t readAngle2();

  // Read both angles at once
  void readAngles(uint16_t& angle1, uint16_t& angle2);

  // Check magnet status
  bool isMagnet1Detected();
  bool isMagnet2Detected();

  // Get sensor objects (for advanced usage)
  AS5600& getSensor1() { return _sensor1; }
  AS5600& getSensor2() { return _sensor2; }

private:
  AS5600 _sensor1;
  AS5600 _sensor2;
  bool _sensor1Connected;
  bool _sensor2Connected;

  // Initialize individual sensor
  bool initSensor(AS5600& sensor, const char* name);
};

#endif // SENSOR_MANAGER_H
