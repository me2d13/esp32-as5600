#include "web_server.h"
#include "config.h"
#include <ArduinoJson.h>

// Static instance pointer
WebServerManager* WebServerManager::instance = nullptr;

// HTML page with WebSocket support
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AS5600 Sensor Monitor</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        
        .container {
            background: rgba(255, 255, 255, 0.95);
            border-radius: 20px;
            padding: 40px;
            box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
            max-width: 800px;
            width: 100%;
            backdrop-filter: blur(10px);
        }
        
        h1 {
            color: #333;
            text-align: center;
            margin-bottom: 10px;
            font-size: 2.5em;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
        }
        
        .subtitle {
            text-align: center;
            color: #666;
            margin-bottom: 40px;
            font-size: 1.1em;
        }
        
        .status {
            text-align: center;
            padding: 12px;
            border-radius: 10px;
            margin-bottom: 30px;
            font-weight: 600;
            transition: all 0.3s ease;
        }
        
        .status.connected {
            background: #d4edda;
            color: #155724;
            border: 2px solid #c3e6cb;
        }
        
        .status.disconnected {
            background: #f8d7da;
            color: #721c24;
            border: 2px solid #f5c6cb;
        }
        
        .sensors {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 30px;
            margin-bottom: 30px;
        }
        
        .sensor-card {
            background: linear-gradient(135deg, #f5f7fa 0%, #c3cfe2 100%);
            border-radius: 15px;
            padding: 30px;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.1);
            transition: transform 0.3s ease, box-shadow 0.3s ease;
        }
        
        .sensor-card:hover {
            transform: translateY(-5px);
            box-shadow: 0 15px 40px rgba(0, 0, 0, 0.2);
        }
        
        .sensor-card h2 {
            color: #667eea;
            margin-bottom: 20px;
            font-size: 1.5em;
            text-align: center;
        }
        
        .angle-display {
            text-align: center;
            margin-bottom: 20px;
        }
        
        .angle-value {
            font-size: 3.5em;
            font-weight: bold;
            color: #333;
            display: block;
            margin-bottom: 5px;
        }
        
        .angle-unit {
            font-size: 1.2em;
            color: #666;
        }
        
        .angle-bar {
            width: 100%;
            height: 30px;
            background: #e0e0e0;
            border-radius: 15px;
            overflow: hidden;
            position: relative;
        }
        
        .angle-fill {
            height: 100%;
            background: linear-gradient(90deg, #667eea 0%, #764ba2 100%);
            transition: width 0.3s ease;
            border-radius: 15px;
        }
        
        .raw-value {
            text-align: center;
            margin-top: 15px;
            color: #666;
            font-size: 0.9em;
        }
        
        .info {
            background: #e7f3ff;
            border-left: 4px solid #2196F3;
            padding: 15px;
            border-radius: 5px;
            margin-top: 20px;
        }
        
        .info p {
            color: #0c5460;
            margin: 5px 0;
            font-size: 0.95em;
        }
        
        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.5; }
        }
        
        .updating {
            animation: pulse 1s ease-in-out infinite;
        }
        
        @media (max-width: 768px) {
            .container {
                padding: 20px;
            }
            
            h1 {
                font-size: 2em;
            }
            
            .angle-value {
                font-size: 2.5em;
            }
            
            .sensors {
                grid-template-columns: 1fr;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🧭 AS5600 Sensor Monitor</h1>
        <p class="subtitle">Real-time Magnetic Encoder Readings</p>
        
        <div id="status" class="status disconnected">
            ⚠️ Connecting to WebSocket...
        </div>
        
        <div class="sensors">
            <div class="sensor-card">
                <h2>Sensor 1</h2>
                <div class="angle-display">
                    <span id="angle1" class="angle-value">---</span>
                    <span class="angle-unit">degrees</span>
                </div>
                <div class="angle-bar">
                    <div id="bar1" class="angle-fill" style="width: 0%"></div>
                </div>
                <div class="raw-value">
                    Raw: <span id="raw1">---</span> / 4095
                </div>
            </div>
            
            <div class="sensor-card">
                <h2>Sensor 2</h2>
                <div class="angle-display">
                    <span id="angle2" class="angle-value">---</span>
                    <span class="angle-unit">degrees</span>
                </div>
                <div class="angle-bar">
                    <div id="bar2" class="angle-fill" style="width: 0%"></div>
                </div>
                <div class="raw-value">
                    Raw: <span id="raw2">---</span> / 4095
                </div>
            </div>
        </div>
        
        <div class="info">
            <p><strong>ℹ️ Information:</strong></p>
            <p>• AS5600 sensors provide 12-bit resolution (0-4095)</p>
            <p>• Values are updated in real-time via WebSocket</p>
            <p>• Angle range: 0° - 360°</p>
        </div>
    </div>
    
    <script>
        let ws;
        let reconnectInterval;
        
        function connectWebSocket() {
            const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
            const wsUrl = `${protocol}//${window.location.hostname}:81/`;
            
            ws = new WebSocket(wsUrl);
            
            ws.onopen = function() {
                console.log('WebSocket connected');
                document.getElementById('status').className = 'status connected';
                document.getElementById('status').innerHTML = '✅ Connected';
                clearInterval(reconnectInterval);
            };
            
            ws.onmessage = function(event) {
                try {
                    const data = JSON.parse(event.data);
                    updateSensor(1, data.angle1, data.raw1);
                    updateSensor(2, data.angle2, data.raw2);
                } catch (e) {
                    console.error('Error parsing WebSocket data:', e);
                }
            };
            
            ws.onclose = function() {
                console.log('WebSocket disconnected');
                document.getElementById('status').className = 'status disconnected';
                document.getElementById('status').innerHTML = '⚠️ Disconnected - Reconnecting...';
                
                // Try to reconnect every 3 seconds
                reconnectInterval = setInterval(connectWebSocket, 3000);
            };
            
            ws.onerror = function(error) {
                console.error('WebSocket error:', error);
            };
        }
        
        function updateSensor(sensorNum, angle, raw) {
            const angleElement = document.getElementById(`angle${sensorNum}`);
            const rawElement = document.getElementById(`raw${sensorNum}`);
            const barElement = document.getElementById(`bar${sensorNum}`);
            
            // Convert raw value (0-4095) to degrees (0-360)
            const degrees = ((raw / 4095) * 360).toFixed(1);
            const percentage = (raw / 4095 * 100).toFixed(1);
            
            angleElement.textContent = degrees;
            rawElement.textContent = raw;
            barElement.style.width = percentage + '%';
            
            // Add pulse animation on update
            angleElement.classList.add('updating');
            setTimeout(() => angleElement.classList.remove('updating'), 300);
        }
        
        // Connect on page load
        connectWebSocket();
    </script>
</body>
</html>
)rawliteral";

WebServerManager::WebServerManager() 
  : wifiEnabled(false), 
    serverPort(80), 
    lastBroadcast(0),
    currentAngle1(0), 
    currentAngle2(0),
    server(nullptr),
    webSocket(nullptr) {
  instance = this;
}

bool WebServerManager::begin(const char* ssid, const char* password, uint16_t port) {
  // Check if WiFi should be enabled (SSID must be non-empty)
  if (ssid == nullptr || strlen(ssid) == 0) {
    #if DEBUG_ENABLED
      Serial.println("WiFi disabled (SSID not configured)");
    #endif
    wifiEnabled = false;
    return false;
  }
  
  serverPort = port;
  
  #if DEBUG_ENABLED
    Serial.println("\n=== WiFi Configuration ===");
    Serial.print("SSID: ");
    Serial.println(ssid);
  #endif
  
  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  #if DEBUG_ENABLED
    Serial.print("Connecting to WiFi");
  #endif
  
  // Wait for connection (timeout after 20 seconds)
  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 20000) {
    delay(500);
    #if DEBUG_ENABLED
      Serial.print(".");
    #endif
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    #if DEBUG_ENABLED
      Serial.println("\nFailed to connect to WiFi");
    #endif
    wifiEnabled = false;
    return false;
  }
  
  #if DEBUG_ENABLED
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  #endif
  
  // Initialize web server
  server = new WebServer(serverPort);
  
  // Set up routes
  server->on("/", []() {
    instance->server->send_P(200, "text/html", INDEX_HTML);
  });
  
  server->onNotFound([]() {
    instance->server->send(404, "text/plain", "404: Not Found");
  });
  
  server->begin();
  
  #if DEBUG_ENABLED
    Serial.print("Web server started on port ");
    Serial.println(serverPort);
  #endif
  
  // Initialize WebSocket server on port 81
  webSocket = new WebSocketsServer(81);
  webSocket->begin();
  webSocket->onEvent([](uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    if (instance) {
      instance->webSocketEvent(num, type, payload, length);
    }
  });
  
  #if DEBUG_ENABLED
    Serial.println("WebSocket server started on port 81");
    Serial.println("==========================\n");
  #endif
  
  wifiEnabled = true;
  return true;
}

