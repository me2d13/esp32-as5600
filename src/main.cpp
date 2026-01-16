#include <Arduino.h>
#include "config.h"
#include "rgb_led.h"
#include "sensor_manager.h"
#include "uart_protocol.h"
#include "web_server.h"

// ============================================================================
// Global Objects
// ============================================================================

RgbLed led(RGB_LED_PIN, RGB_LED_BRIGHTNESS);
SensorManager sensors;
UartProtocol uart(Serial1);
WebServerManager webServer;

// ============================================================================
// Global Variables
// ============================================================================
unsigned long lastSampleTime = 0;

// ============================================================================
// Setup Function
// ============================================================================
void setup() {
  // Initialize USB Serial for debugging (if enabled)
  #if DEBUG_ENABLED
    Serial.begin(115200);
    delay(3000);  // Give USB CDC time to enumerate
    while (!Serial && millis() < 5000) {
      delay(10);  // Wait for Serial connection, timeout after 5 seconds
    }
    Serial.println("\n\n=================================");
    Serial.println("ESP32-S3 Dual AS5600 Reader");
    Serial.println("=================================");
    Serial.println("DEBUG: Serial initialized");
  #endif

  // Initialize RGB LED
  led.begin();
  led.setStatus(RgbLed::INIT);

  // Initialize UART for data transmission
  uart.begin(SERIAL_BAUD_RATE, SERIAL_TX_PIN, SERIAL_RX_PIN);

  // Initialize I2C buses and sensors
  sensors.begin(I2C_BUS0_SDA_PIN, I2C_BUS0_SCL_PIN, I2C_BUS0_FREQ,
                I2C_BUS1_SDA_PIN, I2C_BUS1_SCL_PIN, I2C_BUS1_FREQ);

  // Set status LED based on sensor connection
  if (sensors.areBothConnected()) {
    led.setStatus(RgbLed::OK);
  } else if (sensors.isAnyConnected()) {
    led.setStatus(RgbLed::PARTIAL);
  } else {
    led.setStatus(RgbLed::ERROR);
  }

  // Initialize WiFi and web server (only if configured)
  webServer.begin(WIFI_SSID, WIFI_PASSWORD, WEB_SERVER_PORT);

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
    
    // Read angles from both sensors
    uint16_t angle1, angle2;
    sensors.readAngles(angle1, angle2);
    
    // Transmit data via UART
    uart.transmit(angle1, angle2);
    
    // Update web server with new sensor data
    if (webServer.isEnabled()) {
      webServer.updateSensorData(angle1, angle2);
    }
  }

  // Handle web server clients and broadcast WebSocket data
  if (webServer.isEnabled()) {
    webServer.handleClient();
    webServer.broadcastSensorData();
  }

  // Small delay to prevent watchdog issues
  delay(1);
}

