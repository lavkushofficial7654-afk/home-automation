/*
 * ESP8266 Ultrasonic Distance Sensor Only
 * 
 * Features:
 * - Ultrasonic distance measurement using HC-SR04
 * - Web interface showing real-time distance
 * - Serial commands for testing
 * - No relay control (distance sensing only)
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi Configuration
const char* ssid = "Lav_Guest";
const char* password = "guest123";

// Pin Definitions - Fixed for ESP8266
const int TRIG_PIN = 5;         // D1 (GPIO5) - Ultrasonic trigger
const int ECHO_PIN = 0;         // D3 (GPIO0) - Ultrasonic echo

// Global Variables
ESP8266WebServer server(80);
float lastDistance = 0.0;

void setup() {
  Serial.begin(9600);
  delay(3000);
  
  Serial.println("=====================================");
  Serial.println("ESP8266 Ultrasonic Distance Sensor");
  Serial.println("=====================================");
  
  // Initialize pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Initialize trigger pin LOW
  digitalWrite(TRIG_PIN, LOW);
  
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
  server.on("/distance", handleDistance);
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
  Serial.println("Type 'distance' to measure, 'status' for info, 'help' for commands");
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
    
    if (command == "distance") {
      float distance = measureDistance();
      Serial.print("Distance: ");
      Serial.print(distance, 1);
      Serial.println(" cm");
    }
    else if (command == "status") {
      printStatus();
    }
    else if (command == "help") {
      Serial.println("Available commands:");
      Serial.println("  distance - Measure distance");
      Serial.println("  status - Show system status");
      Serial.println("  help - Show this help");
    }
    else {
      Serial.println("Unknown command. Type 'help' for available commands.");
    }
  }
  
  delay(100);
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
  
  // Measure echo time with timeout
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  
  // Calculate distance (cm)
  float distance = 0.0;
  if (duration > 0) {
    distance = duration * 0.034 / 2;
  }
  
  lastDistance = distance;
  return distance;
}

// Web server handlers
void handleRoot() {
  float distance = measureDistance();
  
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>ESP8266 Ultrasonic Distance</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; margin: 20px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; min-height: 100vh; }";
  html += ".container { max-width: 400px; margin: auto; background: rgba(255,255,255,0.95); padding: 30px; border-radius: 20px; box-shadow: 0 8px 32px rgba(0,0,0,0.3); color: #333; margin-top: 50px; }";
  html += ".btn { background: linear-gradient(45deg, #4CAF50, #45a049); color: white; padding: 15px 30px; border: none; border-radius: 10px; font-size: 16px; margin: 10px; cursor: pointer; text-decoration: none; display: inline-block; transition: all 0.3s; }";
  html += ".btn:hover { transform: translateY(-2px); box-shadow: 0 4px 15px rgba(0,0,0,0.2); }";
  html += ".distance { background: linear-gradient(45deg, #e3f2fd, #bbdefb); color: #1565c0; padding: 20px; border-radius: 15px; margin: 20px 0; font-size: 18px; }";
  html += "h1 { color: #333; margin-bottom: 30px; font-size: 24px; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>📏 Ultrasonic Distance Sensor</h1>";
  
  html += "<div class='distance'>";
  if (distance > 0) {
    html += "<p><strong>Distance:</strong> " + String(distance, 1) + " cm</p>";
  } else {
    html += "<p><strong>Distance:</strong> No object detected</p>";
  }
  html += "</div>";
  
  html += "<a href='/distance' class='btn'>📏 MEASURE</a><br>";
  html += "<a href='/status' class='btn'>📊 STATUS</a>";
  
  html += "<br><br>";
  html += "<p style='color:#666; font-size:14px;'>📏 Distance Sensing Only</p>";
  html += "</div>";
  html += "<script>";
  html += "setTimeout(function(){location.reload();}, 3000);";
  html += "</script>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleDistance() {
  float distance = measureDistance();
  String response;
  if (distance > 0) {
    response = "Distance: " + String(distance, 1) + " cm";
  } else {
    response = "Distance: No object detected";
  }
  server.send(200, "text/plain", response);
}

void handleStatus() {
  float distance = measureDistance();
  String status = "Distance: ";
  if (distance > 0) {
    status += String(distance, 1) + " cm\n";
  } else {
    status += "No object detected\n";
  }
  status += "WiFi: " + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected") + "\n";
  if (WiFi.status() == WL_CONNECTED) {
    status += "IP: " + WiFi.localIP().toString();
  }
  
  server.send(200, "text/plain", status);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void printStatus() {
  float distance = measureDistance();
  Serial.println();
  Serial.println("=== SYSTEM STATUS ===");
  Serial.print("Distance: ");
  if (distance > 0) {
    Serial.print(distance, 1);
    Serial.println(" cm");
  } else {
    Serial.println("No object detected");
  }
  Serial.print("WiFi: ");
  Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
  Serial.println("====================");
} 