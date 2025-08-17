/*
 * ESP8266 Dual Mode - Internet + LED Control
 * 
 * This version connects to your home WiFi (for internet) 
 * while also creating its own access point for LED control
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Your home WiFi credentials (for internet access)
const char* home_ssid = "Lav";  // Replace with your home WiFi name
const char* home_password = "harharmahadev@468";  // Replace with your home WiFi password

// Access Point Configuration (for LED control)
const char* ap_ssid = "ESP8266_LED_Control";
const char* ap_password = "12345678";

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
  Serial.println("Starting ESP8266 Dual Mode");
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
  
  // Set WiFi mode to both Station and Access Point
  WiFi.mode(WIFI_AP_STA);
  delay(1000);
  
  // Connect to home WiFi for internet access
  Serial.println("Connecting to home WiFi for internet...");
  WiFi.begin(home_ssid, home_password);
  
  int wifi_attempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_attempts < 20) {
    delay(500);
    Serial.print(".");
    wifi_attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ Connected to home WiFi!");
    Serial.print("Home WiFi IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("Internet access available!");
  } else {
    Serial.println();
    Serial.println("❌ Failed to connect to home WiFi");
    Serial.println("Continuing without internet...");
  }
  
  // Create Access Point for LED control
  Serial.println("Creating Access Point...");
  bool ap_success = WiFi.softAP(ap_ssid, ap_password);
  delay(1000);
  
  if (ap_success) {
    Serial.println("✅ Access Point Created!");
    IPAddress AP_IP = WiFi.softAPIP();
    Serial.print("AP IP Address: ");
    Serial.println(AP_IP);
  } else {
    Serial.println("❌ Failed to create Access Point");
  }
  
  // Turn LED ON to show setup is complete
  digitalWrite(LED_PIN, HIGH);
  ledState = true;
  
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
  Serial.println("1. Connect to WiFi: " + String(ap_ssid));
  Serial.println("2. Password: " + String(ap_password));
  Serial.println("3. Open browser: http://" + WiFi.softAPIP().toString());
  Serial.println("4. Your device will have internet through ESP8266!");
  Serial.println("======================");
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
      Serial.print("LED: ");
      Serial.println(ledState ? "ON" : "OFF");
      Serial.print("Home WiFi: ");
      Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
      if (WiFi.status() == WL_CONNECTED) {
        Serial.print("Home IP: ");
        Serial.println(WiFi.localIP());
      }
      Serial.print("AP IP: ");
      Serial.println(WiFi.softAPIP());
      Serial.print("Connected devices: ");
      Serial.println(WiFi.softAPgetStationNum());
    }
    else if (command == "info") {
      Serial.println("=== WiFi Status ===");
      Serial.println("Home WiFi: " + String(home_ssid));
      Serial.println("Status: " + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected"));
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Home IP: " + WiFi.localIP().toString());
        Serial.println("Internet: Available");
      }
      Serial.println("AP Name: " + String(ap_ssid));
      Serial.println("AP IP: " + WiFi.softAPIP().toString());
      Serial.println("Connected devices: " + String(WiFi.softAPgetStationNum()));
    }
  }
  
  delay(10);
}

// Web page
void handleRoot() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>ESP8266 LED Control + Internet</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; margin: 20px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; min-height: 100vh; }";
  html += ".container { max-width: 400px; margin: auto; background: rgba(255,255,255,0.95); padding: 30px; border-radius: 20px; box-shadow: 0 8px 32px rgba(0,0,0,0.3); color: #333; margin-top: 50px; }";
  html += ".btn { background: linear-gradient(45deg, #4CAF50, #45a049); color: white; padding: 20px 40px; border: none; border-radius: 15px; font-size: 18px; margin: 10px; cursor: pointer; text-decoration: none; display: inline-block; transition: all 0.3s; box-shadow: 0 4px 15px rgba(0,0,0,0.2); }";
  html += ".btn:hover { transform: translateY(-3px); box-shadow: 0 6px 20px rgba(0,0,0,0.3); }";
  html += ".btn-off { background: linear-gradient(45deg, #f44336, #da190b); }";
  html += ".status { background: linear-gradient(45deg, #e8f5e8, #d4edda); padding: 20px; border-radius: 15px; margin: 20px 0; font-size: 16px; color: #333; }";
  html += ".success { background: linear-gradient(45deg, #d4edda, #c3e6cb); color: #155724; padding: 15px; border-radius: 10px; margin: 15px 0; }";
  html += ".internet { background: linear-gradient(45deg, #cce5ff, #b3d9ff); color: #004080; padding: 15px; border-radius: 10px; margin: 15px 0; }";
  html += "h1 { color: #333; margin-bottom: 30px; font-size: 24px; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>🚀 ESP8266 LED Control + Internet</h1>";
  
  if (WiFi.status() == WL_CONNECTED) {
    html += "<div class='internet'>";
    html += "🌐 Internet Available!<br>";
    html += "Connected to: " + String(home_ssid) + "<br>";
    html += "Home IP: " + WiFi.localIP().toString();
    html += "</div>";
  } else {
    html += "<div class='internet'>";
    html += "⚠️ No Internet Connection<br>";
    html += "LED control still works!";
    html += "</div>";
  }
  
  html += "<div class='status'>";
  html += "<p><strong>LED Status:</strong> " + String(ledState ? "🔴 ON" : "⚫ OFF") + "</p>";
  html += "<p><strong>Device IP:</strong> " + WiFi.softAPIP().toString() + "</p>";
  html += "<p><strong>Connected Devices:</strong> " + String(WiFi.softAPgetStationNum()) + "</p>";
  html += "</div>";
  html += "<a href='/on' class='btn'>🔴 TURN ON</a><br>";
  html += "<a href='/off' class='btn btn-off'>⚫ TURN OFF</a><br>";
  html += "<a href='/toggle' class='btn'>🔄 TOGGLE</a><br>";
  html += "<a href='/status' class='btn'>📊 STATUS</a>";
  html += "<br><br>";
  html += "<p style='color:#666; font-size:14px;'>🎮 LED Control + Internet Access</p>";
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
  status += "Mode: Dual Mode (AP + STA)\n";
  status += "AP Network: " + String(ap_ssid) + "\n";
  status += "AP IP: " + WiFi.softAPIP().toString() + "\n";
  if (WiFi.status() == WL_CONNECTED) {
    status += "Home WiFi: " + String(home_ssid) + "\n";
    status += "Home IP: " + WiFi.localIP().toString() + "\n";
    status += "Internet: Available\n";
  } else {
    status += "Home WiFi: Disconnected\n";
    status += "Internet: Not Available\n";
  }
  status += "Connected devices: " + String(WiFi.softAPgetStationNum());
  
  Serial.println("Status requested via web");
  server.send(200, "text/plain", status);
} 