String WebServerManager::getIPAddress() const {
  if (!wifiEnabled || WiFi.status() != WL_CONNECTED) {
    return "Not connected";
  }
  return WiFi.localIP().toString();
}

void WebServerManager::handleClient() {
  if (!wifiEnabled) return;
  
  if (server) {
    server->handleClient();
  }
  
  if (webSocket) {
    webSocket->loop();
  }
}

void WebServerManager::updateSensorData(uint16_t angle1, uint16_t angle2) {
  currentAngle1 = angle1;
  currentAngle2 = angle2;
}

void WebServerManager::broadcastSensorData() {
  if (!wifiEnabled || !webSocket) return;
  
  // Check if it's time to broadcast
  unsigned long currentTime = millis();
  if (currentTime - lastBroadcast < WEBSOCKET_UPDATE_MS) {
    return;
  }
  
  lastBroadcast = currentTime;
  
  // Create JSON document
  StaticJsonDocument<128> doc;
  doc["angle1"] = ((float)currentAngle1 / 4095.0f) * 360.0f;
  doc["angle2"] = ((float)currentAngle2 / 4095.0f) * 360.0f;
  doc["raw1"] = currentAngle1;
  doc["raw2"] = currentAngle2;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  // Broadcast to all connected clients
  webSocket->broadcastTXT(jsonString);
}

void WebServerManager::webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      #if DEBUG_ENABLED
        Serial.printf("WebSocket [%u] disconnected\n", num);
      #endif
      break;
      
    case WStype_CONNECTED:
      {
        #if DEBUG_ENABLED
          IPAddress ip = webSocket->remoteIP(num);
          Serial.printf("WebSocket [%u] connected from %d.%d.%d.%d\n", 
                       num, ip[0], ip[1], ip[2], ip[3]);
        #endif
        
        // Send initial data to newly connected client
        StaticJsonDocument<128> doc;
        doc["angle1"] = ((float)currentAngle1 / 4095.0f) * 360.0f;
        doc["angle2"] = ((float)currentAngle2 / 4095.0f) * 360.0f;
        doc["raw1"] = currentAngle1;
        doc["raw2"] = currentAngle2;
        
        String jsonString;
        serializeJson(doc, jsonString);
        webSocket->sendTXT(num, jsonString);
      }
      break;
      
    case WStype_TEXT:
      #if DEBUG_ENABLED
        Serial.printf("WebSocket [%u] received text: %s\n", num, payload);
      #endif
      break;
      
    default:
      break;
  }
}
