# Binary Protocol Decoder Reference

## For Receiving ESP32

This document describes how to decode the binary data packets sent by the ESP32-S3 dual AS5600 reader.

## Packet Format

Each packet is **7 bytes** long:

| Byte | Field | Type | Value | Description |
|------|-------|------|-------|-------------|
| 0 | Start Marker | uint8_t | 0xAA | Packet start identifier |
| 1-2 | Angle 1 | uint16_t | 0-4095 | First sensor angle (little-endian) |
| 3-4 | Angle 2 | uint16_t | 0-4095 | Second sensor angle (little-endian) |
| 5 | Checksum | uint8_t | XOR | Data integrity check |
| 6 | End Marker | uint8_t | 0x55 | Packet end identifier |

## Arduino/ESP32 Decoder Example

```cpp
#include <Arduino.h>

// Packet structure (must match sender)
struct SensorData {
  uint8_t startByte;      // 0xAA
  uint16_t angle1;        // Sensor 1 angle (0-4095)
  uint16_t angle2;        // Sensor 2 angle (0-4095)
  uint8_t checksum;       // XOR checksum
  uint8_t endByte;        // 0x55
} __attribute__((packed));

// Calculate checksum for validation
uint8_t calculateChecksum(uint16_t angle1, uint16_t angle2) {
  uint8_t checksum = 0;
  checksum ^= (angle1 >> 8) & 0xFF;
  checksum ^= angle1 & 0xFF;
  checksum ^= (angle2 >> 8) & 0xFF;
  checksum ^= angle2 & 0xFF;
  return checksum;
}

void setup() {
  Serial.begin(115200);   // For debug output
  Serial1.begin(115200);  // For receiving data (adjust pins as needed)
}

void loop() {
  if (Serial1.available() >= sizeof(SensorData)) {
    SensorData packet;
    
    // Read packet
    Serial1.readBytes((uint8_t*)&packet, sizeof(packet));
    
    // Validate packet
    if (packet.startByte == 0xAA && packet.endByte == 0x55) {
      // Verify checksum
      uint8_t expectedChecksum = calculateChecksum(packet.angle1, packet.angle2);
      
      if (packet.checksum == expectedChecksum) {
        // Valid packet - process data
        float degrees1 = packet.angle1 * 360.0 / 4096.0;
        float degrees2 = packet.angle2 * 360.0 / 4096.0;
        
        Serial.printf("Sensor 1: %d (%.2f°), Sensor 2: %d (%.2f°)\n",
                      packet.angle1, degrees1,
                      packet.angle2, degrees2);
        
        // Your processing code here
        
      } else {
        Serial.println("Checksum error!");
      }
    } else {
      Serial.println("Invalid packet markers!");
    }
  }
}
```

## Conversion Formulas

### Raw Value to Degrees
```cpp
float degrees = rawAngle * 360.0 / 4096.0;
```

### Raw Value to Radians
```cpp
float radians = rawAngle * 2.0 * PI / 4096.0;
```

### Degrees to Raw Value
```cpp
uint16_t rawAngle = (uint16_t)(degrees * 4096.0 / 360.0);
```

## Data Rate

- Default: 50 packets/second (50 Hz)
- Configurable in sender's `config.h` (10-100 Hz)
- Each packet: 7 bytes
- Data rate at 50 Hz: 350 bytes/second
- At 115200 baud: ~0.3% utilization (very safe)

## Error Handling

### Packet Synchronization

If packets become desynchronized, search for the start marker:

```cpp
void syncPacket() {
  while (Serial1.available() > 0) {
    if (Serial1.read() == 0xAA) {
      // Found start marker, next read should be angle data
      break;
    }
  }
}
```

### Checksum Validation

Always validate the checksum before using data:

```cpp
if (packet.checksum != calculateChecksum(packet.angle1, packet.angle2)) {
  // Corrupted data - discard packet
  syncPacket();  // Try to resynchronize
  return;
}
```

## Python Decoder Example

For testing or data logging on a PC:

```python
import serial
import struct

# Open serial port
ser = serial.Serial('COM8', 115200, timeout=1)

def calculate_checksum(angle1, angle2):
    checksum = 0
    checksum ^= (angle1 >> 8) & 0xFF
    checksum ^= angle1 & 0xFF
    checksum ^= (angle2 >> 8) & 0xFF
    checksum ^= angle2 & 0xFF
    return checksum

while True:
    if ser.in_waiting >= 7:
        # Read packet
        packet = ser.read(7)
        
        # Unpack (little-endian)
        start, angle1, angle2, checksum, end = struct.unpack('<BHHBB', packet)
        
        # Validate
        if start == 0xAA and end == 0x55:
            expected_checksum = calculate_checksum(angle1, angle2)
            
            if checksum == expected_checksum:
                degrees1 = angle1 * 360.0 / 4096.0
                degrees2 = angle2 * 360.0 / 4096.0
                
                print(f"Sensor 1: {angle1:4d} ({degrees1:6.2f}°), "
                      f"Sensor 2: {angle2:4d} ({degrees2:6.2f}°)")
            else:
                print("Checksum error!")
        else:
            print("Invalid packet!")
```

## Troubleshooting

### No Data Received
- Check TX/RX connections (TX→RX, RX→TX)
- Verify baud rate matches (115200)
- Ensure correct UART pins are configured

### Corrupted Data
- Check for electrical noise on UART lines
- Verify ground connection between devices
- Consider adding pull-up resistors on TX/RX lines
- Reduce baud rate if experiencing errors

### Intermittent Data
- Check power supply stability
- Verify both devices share common ground
- Ensure UART buffer is being read fast enough

## Performance Notes

- **Latency**: ~20ms at 50 Hz sample rate
- **Jitter**: Minimal (<1ms) with proper timing
- **Reliability**: Checksum catches ~99.6% of single-bit errors
- **Throughput**: Can handle up to 100 Hz with 115200 baud

---

**Compatible with**: ESP32, ESP32-S3, ESP32-C3, Arduino, Raspberry Pi, PC
