#ifndef RGB_LED_H
#define RGB_LED_H

#include <Adafruit_NeoPixel.h>
#include <stdint.h>

// ============================================================================
// RGB LED Status Manager
// ============================================================================

class RgbLed {
public:
  // Status colors
  enum Status {
    INIT,       // Yellow - Initializing
    OK,         // Green - Both sensors OK
    PARTIAL,    // Blue - Only one sensor OK
    ERROR,      // Red - Sensor error
    WIFI_AP,    // Cyan - WiFi AP mode
    WIFI_STA,   // Magenta - WiFi Station mode
    CUSTOM      // Custom color
  };

  // Constructor
  RgbLed(uint8_t pin, uint8_t brightness = 50);

  // Initialize the LED
  void begin();

  // Set status color
  void setStatus(Status status);

  // Set custom color (R, G, B)
  void setColor(uint8_t r, uint8_t g, uint8_t b);

  // Turn off LED
  void off();

  // Set brightness (0-255)
  void setBrightness(uint8_t brightness);

private:
  Adafruit_NeoPixel _rgb;
  uint8_t _brightness;

  // Get color for status
  uint32_t getStatusColor(Status status);
};

#endif // RGB_LED_H
