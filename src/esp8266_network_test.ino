/*
 * ESP8266 Network Test + Basic Commands
 * 
 * This sketch tries multiple network names and provides basic commands
 * that work even without WiFi connection
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Try multiple network names
const char* networks[][2] = {
  {"Lav_Guest", "guest123"},
  {"Lav_home", "guest123"},
  {"Lav", "guest123"}
};
const int numNetworks = 3;

// Pin Definitions
const int LED_PIN = 2;          // D4 (GPIO2) - LED control
const int TRIG_PIN = 5;         // D1 (GPIO5) - Ultrasonic trigger
const int ECHO_PIN = 0;         // D3 (GPIO0) - Ultrasonic echo

// Global Variables
ESP8266WebServer server(80);
bool ledState = false;
bool wifiConnected = false;

void setup() {
  Serial.begin(9600);
  delay(3000);
  
  Serial.println("=====================================");
  Serial.println("ESP8266 Network Test + Commands");
  Serial.println("=====================================");
  
  // Initialize pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Initialize LED OFF
  digitalWrite(LED_PIN, LOW);
  ledState = false;
  
  // Initialize trigger pin LOW
  digitalWrite(TRIG_PIN, LOW);
  
  // Try to connect to networks
  Serial.println("Trying to connect to networks...");
  wifiConnected = false;
  
  for (int i = 0; i < numNetworks && !wifiConnected; i++) {
    Serial.print("Trying network ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(networks[i][0]);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(networks[i][0], networks[i][1]);
    
    int attempts = 0;
    const int maxAttempts = 20; // 10 seconds per network
    
    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
      delay(500);
      Serial.print(".");
      attempts++;
      
      // Blink LED during connection attempt
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("✅ Connected to network: " + String(networks[i][0]));
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      Serial.print("Signal Strength (RSSI): ");
      Serial.print(WiFi.RSSI());
      Serial.println(" dBm");
      
      wifiConnected = true;
      
      // Turn LED ON to show connection
      digitalWrite(LED_PIN, HIGH);
      ledState = true;
    } else {
      Serial.println();
      Serial.println("❌ Failed to connect to: " + String(networks[i][0]));
      WiFi.disconnect();
    }
  }
  
  if (!wifiConnected) {
    Serial.println("❌ Failed to connect to any network");
    Serial.println("Commands will still work without WiFi");
    
    // Turn LED OFF to show no connection
    digitalWrite(LED_PIN, LOW);
    ledState = false;
  }
  
  // Setup web server (only if WiFi connected)
  if (wifiConnected) {
    server.on("/", handleRoot);
    server.on("/on", handleLedOn);
    server.on("/off", handleLedOff);
    server.on("/toggle", handleLedToggle);
    server.on("/distance", handleDistance);
    server.on("/status", handleStatus);
    server.onNotFound(handleNotFound);
    
    server.begin();
    Serial.println("✅ Web server started!");
  }
  
  Serial.println();
  Serial.println("=== SYSTEM READY ===");
  if (wifiConnected) {
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Web interface available");
  } else {
    Serial.println("No WiFi connection");
    Serial.println("Commands work via Serial Monitor only");
  }
  Serial.println("Type 'help' for available commands");
  Serial.println("====================");
}

void loop() {
  if (wifiConnected) {
    server.handleClient();
  }
  
  // Handle Serial commands (always work)
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    Serial.print("Command: ");
    Serial.println(command);
    
    if (command == "on") {
      digitalWrite(LED_PIN, HIGH);
      ledState = true;
      Serial.println("🔴 LED ON");
    }
    else if (command == "off") {
      digitalWrite(LED_PIN, LOW);
      ledState = false;
      Serial.println("⚫ LED OFF");
    }
    else if (command == "distance") {
      float distance = measureDistance();
      Serial.print("Distance: ");
      Serial.print(distance, 1);
      Serial.println(" cm");
    }
    else if (command == "status") {
      printStatus();
    }
    else if (command == "wifi") {
      if (wifiConnected) {
        Serial.println("✅ WiFi Connected");
        Serial.print("Network: ");
        Serial.println(WiFi.SSID());
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("Signal: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
      } else {
        Serial.println("❌ WiFi Not Connected");
      }
    }
    else if (command == "help") {
      Serial.println("Available commands:");
      Serial.println("  on/off - Control LED");
      Serial.println("  distance - Measure distance");
      Serial.println("  status - Show system status");
      Serial.println("  wifi - Show WiFi status");
      Serial.println("  help - Show this help");
    }
    else {
      Serial.println("Unknown command. Type 'help' for available commands.");
    }
  }
  
  delay(10);
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
  
  return distance;
}

// Web server handlers (only if WiFi connected)
void handleRoot() {
  if (!wifiConnected) {
    server.send(503, "text/plain", "WiFi not connected");
    return;
  }
  
  float distance = measureDistance();
  
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>ESP8266 Test</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; margin: 20px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; min-height: 100vh; }";
  html += ".container { max-width: 400px; margin: auto; background: rgba(255,255,255,0.95); padding: 30px; border-radius: 20px; box-shadow: 0 8px 32px rgba(0,0,0,0.3); color: #333; margin-top: 50px; }";
  html += ".btn { background: linear-gradient(45deg, #4CAF50, #45a049); color: white; padding: 20px 40px; border: none; border-radius: 15px; font-size: 18px; margin: 10px; cursor: pointer; text-decoration: none; display: inline-block; transition: all 0.3s; }";
  html += ".btn:hover { transform: translateY(-3px); box-shadow: 0 6px 20px rgba(0,0,0,0.3); }";
  html += ".btn-off { background: linear-gradient(45deg, #f44336, #da190b); }";
  html += ".status { background: linear-gradient(45deg, #e8f5e8, #d4edda); padding: 20px; border-radius: 15px; margin: 20px 0; font-size: 16px; color: #333; }";
  html += "h1 { color: #333; margin-bottom: 30px; font-size: 24px; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>💡 ESP8266 Test</h1>";
  
  html += "<div class='status'>";
  html += "<p><strong>LED Status:</strong> " + String(ledState ? "🔴 ON" : "⚫ OFF") + "</p>";
  html += "<p><strong>Distance:</strong> " + String(distance, 1) + " cm</p>";
  html += "</div>";
  
  html += "<a href='/on' class='btn'>🔴 TURN ON</a><br>";
  html += "<a href='/off' class='btn btn-off'>⚫ TURN OFF</a><br>";
  html += "<a href='/toggle' class='btn'>🔄 TOGGLE</a><br>";
  html += "<a href='/distance' class='btn'>📏 MEASURE</a><br>";
  html += "<a href='/status' class='btn'>📊 STATUS</a>";
  
  html += "<br><br>";
  html += "<p style='color:#666; font-size:14px;'>🎮 LED Control + 📏 Distance</p>";
  html += "</div>";
  html += "<script>";
  html += "setTimeout(function(){location.reload();}, 5000);";
  html += "</script>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleLedOn() {
  digitalWrite(LED_PIN, HIGH);
  ledState = true;
  Serial.println("LED turned ON via web");
  server.send(200, "text/plain", "LED is now ON!");
}

void handleLedOff() {
  digitalWrite(LED_PIN, LOW);
  ledState = false;
  Serial.println("LED turned OFF via web");
  server.send(200, "text/plain", "LED is now OFF!");
}

void handleLedToggle() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  ledState = digitalRead(LED_PIN);
  Serial.println("LED toggled via web to: " + String(ledState ? "ON" : "OFF"));
  server.send(200, "text/plain", "LED " + String(ledState ? "ON" : "OFF"));
}

void handleDistance() {
  float distance = measureDistance();
  String response = "Distance: " + String(distance, 1) + " cm";
  server.send(200, "text/plain", response);
}

void handleStatus() {
  float distance = measureDistance();
  String status = "LED: " + String(ledState ? "ON" : "OFF") + "\n";
  status += "Distance: " + String(distance, 1) + " cm\n";
  status += "WiFi: " + String(wifiConnected ? "Connected" : "Disconnected") + "\n";
  if (wifiConnected) {
    status += "Network: " + WiFi.SSID() + "\n";
    status += "IP: " + WiFi.localIP().toString() + "\n";
    status += "Signal Strength: " + String(WiFi.RSSI()) + " dBm";
  }
  
  Serial.println("Status requested via web");
  server.send(200, "text/plain", status);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void printStatus() {
  float distance = measureDistance();
  Serial.println();
  Serial.println("=== SYSTEM STATUS ===");
  Serial.print("LED: ");
  Serial.println(ledState ? "ON" : "OFF");
  Serial.print("Distance: ");
  Serial.print(distance, 1);
  Serial.println(" cm");
  Serial.print("WiFi: ");
  Serial.println(wifiConnected ? "Connected" : "Disconnected");
  if (wifiConnected) {
    Serial.print("Network: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  }
  Serial.println("====================");
} 