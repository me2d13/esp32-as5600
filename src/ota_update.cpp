#include "ota_update.h"
#include "config.h"
#include "logger.h"
#include "rgb_led.h"

// External RGB LED reference for status indication during OTA
extern RgbLed led;

OTAUpdate::OTAUpdate() : otaEnabled(false) {}

void OTAUpdate::begin(const char* hostname, const char* password) {
  // Only enable OTA if WiFi is connected
  if (WiFi.status() != WL_CONNECTED) {
    LOG_INFO("OTA: WiFi not connected, OTA disabled");
    otaEnabled = false;
    return;
  }

  LOG_INFO("=== OTA Configuration ===");
  LOG_INFOF("Hostname: %s", hostname);
  LOG_INFO("Setting up OTA updates...");

  // Configure OTA
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.setPassword(password);

  // Set OTA callbacks
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    
    LOG_INFOF("OTA: Start updating %s", type.c_str());
    
    // Turn LED purple during OTA update
    led.setColor(128, 0, 128);  // Purple
  });

  ArduinoOTA.onEnd([]() {
    LOG_INFO("OTA: Update complete!");
    
    // Turn LED white on completion
    led.setColor(255, 255, 255);  // White
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    LOG_DEBUGF("OTA Progress: %u%%", (progress / (total / 100)));
    
    // Blink LED during progress
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 200) {
      static bool ledState = false;
      if (ledState) {
        led.setColor(128, 0, 128);  // Purple
      } else {
        led.setColor(0, 0, 0);      // Off
      }
      ledState = !ledState;
      lastBlink = millis();
    }
  });

  ArduinoOTA.onError([](ota_error_t error) {
    const char* errorMsg = "Unknown";
    if (error == OTA_AUTH_ERROR) {
      errorMsg = "Auth Failed";
    } else if (error == OTA_BEGIN_ERROR) {
      errorMsg = "Begin Failed";
    } else if (error == OTA_CONNECT_ERROR) {
      errorMsg = "Connect Failed";
    } else if (error == OTA_RECEIVE_ERROR) {
      errorMsg = "Receive Failed";
    } else if (error == OTA_END_ERROR) {
      errorMsg = "End Failed";
    }
    LOG_ERRORF("OTA Error[%u]: %s", error, errorMsg);
    
    // Turn LED red on error briefly, then restore to normal status
    led.setColor(255, 0, 0);  // Red
    delay(2000);
    // Restore to green (assuming sensors are OK if we got this far)
    led.setStatus(RgbLed::OK);
  });

  // Start OTA service
  ArduinoOTA.begin();
  otaEnabled = true;

  LOG_INFO("OTA: Ready for updates");
  LOG_INFOF("OTA: IP address: %s", WiFi.localIP().toString().c_str());
  LOG_INFO("==========================");
}

void OTAUpdate::handle() {
  if (otaEnabled) {
    ArduinoOTA.handle();
  }
}
