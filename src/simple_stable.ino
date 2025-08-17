/*
 * Simple Stable WiFi LED Control
 * 
 * This version is super simple and stable - no complex monitoring
 * Uses your existing GPIO2 wiring
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi Configuration
const char* ssid = "Lav";
const char* password = "harharmahadev@468";

// Pin Definition - using your existing wiring
const int LED_PIN = 2;  // Your red LED connected to GPIO2

// Global Variables
ESP8266WebServer server(80);
bool ledState = false;

void setup() {
  // Initialize Serial
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== Simple Stable WiFi LED Control ===");
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Connect to WiFi (one time only)
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection with blinking LED
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Blink while connecting
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
  
  server.begin();
  Serial.println("🌐 Web server started");
  Serial.println("🎯 Go to: http://" + WiFi.localIP().toString());
  Serial.println("🚀 System ready!");
}

void loop() {
  // Just handle web requests - no WiFi monitoring!
  server.handleClient();
  delay(10);
}

// Web page
void handleRoot() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>Simple LED Control</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; margin: 40px; background: #f5f5f5; }";
  html += ".card { max-width: 300px; margin: auto; background: white; padding: 30px; border-radius: 15px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }";
  html += ".btn { background: #4CAF50; color: white; padding: 15px 30px; border: none; border-radius: 8px; font-size: 16px; margin: 10px; cursor: pointer; text-decoration: none; display: inline-block; }";
  html += ".btn:hover { background: #45a049; }";
  html += ".btn-off { background: #f44336; }";
  html += ".btn-off:hover { background: #da190b; }";
  html += ".status { background: #e8f5e8; padding: 15px; border-radius: 8px; margin: 20px 0; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='card'>";
  html += "<h1>💡 LED Control</h1>";
  html += "<div class='status'>";
  html += "<p><strong>LED Status:</strong> " + String(ledState ? "🔴 ON" : "⚫ OFF") + "</p>";
  html += "<p><strong>IP:</strong> " + WiFi.localIP().toString() + "</p>";
  html += "</div>";
  html += "<a href='/on' class='btn'>🔴 Turn ON</a><br>";
  html += "<a href='/off' class='btn btn-off'>⚫ Turn OFF</a><br>";
  html += "<a href='/toggle' class='btn'>🔄 Toggle</a>";
  html += "</div></body></html>";
  
  server.send(200, "text/html", html);
}

// LED control functions
void handleLedOn() {
  digitalWrite(LED_PIN, HIGH);
  ledState = true;
  Serial.println("🔴 LED turned ON");
  server.send(200, "text/plain", "LED is now ON!");
}

void handleLedOff() {
  digitalWrite(LED_PIN, LOW);
  ledState = false;
  Serial.println("⚫ LED turned OFF");
  server.send(200, "text/plain", "LED is now OFF!");
}

void handleLedToggle() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  ledState = digitalRead(LED_PIN);
  Serial.println("🔄 LED toggled to: " + String(ledState ? "ON" : "OFF"));
  server.send(200, "text/plain", "LED toggled to " + String(ledState ? "ON" : "OFF"));
} 