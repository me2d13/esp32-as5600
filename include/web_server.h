#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

class WebServerManager {
public:
  WebServerManager();
  
  // Initialize WiFi and web server (only if WiFi credentials are configured)
  bool begin(const char* ssid, const char* password, uint16_t port);
  
  // Check if WiFi is enabled and connected
  bool isEnabled() const { return wifiEnabled; }
  bool isConnected() const { return wifiEnabled && WiFi.status() == WL_CONNECTED; }
  
  // Get IP address
  String getIPAddress() const;
  
  // Handle client requests (call this in loop)
  void handleClient();
  
  // Update sensor data for WebSocket broadcast
  void updateSensorData(uint16_t angle1, uint16_t angle2);
  
  // Send sensor data via WebSocket
  void broadcastSensorData();

private:
  bool wifiEnabled;
  uint16_t serverPort;
  unsigned long lastBroadcast;
  
  // Current sensor values
  uint16_t currentAngle1;
  uint16_t currentAngle2;
  
  WebServer* server;
  WebSocketsServer* webSocket;
  
  // WebSocket event handler
  void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length);
  
  // Static instance pointer for callbacks
  static WebServerManager* instance;
};

#endif // WEB_SERVER_H
