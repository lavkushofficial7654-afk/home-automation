/*
 * Simple ESP8266 WiFi LED Control
 * 
 * This version connects to your main WiFi network "Lav"
 * so it can be accessed from devices on the same network
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Your home WiFi credentials
const char* ssid = "Lav";
const char* password = "harharmahadev@468";

// Pin Definition
const int LED_PIN = 2;

// Global Variables
ESP8266WebServer server(80);
bool ledState = false;

void setup() {
  Serial.begin(9600);
  delay(3000);
  
  Serial.println("=====================================");
  Serial.println("ESP8266 WiFi LED Control");
  Serial.println("=====================================");
  
  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
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
    Serial.println("✅ Connected to WiFi!");
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
    Serial.println("❌ Failed to connect to WiFi");
    Serial.println("Possible issues:");
    Serial.println("1. Wrong SSID or password");
    Serial.println("2. WiFi signal too weak");
    Serial.println("3. Router not broadcasting");
    Serial.println("4. Too many devices connected");
    
    // Turn LED OFF to show no connection
    digitalWrite(LED_PIN, LOW);
    ledState = false;
  }
  
  // Setup web server
  server.on("/", handleRoot);
  server.on("/on", handleLedOn);
  server.on("/off", handleLedOff);
  server.on("/toggle", handleLedToggle);
  server.on("/status", handleStatus);
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("✅ Web server started!");
  
  Serial.println();
  Serial.println("=== SYSTEM READY ===");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Open browser and go to the IP address above");
  Serial.println("====================");
}

void loop() {
  server.handleClient();
  
  // Handle Serial commands
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    Serial.print("Command: ");
    Serial.println(command);
    
    if (command == "on") {
      digitalWrite(LED_PIN, HIGH);
      ledState = true;
      Serial.println("LED ON");
    }
    else if (command == "off") {
      digitalWrite(LED_PIN, LOW);
      ledState = false;
      Serial.println("LED OFF");
    }
    else if (command == "status") {
      printStatus();
    }
    else if (command == "help") {
      Serial.println("Available commands:");
      Serial.println("  on/off - Control LED");
      Serial.println("  status - Show system status");
      Serial.println("  help - Show this help");
    }
  }
  
  delay(10);
}

void printStatus() {
  Serial.println();
  Serial.println("=== SYSTEM STATUS ===");
  Serial.print("LED: ");
  Serial.println(ledState ? "ON" : "OFF");
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

void handleRoot() {
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
  html += "h1 { color: #333; margin-bottom: 30px; font-size: 24px; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>💡 ESP8266 LED Control</h1>";
  
  html += "<div class='status'>";
  html += "<p><strong>LED Status:</strong> " + String(ledState ? "🔴 ON" : "⚫ OFF") + "</p>";
  html += "<p><strong>WiFi:</strong> " + String(ssid) + "</p>";
  html += "<p><strong>IP:</strong> " + WiFi.localIP().toString() + "</p>";
  html += "<p><strong>Signal:</strong> " + String(WiFi.RSSI()) + " dBm</p>";
  html += "</div>";
  
  html += "<a href='/on' class='btn'>🔴 TURN ON</a><br>";
  html += "<a href='/off' class='btn btn-off'>⚫ TURN OFF</a><br>";
  html += "<a href='/toggle' class='btn'>🔄 TOGGLE</a><br>";
  html += "<a href='/status' class='btn'>📊 STATUS</a>";
  html += "<br><br>";
  html += "<p style='color:#666; font-size:14px;'>🎮 LED Control via Web Interface</p>";
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

void handleStatus() {
  String status = "LED: " + String(ledState ? "ON" : "OFF") + "\n";
  status += "WiFi: " + String(ssid) + "\n";
  status += "IP: " + WiFi.localIP().toString() + "\n";
  status += "Signal Strength: " + String(WiFi.RSSI()) + " dBm";
  
  Serial.println("Status requested via web");
  server.send(200, "text/plain", status);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
} 