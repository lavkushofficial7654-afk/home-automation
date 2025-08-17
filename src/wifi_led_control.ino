/*
 * WiFi LED Control Project
 * ESP8266 WiFi Module with LED Control
 * 
 * Features:
 * - WiFi connection with LED indicator
 * - Web-based LED control
 * - Serial monitoring
 * - Relay control (optional)
 * 
 * Hardware Connections:
 * - LED connected to GPIO2 (with 220Ω resistor)
 * - Relay module connected to GPIO4 (optional)
 * - Status LED connected to GPIO0 (optional)
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi Configuration
const char* ssid = "Lav";
const char* password = "harharmahadev@468";

// Pin Definitions
const int WIFI_LED_PIN = 2;      // Built-in LED on most ESP8266 boards
const int COMMAND_LED_PIN = 0;   // External LED for command indication
const int RELAY_PIN = 4;         // Relay module control pin

// Global Variables
ESP8266WebServer server(80);
bool ledState = false;
bool relayState = false;
unsigned long lastBlinkTime = 0;
const unsigned long BLINK_INTERVAL = 500; // 500ms blink interval

void setup() {
  // Initialize Serial Communication
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== WiFi LED Control Project ===");
  Serial.println("Initializing...");
  
  // Initialize GPIO Pins
  initializePins();
  
  // Connect to WiFi
  connectToWiFi();
  
  // Setup Web Server
  setupWebServer();
  
  Serial.println("Setup complete!");
  Serial.println("LED will blink when WiFi is connected");
}

void loop() {
  // Handle web server requests
  server.handleClient();
  
  // WiFi status monitoring
  monitorWiFiStatus();
  
  // Small delay to prevent watchdog reset
  delay(10);
}

void initializePins() {
  Serial.println("Initializing GPIO pins...");
  
  pinMode(WIFI_LED_PIN, OUTPUT);
  pinMode(COMMAND_LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  // Set initial states
  digitalWrite(WIFI_LED_PIN, LOW);
  digitalWrite(COMMAND_LED_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);
  
  Serial.println("GPIO pins initialized");
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  // Blink LED while connecting
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    digitalWrite(WIFI_LED_PIN, !digitalRead(WIFI_LED_PIN));
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    
    // Turn on WiFi LED to indicate connection
    digitalWrite(WIFI_LED_PIN, HIGH);
  } else {
    Serial.println("\nWiFi connection failed!");
    Serial.println("Please check your credentials and try again");
    // Keep LED blinking to indicate connection failure
  }
}

void setupWebServer() {
  // Define web server routes
  server.on("/", handleRoot);
  server.on("/on", handleLedOn);
  server.on("/off", handleLedOff);
  server.on("/toggle", handleLedToggle);
  server.on("/status", handleStatus);
  server.on("/relay/on", handleRelayOn);
  server.on("/relay/off", handleRelayOff);
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("Web server started");
  Serial.println("Available commands:");
  Serial.println("  / - Main page");
  Serial.println("  /on - Turn LED on");
  Serial.println("  /off - Turn LED off");
  Serial.println("  /toggle - Toggle LED");
  Serial.println("  /status - Get system status");
  Serial.println("  /relay/on - Turn relay on");
  Serial.println("  /relay/off - Turn relay off");
}

void monitorWiFiStatus() {
  static bool lastWiFiStatus = false;
  bool currentWiFiStatus = (WiFi.status() == WL_CONNECTED);
  
  if (currentWiFiStatus != lastWiFiStatus) {
    if (currentWiFiStatus) {
      Serial.println("WiFi reconnected");
      digitalWrite(WIFI_LED_PIN, HIGH);
    } else {
      Serial.println("WiFi disconnected");
      digitalWrite(WIFI_LED_PIN, LOW);
    }
    lastWiFiStatus = currentWiFiStatus;
  }
}

// Web Server Handlers
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>WiFi LED Control</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body{font-family:Arial;text-align:center;margin:20px;}";
  html += ".button{background-color:#4CAF50;border:none;color:white;padding:15px 32px;";
  html += "text-align:center;text-decoration:none;display:inline-block;font-size:16px;";
  html += "margin:4px 2px;cursor:pointer;border-radius:8px;}";
  html += ".button-off{background-color:#f44336;}";
  html += ".status{font-size:18px;margin:20px;}";
  html += "</style></head><body>";
  html += "<h1>WiFi LED Control Panel</h1>";
  html += "<div class='status'>";
  html += "<p>WiFi Status: " + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected") + "</p>";
  html += "<p>IP Address: " + WiFi.localIP().toString() + "</p>";
  html += "<p>LED Status: " + String(ledState ? "ON" : "OFF") + "</p>";
  html += "<p>Relay Status: " + String(relayState ? "ON" : "OFF") + "</p>";
  html += "</div>";
  html += "<a href='/on' class='button'>LED ON</a>";
  html += "<a href='/off' class='button button-off'>LED OFF</a>";
  html += "<a href='/toggle' class='button'>TOGGLE LED</a><br><br>";
  html += "<a href='/relay/on' class='button'>RELAY ON</a>";
  html += "<a href='/relay/off' class='button button-off'>RELAY OFF</a>";
  html += "<script>";
  html += "setInterval(function(){location.reload();}, 5000);"; // Auto-refresh every 5 seconds
  html += "</script></body></html>";
  
  server.send(200, "text/html", html);
}

void handleLedOn() {
  ledState = true;
  digitalWrite(COMMAND_LED_PIN, HIGH);
  digitalWrite(WIFI_LED_PIN, HIGH);
  Serial.println("LED turned ON");
  server.send(200, "text/plain", "LED ON");
}

void handleLedOff() {
  ledState = false;
  digitalWrite(COMMAND_LED_PIN, LOW);
  digitalWrite(WIFI_LED_PIN, LOW);
  Serial.println("LED turned OFF");
  server.send(200, "text/plain", "LED OFF");
}

void handleLedToggle() {
  ledState = !ledState;
  digitalWrite(COMMAND_LED_PIN, ledState ? HIGH : LOW);
  digitalWrite(WIFI_LED_PIN, ledState ? HIGH : LOW);
  Serial.print("LED toggled to: ");
  Serial.println(ledState ? "ON" : "OFF");
  server.send(200, "text/plain", ledState ? "LED ON" : "LED OFF");
}

void handleStatus() {
  String status = "{\"wifi\":\"" + String(WiFi.status() == WL_CONNECTED ? "connected" : "disconnected") + "\",";
  status += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  status += "\"led\":\"" + String(ledState ? "on" : "off") + "\",";
  status += "\"relay\":\"" + String(relayState ? "on" : "off") + "\"}";
  server.send(200, "application/json", status);
}

void handleRelayOn() {
  relayState = true;
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("Relay turned ON");
  server.send(200, "text/plain", "Relay ON");
}

void handleRelayOff() {
  relayState = false;
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("Relay turned OFF");
  server.send(200, "text/plain", "Relay OFF");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  
  server.send(404, "text/plain", message);
} 