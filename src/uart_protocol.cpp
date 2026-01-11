#include "uart_protocol.h"
#include "config.h"

// ============================================================================
// Constructor
// ============================================================================
UartProtocol::UartProtocol(HardwareSerial& serial) : _serial(serial) {
}

// ============================================================================
// Initialize UART
// ============================================================================
void UartProtocol::begin(uint32_t baudRate, uint8_t txPin, uint8_t rxPin) {
  _serial.begin(baudRate, SERIAL_8N1, rxPin, txPin);
  
  #if DEBUG_ENABLED
    Serial.printf("UART initialized: TX=%d, RX=%d, Baud=%d\n", 
                  txPin, rxPin, baudRate);
  #endif
}

// ============================================================================
// Transmit Sensor Data
// ============================================================================
void UartProtocol::transmit(uint16_t angle1, uint16_t angle2) {
  buildPacket(angle1, angle2);
  _serial.write((uint8_t*)&_packet, sizeof(_packet));

  #if DEBUG_ENABLED
    Serial.printf("TX: Angle1=%4d (%.2f°), Angle2=%4d (%.2f°), Checksum=0x%02X\n",
                  angle1, angle1 * 360.0 / 4096.0,
                  angle2, angle2 * 360.0 / 4096.0,
                  _packet.checksum);
  #endif
}

// ============================================================================
// Build Packet
// ============================================================================
void UartProtocol::buildPacket(uint16_t angle1, uint16_t angle2) {
  _packet.startByte = PACKET_START_BYTE;
  _packet.angle1 = angle1;
  _packet.angle2 = angle2;
  _packet.checksum = calculateChecksum(angle1, angle2);
  _packet.endByte = PACKET_END_BYTE;
}

// ============================================================================
// Calculate Checksum
// ============================================================================
uint8_t UartProtocol::calculateChecksum(uint16_t angle1, uint16_t angle2) {
  uint8_t checksum = 0;
  checksum ^= (angle1 >> 8) & 0xFF;   // High byte of angle1
  checksum ^= angle1 & 0xFF;          // Low byte of angle1
  checksum ^= (angle2 >> 8) & 0xFF;   // High byte of angle2
  checksum ^= angle2 & 0xFF;          // Low byte of angle2
  return checksum;
}
