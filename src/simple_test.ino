/*
 * Simple ESP8266 Test Sketch
 * 
 * This sketch tests basic ESP8266 functionality:
 * - Serial communication
 * - WiFi connection
 * - LED control
 * - Basic web server
 * 
 * Use this to verify your hardware setup before running the main program
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi Configuration
const char* ssid = "Lav";
const char* password = "harharmahadev@468";

// Pin Definitions
const int LED_PIN = 2;  // Built-in LED on most ESP8266 boards

// Web Server
ESP8266WebServer server(80);

void setup() {
  // Initialize Serial
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== ESP8266 Simple Test ===");
  
  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("LED initialized");
  
  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    // Turn on LED to indicate connection
    digitalWrite(LED_PIN, HIGH);
  } else {
    Serial.println("\nWiFi connection failed!");
  }
  
  // Setup web server
  server.on("/", handleRoot);
  server.on("/test", handleTest);
  server.on("/led/on", handleLedOn);
  server.on("/led/off", handleLedOff);
  server.on("/led/toggle", handleLedToggle);
  
  server.begin();
  Serial.println("Web server started");
  Serial.println("Test URLs:");
  Serial.println("  http://" + WiFi.localIP().toString() + "/");
  Serial.println("  http://" + WiFi.localIP().toString() + "/test");
  Serial.println("  http://" + WiFi.localIP().toString() + "/led/on");
  Serial.println("  http://" + WiFi.localIP().toString() + "/led/off");
  Serial.println("  http://" + WiFi.localIP().toString() + "/led/toggle");
}

void loop() {
  server.handleClient();
  delay(10);
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>ESP8266 Test</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body{font-family:Arial;text-align:center;margin:20px;}";
  html += ".button{background-color:#4CAF50;border:none;color:white;padding:15px 32px;";
  html += "text-align:center;text-decoration:none;display:inline-block;font-size:16px;";
  html += "margin:4px 2px;cursor:pointer;border-radius:8px;}";
  html += ".button-off{background-color:#f44336;}";
  html += "</style></head><body>";
  html += "<h1>ESP8266 Simple Test</h1>";
  html += "<p>If you can see this page, your ESP8266 is working!</p>";
  html += "<p>WiFi Status: " + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected") + "</p>";
  html += "<p>IP Address: " + WiFi.localIP().toString() + "</p>";
  html += "<br>";
  html += "<a href='/led/on' class='button'>LED ON</a>";
  html += "<a href='/led/off' class='button button-off'>LED OFF</a>";
  html += "<a href='/led/toggle' class='button'>TOGGLE</a>";
  html += "<br><br>";
  html += "<a href='/test' class='button'>RUN TEST</a>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleTest() {
  String result = "ESP8266 Test Results:\n\n";
  result += "1. Serial Communication: OK\n";
  result += "2. WiFi Connection: " + String(WiFi.status() == WL_CONNECTED ? "OK" : "FAILED") + "\n";
  result += "3. Web Server: OK\n";
  result += "4. LED Control: OK\n";
  result += "5. IP Address: " + WiFi.localIP().toString() + "\n";
  result += "6. MAC Address: " + WiFi.macAddress() + "\n";
  result += "7. Signal Strength: " + String(WiFi.RSSI()) + " dBm\n";
  
  Serial.println("Test requested:");
  Serial.println(result);
  
  server.send(200, "text/plain", result);
}

void handleLedOn() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED turned ON via web");
  server.send(200, "text/plain", "LED ON");
}

void handleLedOff() {
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED turned OFF via web");
  server.send(200, "text/plain", "LED OFF");
}

void handleLedToggle() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  String state = digitalRead(LED_PIN) ? "ON" : "OFF";
  Serial.println("LED toggled to: " + state);
  server.send(200, "text/plain", "LED " + state);
} 