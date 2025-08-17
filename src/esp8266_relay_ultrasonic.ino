/*
 * ESP8266 Relay + Ultrasonic Distance Sensor
 * 
 * Features:
 * - Relay control for LED/bulb (ON/OFF)
 * - Ultrasonic distance measurement
 * - Web interface for both controls
 * - Serial commands for testing
 * - Automatic light control based on distance
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi Configuration
const char* ssid = "Lav_Guest";
const char* password = "guest123";

// Pin Definitions
const int RELAY_PIN = 4;        // D4 - Controls relay
const int TRIG_PIN = 5;         // D1 - Ultrasonic trigger
const int ECHO_PIN = 2;         // D4 - Ultrasonic echo (shared with relay)

// Global Variables
ESP8266WebServer server(80);
bool relayState = false;
bool autoMode = false;
int autoDistance = 50;          // Auto turn on when object closer than 50cm

void setup() {
  Serial.begin(9600);
  delay(3000);
  
  Serial.println("=====================================");
  Serial.println("ESP8266 Relay + Ultrasonic Sensor");
  Serial.println("=====================================");
  
  // Initialize pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Initialize relay OFF
  digitalWrite(RELAY_PIN, LOW);
  relayState = false;
  
  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ Connected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("❌ Failed to connect to WiFi");
  }
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/on", handleRelayOn);
  server.on("/off", handleRelayOff);
  server.on("/toggle", handleRelayToggle);
  server.on("/distance", handleDistance);
  server.on("/auto", handleAutoMode);
  server.on("/status", handleStatus);
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("✅ Web server started!");
  
  Serial.println();
  Serial.println("=== SYSTEM READY ===");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Web Interface: http://" + WiFi.localIP().toString());
  Serial.println("====================");
}

void loop() {
  server.handleClient();
  
  // Handle serial commands
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    Serial.print("Command: ");
    Serial.println(command);
    
    if (command == "on") {
      turnRelayOn();
    }
    else if (command == "off") {
      turnRelayOff();
    }
    else if (command == "distance") {
      float distance = measureDistance();
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" cm");
    }
    else if (command == "auto on") {
      autoMode = true;
      Serial.println("Auto mode: ON");
    }
    else if (command == "auto off") {
      autoMode = false;
      Serial.println("Auto mode: OFF");
    }
    else if (command == "status") {
      printStatus();
    }
    else if (command == "help") {
      Serial.println("Available commands:");
      Serial.println("  on/off - Control relay");
      Serial.println("  distance - Measure distance");
      Serial.println("  auto on/off - Enable/disable auto mode");
      Serial.println("  status - Show system status");
      Serial.println("  help - Show this help");
    }
  }
  
  // Auto mode logic
  if (autoMode) {
    float distance = measureDistance();
    if (distance < autoDistance && distance > 0) {
      if (!relayState) {
        turnRelayOn();
        Serial.println("Auto: Object detected, turning ON");
      }
    } else {
      if (relayState) {
        turnRelayOff();
        Serial.println("Auto: No object, turning OFF");
      }
    }
    delay(1000); // Check every second
  }
  
  delay(10);
}

// Relay control functions
void turnRelayOn() {
  digitalWrite(RELAY_PIN, HIGH);
  relayState = true;
  Serial.println("Relay: ON");
}

void turnRelayOff() {
  digitalWrite(RELAY_PIN, LOW);
  relayState = false;
  Serial.println("Relay: OFF");
}

// Ultrasonic distance measurement
float measureDistance() {
  // Clear trigger
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Send trigger pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measure echo time
  long duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate distance (cm)
  float distance = duration * 0.034 / 2;
  
  return distance;
}

// Web server handlers
void handleRoot() {
  float distance = measureDistance();
  
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>ESP8266 Relay + Ultrasonic</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; margin: 20px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; min-height: 100vh; }";
  html += ".container { max-width: 500px; margin: auto; background: rgba(255,255,255,0.95); padding: 30px; border-radius: 20px; box-shadow: 0 8px 32px rgba(0,0,0,0.3); color: #333; margin-top: 50px; }";
  html += ".btn { background: linear-gradient(45deg, #4CAF50, #45a049); color: white; padding: 15px 30px; border: none; border-radius: 10px; font-size: 16px; margin: 10px; cursor: pointer; text-decoration: none; display: inline-block; transition: all 0.3s; }";
  html += ".btn:hover { transform: translateY(-2px); box-shadow: 0 4px 15px rgba(0,0,0,0.2); }";
  html += ".btn-off { background: linear-gradient(45deg, #f44336, #da190b); }";
  html += ".status { background: linear-gradient(45deg, #e8f5e8, #d4edda); padding: 20px; border-radius: 15px; margin: 20px 0; font-size: 16px; color: #333; }";
  html += ".distance { background: linear-gradient(45deg, #e3f2fd, #bbdefb); color: #1565c0; padding: 15px; border-radius: 10px; margin: 15px 0; }";
  html += "h1 { color: #333; margin-bottom: 30px; font-size: 24px; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>🔌 Relay + 📏 Ultrasonic Control</h1>";
  
  html += "<div class='status'>";
  html += "<p><strong>Relay Status:</strong> " + String(relayState ? "🔴 ON" : "⚫ OFF") + "</p>";
  html += "<p><strong>Auto Mode:</strong> " + String(autoMode ? "🟢 ON" : "🔴 OFF") + "</p>";
  html += "</div>";
  
  html += "<div class='distance'>";
  html += "<p><strong>Distance:</strong> " + String(distance, 1) + " cm</p>";
  html += "</div>";
  
  html += "<a href='/on' class='btn'>🔴 TURN ON</a><br>";
  html += "<a href='/off' class='btn btn-off'>⚫ TURN OFF</a><br>";
  html += "<a href='/toggle' class='btn'>🔄 TOGGLE</a><br>";
  html += "<a href='/distance' class='btn'>📏 MEASURE</a><br>";
  html += "<a href='/auto' class='btn'>🤖 AUTO MODE</a><br>";
  html += "<a href='/status' class='btn'>📊 STATUS</a>";
  
  html += "<br><br>";
  html += "<p style='color:#666; font-size:14px;'>🎮 Relay Control + 📏 Distance Sensing</p>";
  html += "</div>";
  html += "<script>";
  html += "setTimeout(function(){location.reload();}, 5000);";
  html += "</script>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleRelayOn() {
  turnRelayOn();
  server.send(200, "text/plain", "Relay turned ON!");
}

void handleRelayOff() {
  turnRelayOff();
  server.send(200, "text/plain", "Relay turned OFF!");
}

void handleRelayToggle() {
  if (relayState) {
    turnRelayOff();
  } else {
    turnRelayOn();
  }
  server.send(200, "text/plain", "Relay " + String(relayState ? "ON" : "OFF"));
}

void handleDistance() {
  float distance = measureDistance();
  String response = "Distance: " + String(distance, 1) + " cm";
  server.send(200, "text/plain", response);
}

void handleAutoMode() {
  autoMode = !autoMode;
  String response = "Auto mode: " + String(autoMode ? "ON" : "OFF");
  server.send(200, "text/plain", response);
}

void handleStatus() {
  float distance = measureDistance();
  String status = "Relay: " + String(relayState ? "ON" : "OFF") + "\n";
  status += "Auto Mode: " + String(autoMode ? "ON" : "OFF") + "\n";
  status += "Distance: " + String(distance, 1) + " cm\n";
  status += "IP: " + WiFi.localIP().toString();
  
  server.send(200, "text/plain", status);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void printStatus() {
  float distance = measureDistance();
  Serial.println();
  Serial.println("=== SYSTEM STATUS ===");
  Serial.print("Relay: ");
  Serial.println(relayState ? "ON" : "OFF");
  Serial.print("Auto Mode: ");
  Serial.println(autoMode ? "ON" : "OFF");
  Serial.print("Distance: ");
  Serial.print(distance, 1);
  Serial.println(" cm");
  Serial.print("WiFi: ");
  Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
  Serial.println("====================");
} 