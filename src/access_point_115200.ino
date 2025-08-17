/*
 * ESP8266 Access Point Mode - LED Control (9600 Baud)
 * 
 * Creates its own WiFi network to bypass router restrictions
 * Connect directly to ESP8266's WiFi network
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Access Point Configuration
const char* ap_ssid = "ESP8266_LED_Control";     // WiFi network name
const char* ap_password = "12345678";            // WiFi password (min 8 chars)

// Pin Definition
const int LED_PIN = 2;  // Your red LED connected to GPIO2

// Global Variables
ESP8266WebServer server(80);
bool ledState = false;

void setup() {
  // Initialize Serial at 9600 baud
  Serial.begin(9600);
  delay(3000);  // Wait for Serial Monitor to be opened
  
  // Clear any garbage and send initial message
  Serial.println();
  Serial.println();
  Serial.println("=====================================");
  Serial.println("Starting ESP8266 LED Control");
  Serial.println("=====================================");
  Serial.flush();
  delay(1000);
  
  Serial.println("Baud Rate: 9600");
  Serial.flush();
  delay(500);
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED initialized");
  Serial.flush();
  delay(500);
  
  // Test LED with longer delays
  Serial.println("Testing LED...");
  Serial.flush();
  for(int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(500);  // Longer blink
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
  Serial.println("LED test complete");
  Serial.flush();
  delay(1000);
  
  // Create Access Point with more feedback
  Serial.println("Creating WiFi Access Point...");
  Serial.flush();
  WiFi.mode(WIFI_AP);
  delay(500);
  
  bool success = WiFi.softAP(ap_ssid, ap_password);
  delay(1000);  // Wait for AP to start
  
  if (success) {
    Serial.println("✅ Access Point Created Successfully!");
  } else {
    Serial.println("❌ Failed to create Access Point");
  }
  Serial.flush();
  delay(500);
  
  // Get and show IP address
  IPAddress IP = WiFi.softAPIP();
  Serial.println();
  Serial.println("=== WiFi Network Details ===");
  Serial.print("📱 Network Name: ");
  Serial.println(ap_ssid);
  Serial.print("🔑 Password: ");
  Serial.println(ap_password);
  Serial.print("🌐 IP Address: ");
  Serial.println(IP);
  Serial.println("===========================");
  Serial.flush();
  delay(1000);
  
  // Turn LED ON to show AP is ready
  digitalWrite(LED_PIN, HIGH);
  ledState = true;
  
  // Setup web server with feedback
  Serial.println();
  Serial.println("Starting web server...");
  server.on("/", handleRoot);
  server.on("/on", handleLedOn);
  server.on("/off", handleLedOff);
  server.on("/toggle", handleLedToggle);
  server.on("/status", handleStatus);
  
  server.begin();
  Serial.println("✅ Web server started!");
  Serial.println();
  Serial.println("=== INSTRUCTIONS ===");
  Serial.println("1. Disconnect from your current WiFi");
  Serial.println("2. Connect to WiFi network: " + String(ap_ssid));
  Serial.println("3. Use password: " + String(ap_password));
  Serial.println("4. Open browser and go to: http://" + IP.toString());
  Serial.println();
  Serial.println("Available commands:");
  Serial.println("- on     : Turn LED on");
  Serial.println("- off    : Turn LED off");
  Serial.println("- toggle : Toggle LED");
  Serial.println("- status : Show current status");
  Serial.println("- info   : Show WiFi details");
  Serial.println("- help   : Show this help");
  Serial.println("================================");
  Serial.flush();
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
      Serial.print("AP IP: ");
      Serial.println(WiFi.softAPIP());
      Serial.print("Connected devices: ");
      Serial.println(WiFi.softAPgetStationNum());
    }
    else if (command == "info") {
      Serial.println("=== WiFi Access Point Info ===");
      Serial.println("Network Name: " + String(ap_ssid));
      Serial.println("Password: " + String(ap_password));
      Serial.println("IP Address: " + WiFi.softAPIP().toString());
      Serial.println("Connected devices: " + String(WiFi.softAPgetStationNum()));
      Serial.println("Web interface: http://" + WiFi.softAPIP().toString());
    }
    else if (command == "help") {
      Serial.println("Available commands: on, off, toggle, status, info, help");
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
  html += ".success { background: linear-gradient(45deg, #d4edda, #c3e6cb); color: #155724; padding: 15px; border-radius: 10px; margin: 15px 0; }";
  html += "h1 { color: #333; margin-bottom: 30px; font-size: 28px; }";
  html += ".wifi-info { background: linear-gradient(45deg, #fff3cd, #ffeaa7); color: #856404; padding: 15px; border-radius: 10px; margin: 15px 0; font-size: 14px; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>🚀 ESP8266 LED Control</h1>";
  html += "<div class='success'>";
  html += "✅ Direct WiFi Connection Successful!";
  html += "</div>";
  html += "<div class='wifi-info'>";
  html += "📡 Connected to: " + String(ap_ssid) + "<br>";
  html += "🔗 Direct connection - No router needed!<br>";
  html += "⚡ Baud Rate: 9600";
  html += "</div>";
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
  html += "<p style='color:#666; font-size:14px;'>🎮 Control via web or Serial Monitor</p>";
  html += "</div>";
  html += "<script>";
  html += "setTimeout(function(){location.reload();}, 10000);"; // Auto-refresh every 10 seconds
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
  status += "Mode: Access Point\n";
  status += "Network: " + String(ap_ssid) + "\n";
  status += "IP: " + WiFi.softAPIP().toString() + "\n";
  status += "Connected devices: " + String(WiFi.softAPgetStationNum());
  
  Serial.println("Status requested via web");
  server.send(200, "text/plain", status);
} 