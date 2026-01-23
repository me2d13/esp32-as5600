#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

class OTAUpdate {
public:
  OTAUpdate();
  
  // Initialize OTA (only if WiFi is connected)
  void begin(const char* hostname = "esp32-as5600", const char* password = "esp32-as5600");
  
  // Handle OTA updates (call this in loop)
  void handle();
  
  // Check if OTA is enabled
  bool isEnabled() const { return otaEnabled; }

private:
  bool otaEnabled;
  
  // OTA callbacks
  static void onStart();
  static void onEnd();
  static void onProgress(unsigned int progress, unsigned int total);
  static void onError(ota_error_t error);
};

#endif // OTA_UPDATE_H
