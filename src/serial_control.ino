/*
 * Serial + Web LED Control
 * 
 * This version allows LED control via Serial commands AND web interface
 * Uses port 8080 to bypass router restrictions
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi Configuration
const char* ssid = "Lav";
const char* password = "harharmahadev@468";

// Pin Definition
const int LED_PIN = 2;  // Your red LED connected to GPIO2

// Global Variables
ESP8266WebServer server(8080);  // Using port 8080 instead of 80
bool ledState = false;

void setup() {
  // Initialize Serial
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== Serial + Web LED Control ===");
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("🔴 LED initialized - Ready for commands!");
  Serial.println("");
  Serial.println("📋 Serial Commands:");
  Serial.println("  Type 'on' to turn LED ON");
  Serial.println("  Type 'off' to turn LED OFF");
  Serial.println("  Type 'toggle' to toggle LED");
  Serial.println("  Type 'status' to check LED status");
  Serial.println("");
  
  // Test LED first
  Serial.println("🧪 Testing LED...");
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  Serial.println("✅ LED test complete!");
  Serial.println("");
  
  // Connect to WiFi
  Serial.println("🌐 Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection with blinking LED
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(500);
    Serial.print(".");
  }
  
  // Connected!
  Serial.println("\n✅ WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Turn LED ON to show connection
  digitalWrite(LED_PIN, HIGH);
  ledState = true;
  
  // Setup web server
  server.on("/", handleRoot);
  server.on("/on", handleLedOn);
  server.on("/off", handleLedOff);
  server.on("/toggle", handleLedToggle);
  server.on("/status", handleStatus);
  
  server.begin();
  Serial.println("🌐 Web server started on port 8080");
  Serial.println("🎯 Try: http://" + WiFi.localIP().toString() + ":8080");
  Serial.println("📱 Or try from your phone browser");
  Serial.println("");
  Serial.println("🚀 System ready! Try typing 'on' or 'off' in Serial Monitor");
}

void loop() {
  // Handle web requests
  server.handleClient();
  
  // Handle Serial commands
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    Serial.println("📝 Received command: " + command);
    
    if (command == "on") {
      digitalWrite(LED_PIN, HIGH);
      ledState = true;
      Serial.println("🔴 LED turned ON via Serial");
    }
    else if (command == "off") {
      digitalWrite(LED_PIN, LOW);
      ledState = false;
      Serial.println("⚫ LED turned OFF via Serial");
    }
    else if (command == "toggle") {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      ledState = digitalRead(LED_PIN);
      Serial.println("🔄 LED toggled to: " + String(ledState ? "ON" : "OFF"));
    }
    else if (command == "status") {
      Serial.println("📊 LED Status: " + String(ledState ? "ON" : "OFF"));
      Serial.println("📡 WiFi Status: " + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected"));
      Serial.println("🌐 IP Address: " + WiFi.localIP().toString());
    }
    else if (command == "help") {
      Serial.println("📋 Available commands: on, off, toggle, status, help");
    }
    else {
      Serial.println("❌ Unknown command. Type 'help' for available commands");
    }
  }
  
  delay(10);
}

// Web page
void handleRoot() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>LED Control - Port 8080</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; margin: 40px; background: #f5f5f5; }";
  html += ".card { max-width: 350px; margin: auto; background: white; padding: 30px; border-radius: 15px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }";
  html += ".btn { background: #4CAF50; color: white; padding: 15px 30px; border: none; border-radius: 8px; font-size: 16px; margin: 10px; cursor: pointer; text-decoration: none; display: inline-block; }";
  html += ".btn:hover { background: #45a049; }";
  html += ".btn-off { background: #f44336; }";
  html += ".btn-off:hover { background: #da190b; }";
  html += ".status { background: #e8f5e8; padding: 15px; border-radius: 8px; margin: 20px 0; }";
  html += ".note { background: #fff3cd; padding: 10px; border-radius: 5px; margin: 15px 0; font-size: 14px; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='card'>";
  html += "<h1>💡 LED Control</h1>";
  html += "<div class='note'>";
  html += "🚀 Success! Web interface working on port 8080";
  html += "</div>";
  html += "<div class='status'>";
  html += "<p><strong>LED Status:</strong> " + String(ledState ? "🔴 ON" : "⚫ OFF") + "</p>";
  html += "<p><strong>IP:</strong> " + WiFi.localIP().toString() + ":8080</p>";
  html += "<p><strong>Commands work via:</strong><br>Web buttons OR Serial Monitor</p>";
  html += "</div>";
  html += "<a href='/on' class='btn'>🔴 Turn ON</a><br>";
  html += "<a href='/off' class='btn btn-off'>⚫ Turn OFF</a><br>";
  html += "<a href='/toggle' class='btn'>🔄 Toggle</a><br>";
  html += "<a href='/status' class='btn'>📊 Status</a>";
  html += "</div></body></html>";
  
  server.send(200, "text/html", html);
}

// LED control functions
void handleLedOn() {
  digitalWrite(LED_PIN, HIGH);
  ledState = true;
  Serial.println("🔴 LED turned ON via web");
  server.send(200, "text/plain", "LED is now ON!");
}

void handleLedOff() {
  digitalWrite(LED_PIN, LOW);
  ledState = false;
  Serial.println("⚫ LED turned OFF via web");
  server.send(200, "text/plain", "LED is now OFF!");
}

void handleLedToggle() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  ledState = digitalRead(LED_PIN);
  Serial.println("🔄 LED toggled to: " + String(ledState ? "ON" : "OFF"));
  server.send(200, "text/plain", "LED toggled to " + String(ledState ? "ON" : "OFF"));
}

void handleStatus() {
  String status = "LED: " + String(ledState ? "ON" : "OFF") + "\n";
  status += "WiFi: Connected\n";
  status += "IP: " + WiFi.localIP().toString() + ":8080";
  
  Serial.println("📊 Status requested via web");
  server.send(200, "text/plain", status);
} 