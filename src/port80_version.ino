/*
 * WiFi LED Control - Port 80 Version
 * 
 * Uses standard port 80 which might bypass router restrictions
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi Configuration
const char* ssid = "Lav";
const char* password = "harharmahadev@468";

// Pin Definition
const int LED_PIN = 2;  // Your red LED connected to GPIO2

// Global Variables
ESP8266WebServer server(80);  // Using standard port 80
bool ledState = false;

void setup() {
  // Initialize Serial at 9600 baud
  Serial.begin(9600);
  delay(2000);
  
  Serial.println();
  Serial.println("=== WiFi LED Control - Port 80 ===");
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("LED initialized");
  
  // Test LED
  Serial.println("Testing LED...");
  for(int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
  Serial.println("LED test complete");
  
  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi Connected!");
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
  Serial.println("Web server started on port 80");
  Serial.print("Try: http://");
  Serial.println(WiFi.localIP());
  Serial.print("Or try from phone: http://");
  Serial.println(WiFi.localIP());
  Serial.println("");
  Serial.println("Serial commands: on, off, toggle, status");
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
    else if (command == "toggle") {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      ledState = digitalRead(LED_PIN);
      Serial.println(ledState ? "LED ON" : "LED OFF");
    }
    else if (command == "status") {
      Serial.print("LED: ");
      Serial.println(ledState ? "ON" : "OFF");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
    }
    else if (command == "ip") {
      Serial.print("Current IP: ");
      Serial.println(WiFi.localIP());
    }
  }
  
  delay(10);
}

// Web page
void handleRoot() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>LED Control</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; margin: 20px; background: #f0f8ff; }";
  html += ".container { max-width: 400px; margin: auto; background: white; padding: 30px; border-radius: 15px; box-shadow: 0 4px 12px rgba(0,0,0,0.1); }";
  html += ".btn { background: #4CAF50; color: white; padding: 20px 40px; border: none; border-radius: 10px; font-size: 18px; margin: 10px; cursor: pointer; text-decoration: none; display: inline-block; transition: all 0.3s; }";
  html += ".btn:hover { background: #45a049; transform: translateY(-2px); }";
  html += ".btn-off { background: #f44336; }";
  html += ".btn-off:hover { background: #da190b; }";
  html += ".status { background: #e8f5e8; padding: 20px; border-radius: 10px; margin: 20px 0; font-size: 16px; }";
  html += ".success { background: #d4edda; color: #155724; padding: 15px; border-radius: 8px; margin: 15px 0; }";
  html += "h1 { color: #333; margin-bottom: 30px; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>🔴 LED Control Panel</h1>";
  html += "<div class='success'>";
  html += "✅ Successfully connected on port 80!";
  html += "</div>";
  html += "<div class='status'>";
  html += "<p><strong>LED Status:</strong> " + String(ledState ? "🔴 ON" : "⚫ OFF") + "</p>";
  html += "<p><strong>Device IP:</strong> " + WiFi.localIP().toString() + "</p>";
  html += "<p><strong>WiFi Signal:</strong> " + String(WiFi.RSSI()) + " dBm</p>";
  html += "</div>";
  html += "<a href='/on' class='btn'>🔴 TURN ON</a><br>";
  html += "<a href='/off' class='btn btn-off'>⚫ TURN OFF</a><br>";
  html += "<a href='/toggle' class='btn'>🔄 TOGGLE</a><br>";
  html += "<a href='/status' class='btn'>📊 STATUS</a>";
  html += "<br><br>";
  html += "<p style='color:#666; font-size:14px;'>Control via web or Serial Monitor</p>";
  html += "</div>";
  html += "<script>";
  html += "setTimeout(function(){location.reload();}, 15000);"; // Auto-refresh every 15 seconds
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
  status += "WiFi: Connected\n";
  status += "IP: " + WiFi.localIP().toString() + "\n";
  status += "Signal: " + String(WiFi.RSSI()) + " dBm";
  
  Serial.println("Status requested via web");
  server.send(200, "text/plain", status);
} 