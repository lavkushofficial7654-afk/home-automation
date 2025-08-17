/*
 * ESP8266 Main WiFi Connection - LED Control
 * 
 * This version connects ESP8266 directly to your main WiFi
 * Both ESP8266 and your computer will be on the same network
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Your main WiFi credentials
const char* ssid = "Lav";  // Your main WiFi name
const char* password = "harharmahadev@468";  // Your main WiFi password

// Pin Definition
const int LED_PIN = 2;  // Your red LED connected to GPIO2

// Global Variables
ESP8266WebServer server(80);
bool ledState = false;

void setup() {
  // Initialize Serial at 9600 baud
  Serial.begin(9600);
  delay(3000);
  
  Serial.println();
  Serial.println("=====================================");
  Serial.println("ESP8266 Main WiFi Connection");
  Serial.println("=====================================");
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED initialized");
  
  // Test LED
  Serial.println("Testing LED...");
  for(int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
  Serial.println("LED test complete");
  
  // Connect to main WiFi
  Serial.println("Connecting to main WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int wifi_attempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_attempts < 30) {
    delay(500);
    Serial.print(".");
    wifi_attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ Connected to main WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Internet access available!");
    
    // Turn LED ON to show connection success
    digitalWrite(LED_PIN, HIGH);
    ledState = true;
  } else {
    Serial.println();
    Serial.println("❌ Failed to connect to main WiFi");
    Serial.println("Check WiFi credentials and try again");
    return;
  }
  
  // Setup web server
  Serial.println("Starting web server...");
  server.on("/", handleRoot);
  server.on("/on", handleLedOn);
  server.on("/off", handleLedOff);
  server.on("/toggle", handleLedToggle);
  server.on("/status", handleStatus);
  
  server.begin();
  Serial.println("✅ Web server started!");
  
  Serial.println();
  Serial.println("=== SETUP COMPLETE ===");
  Serial.println("Both devices are on the same network!");
  Serial.println("ESP8266 IP: " + WiFi.localIP().toString());
  Serial.println("Open browser: http://" + WiFi.localIP().toString());
  Serial.println("======================");
}

void loop() {
  server.handleClient();
  
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Attempting to reconnect...");
    WiFi.begin(ssid, password);
    delay(5000);
  }
  
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
      Serial.print("LED: ");
      Serial.println(ledState ? "ON" : "OFF");
      Serial.print("WiFi: ");
      Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
    }
    else if (command == "ip") {
      Serial.println("ESP8266 IP: " + WiFi.localIP().toString());
      Serial.println("Use this IP in your browser");
    }
  }
  
  delay(10);
}

// Web page
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
  html += ".internet { background: linear-gradient(45deg, #cce5ff, #b3d9ff); color: #004080; padding: 15px; border-radius: 10px; margin: 15px 0; }";
  html += "h1 { color: #333; margin-bottom: 30px; font-size: 24px; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>🚀 ESP8266 LED Control</h1>";
  
  html += "<div class='internet'>";
  html += "🌐 Internet Available!<br>";
  html += "Connected to: " + String(ssid) + "<br>";
  html += "ESP8266 IP: " + WiFi.localIP().toString();
  html += "</div>";
  
  html += "<div class='status'>";
  html += "<p><strong>LED Status:</strong> " + String(ledState ? "🔴 ON" : "⚫ OFF") + "</p>";
  html += "<p><strong>Network:</strong> " + String(ssid) + "</p>";
  html += "<p><strong>Signal Strength:</strong> " + String(WiFi.RSSI()) + " dBm</p>";
  html += "</div>";
  html += "<a href='/on' class='btn'>🔴 TURN ON</a><br>";
  html += "<a href='/off' class='btn btn-off'>⚫ TURN OFF</a><br>";
  html += "<a href='/toggle' class='btn'>🔄 TOGGLE</a><br>";
  html += "<a href='/status' class='btn'>📊 STATUS</a>";
  html += "<br><br>";
  html += "<p style='color:#666; font-size:14px;'>🎮 Same Network + Internet Access</p>";
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
  status += "Network: " + String(ssid) + "\n";
  status += "IP: " + WiFi.localIP().toString() + "\n";
  status += "Signal: " + String(WiFi.RSSI()) + " dBm\n";
  status += "Internet: Available";
  
  Serial.println("Status requested via web");
  server.send(200, "text/plain", status);
} 