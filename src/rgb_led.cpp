#include "rgb_led.h"

// ============================================================================
// Constructor
// ============================================================================
RgbLed::RgbLed(uint8_t pin, uint8_t brightness)
  : _rgb(1, pin, NEO_GRB + NEO_KHZ800), _brightness(brightness) {
}

// ============================================================================
// Initialize
// ============================================================================
void RgbLed::begin() {
  _rgb.begin();
  _rgb.setBrightness(_brightness);
  _rgb.show();
}

// ============================================================================
// Set Status Color
// ============================================================================
void RgbLed::setStatus(Status status) {
  uint32_t color = getStatusColor(status);
  _rgb.setPixelColor(0, color);
  _rgb.show();
}

// ============================================================================
// Set Custom Color
// ============================================================================
void RgbLed::setColor(uint8_t r, uint8_t g, uint8_t b) {
  _rgb.setPixelColor(0, _rgb.Color(r, g, b));
  _rgb.show();
}

// ============================================================================
// Turn Off
// ============================================================================
void RgbLed::off() {
  _rgb.setPixelColor(0, 0);
  _rgb.show();
}

// ============================================================================
// Set Brightness
// ============================================================================
void RgbLed::setBrightness(uint8_t brightness) {
  _brightness = brightness;
  _rgb.setBrightness(_brightness);
  _rgb.show();
}

// ============================================================================
// Get Status Color
// ============================================================================
uint32_t RgbLed::getStatusColor(Status status) {
  switch (status) {
    case INIT:
      return _rgb.Color(255, 255, 0);   // Yellow
    case OK:
      return _rgb.Color(0, 255, 0);     // Green
    case PARTIAL:
      return _rgb.Color(0, 0, 255);     // Blue
    case ERROR:
      return _rgb.Color(255, 0, 0);     // Red
    case WIFI_AP:
      return _rgb.Color(0, 255, 255);   // Cyan
    case WIFI_STA:
      return _rgb.Color(255, 0, 255);   // Magenta
    default:
      return 0;                          // Off
  }
}
