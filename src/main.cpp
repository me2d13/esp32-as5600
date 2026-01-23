#include <Arduino.h>
#include "config.h"
#include "logger.h"
#include "rgb_led.h"
#include "sensor_manager.h"
#include "uart_protocol.h"
#include "web_server.h"
#include "ota_update.h"

// ============================================================================
// Global Objects
// ============================================================================

RgbLed led(RGB_LED_PIN, RGB_LED_BRIGHTNESS);
SensorManager sensors;
UartProtocol uart(Serial1);
WebServerManager webServer;
OTAUpdate ota;

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
  #endif

  // Initialize logger
  logger.begin(LOG_BUFFER_SIZE);
  
  LOG_INFO("=================================");
  LOG_INFO("ESP32-S3 Dual AS5600 Reader");
  LOG_INFO("=================================");
  LOG_INFO("System initialized");

  // Initialize RGB LED
  led.begin();
  led.setStatus(RgbLed::INIT);

  // Initialize UART for data transmission
  uart.begin(SERIAL_BAUD_RATE, SERIAL_TX_PIN, SERIAL_RX_PIN);

  // Initialize I2C buses and sensors
  sensors.begin(I2C_BUS0_SDA_PIN, I2C_BUS0_SCL_PIN, I2C_BUS0_FREQ,
                I2C_BUS1_SDA_PIN, I2C_BUS1_SCL_PIN, I2C_BUS1_FREQ);

  // Initialize WiFi and web server (only if configured)
  webServer.begin(WIFI_SSID, WIFI_PASSWORD, WEB_SERVER_PORT);

  // Initialize OTA updates (only if WiFi is connected)
  ota.begin("esp32-as5600", "esp32-as5600");

  // Set status LED based on sensor connection (after WiFi/OTA to avoid being overwritten)
  if (sensors.areBothConnected()) {
    led.setStatus(RgbLed::OK);
  } else if (sensors.isAnyConnected()) {
    led.setStatus(RgbLed::PARTIAL);
  } else {
    led.setStatus(RgbLed::ERROR);
  }

  LOG_INFOF("Sample rate: %d Hz (%d ms interval)", SAMPLE_RATE_HZ, SAMPLE_INTERVAL_MS);
  LOG_INFO("Starting main loop...");

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

  // Handle OTA updates
  ota.handle();

  // Small delay to prevent watchdog issues
  delay(1);
}

