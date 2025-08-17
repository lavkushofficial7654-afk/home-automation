/*
 * ESP8266 Guest Network + Ultrasonic Sensor (Safe Pins)
 * 
 * Uses safe pin configuration to avoid conflicts
 * TRIG: D1 (GPIO5), ECHO: D2 (GPIO4)
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi Configuration (same as working sketch)
const char* ssid = "Lav_Guest";
const char* password = "guest123";

// Pin Definitions (SAFE CONFIGURATION)
const int LED_PIN = 2;          // D4 (GPIO2) - LED control (same as working sketch)
const int TRIG_PIN = 5;         // D1 (GPIO5) - Ultrasonic trigger (SAFE)
const int ECHO_PIN = 4;         // D2 (GPIO4) - Ultrasonic echo (SAFE)

// Auto Control Settings
const float TRIGGER_DISTANCE = 10.0;  // Auto ON when distance < 10cm
const unsigned long AUTO_CHECK_INTERVAL = 2000; // Check every 2 seconds

// Global Variables
ESP8266WebServer server(80);
bool ledState = false;
bool manualOverride = false;  // True when user manually controls LED
unsigned long lastAutoCheck = 0;
bool sensorWorking = false;   // Track if sensor is working

void setup() {
  Serial.begin(9600);
  delay(3000);
  
  Serial.println("=====================================");
  Serial.println("ESP8266 Guest Network + Safe Pins");
  Serial.println("=====================================");
  
  // Initialize LED (same as working sketch)
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  ledState = false;
  manualOverride = false;
  
  // Initialize ultrasonic sensor pins SAFELY
  Serial.println("Initializing ultrasonic sensor with safe pins...");
  Serial.println("TRIG: D1 (GPIO5), ECHO: D2 (GPIO4)");
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  
  // Test sensor
  float testDistance = measureDistance();
  if (testDistance > 0) {
    sensorWorking = true;
    Serial.println("✅ Ultrasonic sensor working");
    Serial.print("Test distance: ");
    Serial.print(testDistance, 1);
    Serial.println(" cm");
  } else {
    sensorWorking = false;
    Serial.println("⚠️ Ultrasonic sensor not responding");
    Serial.println("Commands will work, auto control disabled");
  }
  
  // Connect to WiFi (same as working sketch)
  Serial.println("Connecting to Guest Network...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  const int maxAttempts = 30; // 15 seconds total
  
  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
    delay(500);
    Serial.print(".");
    attempts++;
    
    // Blink LED during connection attempt
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ Connected to Guest Network!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    
    // Turn LED ON to show connection
    digitalWrite(LED_PIN, HIGH);
    ledState = true;
  } else {
    Serial.println();
    Serial.println("❌ Failed to connect to Guest Network");
    Serial.println("Please check:");
    Serial.println("1. Guest network name and password");
    Serial.println("2. Guest network is enabled");
    Serial.println("3. ESP8266 is close to router");
    
    // Turn LED OFF to show no connection
    digitalWrite(LED_PIN, LOW);
    ledState = false;
  }
  
  // Setup web server routes (same as working sketch + sensor)
  server.on("/", handleRoot);
  server.on("/on", handleLedOn);
  server.on("/off", handleLedOff);
  server.on("/toggle", handleLedToggle);
  server.on("/distance", handleDistance);
  server.on("/status", handleStatus);
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("✅ Web server started!");
  
  Serial.println();
  Serial.println("=== SYSTEM READY ===");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  if (sensorWorking) {
    Serial.println("Auto ON when distance < " + String(TRIGGER_DISTANCE) + "cm");
  } else {
    Serial.println("Sensor not working - auto control disabled");
  }
  Serial.println("Manual on/off commands always work");
  Serial.println("====================");
}

void loop() {
  server.handleClient();
  
  // Auto control logic (only if sensor is working)
  if (sensorWorking && millis() - lastAutoCheck >= AUTO_CHECK_INTERVAL) {
    lastAutoCheck = millis();
    
    if (!manualOverride) {  // Only auto control if no manual override
      float distance = measureDistance();
      
      if (distance > 0 && distance < TRIGGER_DISTANCE) {
        if (!ledState) {
          digitalWrite(LED_PIN, HIGH);
          ledState = true;
          Serial.println("🔄 Auto ON: Object detected at " + String(distance, 1) + "cm");
        }
      }
      // Note: We don't auto OFF - only manual OFF works
    }
  }
  
  // Handle Serial commands (same as working sketch)
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    Serial.print("Command: ");
    Serial.println(command);
    
    if (command == "on") {
      digitalWrite(LED_PIN, HIGH);
      ledState = true;
      manualOverride = true;  // User manually controlled
      Serial.println("🔴 LED ON (Manual)");
    }
    else if (command == "off") {
      digitalWrite(LED_PIN, LOW);
      ledState = false;
      manualOverride = true;  // User manually controlled
      Serial.println("⚫ LED OFF (Manual)");
    }
    else if (command == "distance") {
      if (sensorWorking) {
        float distance = measureDistance();
        Serial.print("Distance: ");
        Serial.print(distance, 1);
        Serial.println(" cm");
      } else {
        Serial.println("❌ Sensor not working");
      }
    }
    else if (command == "status") {
      printStatus();
    }
    else if (command == "auto") {
      if (sensorWorking) {
        manualOverride = false;
        Serial.println("🔄 Auto mode enabled");
      } else {
        Serial.println("❌ Auto mode disabled - sensor not working");
      }
    }
    else if (command == "manual") {
      manualOverride = true;
      Serial.println("👆 Manual mode enabled");
    }
    else if (command == "sensor") {
      if (sensorWorking) {
        Serial.println("✅ Sensor working");
      } else {
        Serial.println("❌ Sensor not working");
      }
    }
    else if (command == "pins") {
      Serial.println("Current pin configuration:");
      Serial.println("LED: D4 (GPIO2)");
      Serial.println("TRIG: D1 (GPIO5)");
      Serial.println("ECHO: D2 (GPIO4)");
    }
    else if (command == "help") {
      Serial.println("Available commands:");
      Serial.println("  on/off - Control LED (always works)");
      Serial.println("  distance - Measure distance");
      Serial.println("  status - Show system status");
      Serial.println("  auto - Enable auto mode");
      Serial.println("  manual - Enable manual mode");
      Serial.println("  sensor - Check sensor status");
      Serial.println("  pins - Show pin configuration");
      Serial.println("  help - Show this help");
    }
  }
  
  delay(10);
}

// Ultrasonic distance measurement (with error handling)
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
  
  return distance;
}

// Web server handlers (based on working sketch)
void handleRoot() {
  float distance = 0.0;
  if (sensorWorking) {
    distance = measureDistance();
  }
  
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>ESP8266 LED Control</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; margin: 20px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; min-height: 100vh; }";
  html += ".container { max-width: 400px; margin: auto; background: rgba(255,255,255,0.95); padding: 30px; border-radius: 20px; box-shadow: 0 8px 32px rgba(0,0,0,0.3); color: #333; margin-top: 50px; }";
  html += ".btn { background: linear-gradient(45deg, #4CAF50, #45a049); color: white; padding: 20px 40px; border: none; border-radius: 15px; font-size: 18px; margin: 10px; cursor: pointer; text-decoration: none; display: inline-block; transition: all 0.3s; box-shadow: 0 4px 15px rgba(0,0,0,0.2); }";
  html += ".btn:hover { transform: translateY(-3px); box-shadow: 0 6px 20px rgba(0,0,0,0.3); }";
  html += ".btn-off { background: linear-gradient(45deg, #f44336, #da190b); }";
  html += ".status { background: linear-gradient(45deg, #e8f5e8, #d4edda); padding: 20px; border-radius: 15px; margin: 20px 0; font-size: 16px; color: #333; }";
  html += ".distance { background: linear-gradient(45deg, #e3f2fd, #bbdefb); color: #1565c0; padding: 15px; border-radius: 10px; margin: 15px 0; }";
  html += "h1 { color: #333; margin-bottom: 30px; font-size: 24px; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>💡 ESP8266 LED Control</h1>";
  
  html += "<div class='status'>";
  html += "<p><strong>LED Status:</strong> " + String(ledState ? "🔴 ON" : "⚫ OFF") + "</p>";
  html += "<p><strong>Mode:</strong> " + String(manualOverride ? "👆 Manual" : "🔄 Auto") + "</p>";
  html += "</div>";
  
  html += "<div class='distance'>";
  if (sensorWorking) {
    html += "<p><strong>Distance:</strong> " + String(distance, 1) + " cm</p>";
    html += "<p><strong>Auto ON:</strong> < " + String(TRIGGER_DISTANCE) + "cm</p>";
  } else {
    html += "<p><strong>Sensor:</strong> ❌ Not working</p>";
    html += "<p><strong>Auto Control:</strong> Disabled</p>";
  }
  html += "</div>";
  
  html += "<a href='/on' class='btn'>🔴 TURN ON</a><br>";
  html += "<a href='/off' class='btn btn-off'>⚫ TURN OFF</a><br>";
  html += "<a href='/toggle' class='btn'>🔄 TOGGLE</a><br>";
  html += "<a href='/distance' class='btn'>📏 MEASURE</a><br>";
  html += "<a href='/status' class='btn'>📊 STATUS</a>";
  
  html += "<br><br>";
  html += "<p style='color:#666; font-size:14px;'>🎮 LED Control via Guest Network</p>";
  html += "</div>";
  html += "<script>";
  html += "setTimeout(function(){location.reload();}, 10000);";
  html += "</script>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleLedOn() {
  digitalWrite(LED_PIN, HIGH);
  ledState = true;
  manualOverride = true;  // User manually controlled
  Serial.println("LED turned ON via web");
  server.send(200, "text/plain", "LED is now ON!");
}

void handleLedOff() {
  digitalWrite(LED_PIN, LOW);
  ledState = false;
  manualOverride = true;  // User manually controlled
  Serial.println("LED turned OFF via web");
  server.send(200, "text/plain", "LED is now OFF!");
}

void handleLedToggle() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  ledState = digitalRead(LED_PIN);
  manualOverride = true;  // User manually controlled
  Serial.println("LED toggled via web to: " + String(ledState ? "ON" : "OFF"));
  server.send(200, "text/plain", "LED " + String(ledState ? "ON" : "OFF"));
}

void handleDistance() {
  if (sensorWorking) {
    float distance = measureDistance();
    String response = "Distance: " + String(distance, 1) + " cm";
    server.send(200, "text/plain", response);
  } else {
    server.send(503, "text/plain", "Sensor not working");
  }
}

void handleStatus() {
  float distance = 0.0;
  if (sensorWorking) {
    distance = measureDistance();
  }
  
  String status = "LED: " + String(ledState ? "ON" : "OFF") + "\n";
  status += "Mode: " + String(manualOverride ? "Manual" : "Auto") + "\n";
  status += "Sensor: " + String(sensorWorking ? "Working" : "Not working") + "\n";
  if (sensorWorking) {
    status += "Distance: " + String(distance, 1) + " cm\n";
    status += "Auto ON: < " + String(TRIGGER_DISTANCE) + "cm\n";
  }
  status += "Network: " + String(ssid) + "\n";
  status += "IP: " + WiFi.localIP().toString() + "\n";
  status += "Signal Strength: " + String(WiFi.RSSI()) + " dBm";
  
  Serial.println("Status requested via web");
  server.send(200, "text/plain", status);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void printStatus() {
  float distance = 0.0;
  if (sensorWorking) {
    distance = measureDistance();
  }
  
  Serial.println();
  Serial.println("=== SYSTEM STATUS ===");
  Serial.print("LED: ");
  Serial.println(ledState ? "ON" : "OFF");
  Serial.print("Mode: ");
  Serial.println(manualOverride ? "Manual" : "Auto");
  Serial.print("Sensor: ");
  Serial.println(sensorWorking ? "Working" : "Not working");
  if (sensorWorking) {
    Serial.print("Distance: ");
    Serial.print(distance, 1);
    Serial.println(" cm");
    Serial.print("Auto ON: < ");
    Serial.print(TRIGGER_DISTANCE);
    Serial.println("cm");
  }
  Serial.print("WiFi: ");
  Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  }
  Serial.println("====================");
} 