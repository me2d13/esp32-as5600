#ifndef UART_PROTOCOL_H
#define UART_PROTOCOL_H

#include <Arduino.h>
#include <stdint.h>

// ============================================================================
// UART Binary Protocol Manager
// ============================================================================

class UartProtocol {
public:
  // Packet structure for binary transmission
  struct SensorData {
    uint8_t startByte;      // 0xAA
    uint16_t angle1;        // Sensor 1 angle (0-4095, 12-bit)
    uint16_t angle2;        // Sensor 2 angle (0-4095, 12-bit)
    uint8_t checksum;       // Simple XOR checksum
    uint8_t endByte;        // 0x55
  } __attribute__((packed));

  // Constructor
  UartProtocol(HardwareSerial& serial);

  // Initialize UART
  void begin(uint32_t baudRate, uint8_t txPin, uint8_t rxPin);

  // Transmit sensor data
  void transmit(uint16_t angle1, uint16_t angle2);

  // Get packet size
  static constexpr size_t getPacketSize() { return sizeof(SensorData); }

  // Calculate checksum (static for receiver use)
  static uint8_t calculateChecksum(uint16_t angle1, uint16_t angle2);

private:
  HardwareSerial& _serial;
  SensorData _packet;

  // Build packet
  void buildPacket(uint16_t angle1, uint16_t angle2);
};

#endif // UART_PROTOCOL_H
