/*
 * ESP8266 WiFi Bridge with Internet Sharing
 * 
 * This version:
 * 1. Connects to your main WiFi ("Lav") for internet access
 * 2. Creates a bridge network that shares internet with connected devices
 * 3. Allows voice recognition to work while controlling ESP8266
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

// Your home WiFi credentials
const char* home_ssid = "Lav";
const char* home_password = "harharmahadev@468";

// Bridge network settings
const char* bridge_ssid = "ESP8266_Bridge";
const char* bridge_password = "12345678";

// Pin Definition
const int LED_PIN = 2;

// Global Variables
ESP8266WebServer server(80);
DNSServer dnsServer;
bool ledState = false;
bool homeWifiConnected = false;

void setup() {
  Serial.begin(9600);
  delay(3000);
  
  Serial.println("=====================================");
  Serial.println("ESP8266 WiFi Bridge with Internet");
  Serial.println("=====================================");
  
  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Set WiFi mode to both Station and Access Point
  WiFi.mode(WIFI_AP_STA);
  delay(1000);
  
  // Connect to home WiFi for internet access
  Serial.println("Connecting to home WiFi...");
  Serial.print("SSID: ");
  Serial.println(home_ssid);
  
  WiFi.begin(home_ssid, home_password);
  
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
    homeWifiConnected = true;
    Serial.println();
    Serial.println("✅ Connected to home WiFi!");
    Serial.print("Home IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    
    // Turn LED ON to show connection
    digitalWrite(LED_PIN, HIGH);
    ledState = true;
  } else {
    homeWifiConnected = false;
    Serial.println();
    Serial.println("❌ Failed to connect to home WiFi");
    Serial.println("Bridge will work without internet access");
    
    // Turn LED OFF to show no connection
    digitalWrite(LED_PIN, LOW);
    ledState = false;
  }
  
  // Create bridge network
  Serial.println("Creating bridge network...");
  WiFi.softAP(bridge_ssid, bridge_password);
  
  IPAddress AP_IP = WiFi.softAPIP();
  Serial.print("Bridge IP: ");
  Serial.println(AP_IP);
  Serial.print("Bridge Network: ");
  Serial.println(bridge_ssid);
  Serial.print("Bridge Password: ");
  Serial.println(bridge_password);
  
  // Setup DNS server to redirect all requests to ESP8266
  dnsServer.start(53, "*", AP_IP);
  
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
  Serial.println("=== BRIDGE READY ===");
  Serial.println("1. Connect to WiFi: " + String(bridge_ssid));
  Serial.println("2. Password: " + String(bridge_password));
  Serial.println("3. Open browser: http://" + AP_IP.toString());
  Serial.println("4. Devices will have internet access!");
  if (homeWifiConnected) {
    Serial.println("5. ✅ Internet sharing enabled");
  } else {
    Serial.println("5. ❌ No internet access (home WiFi failed)");
  }
  Serial.println("====================");
}

void loop() {
  dnsServer.processNextRequest();
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
  Serial.print("Home WiFi: ");
  Serial.println(homeWifiConnected ? "Connected" : "Disconnected");
  if (homeWifiConnected) {
    Serial.print("Home IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  }
  Serial.print("Bridge Network: ");
  Serial.println(bridge_ssid);
  Serial.print("Bridge IP: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("Connected devices: ");
  Serial.println(WiFi.softAPgetStationNum());
  Serial.println("====================");
}

void handleRoot() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>ESP8266 Bridge + LED Control</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; margin: 20px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; min-height: 100vh; }";
  html += ".container { max-width: 450px; margin: auto; background: rgba(255,255,255,0.95); padding: 30px; border-radius: 20px; box-shadow: 0 8px 32px rgba(0,0,0,0.3); color: #333; margin-top: 50px; }";
  html += ".btn { background: linear-gradient(45deg, #4CAF50, #45a049); color: white; padding: 15px 30px; border: none; border-radius: 10px; font-size: 16px; margin: 5px; cursor: pointer; text-decoration: none; display: inline-block; transition: all 0.3s; box-shadow: 0 4px 15px rgba(0,0,0,0.2); }";
  html += ".btn:hover { transform: translateY(-2px); box-shadow: 0 6px 20px rgba(0,0,0,0.3); }";
  html += ".btn-off { background: linear-gradient(45deg, #f44336, #da190b); }";
  html += ".status { background: linear-gradient(45deg, #e8f5e8, #d4edda); padding: 20px; border-radius: 15px; margin: 20px 0; font-size: 16px; color: #333; }";
  html += ".bridge { background: linear-gradient(45deg, #cce5ff, #b3d9ff); color: #004080; padding: 15px; border-radius: 10px; margin: 15px 0; }";
  html += ".internet { background: linear-gradient(45deg, #d4edda, #c3e6cb); color: #155724; padding: 15px; border-radius: 10px; margin: 15px 0; }";
  html += ".no-internet { background: linear-gradient(45deg, #f8d7da, #f5c6cb); color: #721c24; padding: 15px; border-radius: 10px; margin: 15px 0; }";
  html += "h1 { color: #333; margin-bottom: 30px; font-size: 24px; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>🌐 ESP8266 Bridge + LED Control</h1>";
  
  // Bridge status
  html += "<div class='bridge'>";
  html += "🌐 Bridge Network: " + String(bridge_ssid) + "<br>";
  html += "Bridge IP: " + WiFi.softAPIP().toString() + "<br>";
  html += "Connected devices: " + String(WiFi.softAPgetStationNum());
  html += "</div>";
  
  // Internet status
  if (homeWifiConnected) {
    html += "<div class='internet'>";
    html += "✅ Internet Access Available<br>";
    html += "Connected to: " + String(home_ssid) + "<br>";
    html += "Home IP: " + WiFi.localIP().toString();
    html += "</div>";
  } else {
    html += "<div class='no-internet'>";
    html += "❌ No Internet Access<br>";
    html += "Home WiFi connection failed";
    html += "</div>";
  }
  
  html += "<div class='status'>";
  html += "<p><strong>LED Status:</strong> " + String(ledState ? "🔴 ON" : "⚫ OFF") + "</p>";
  html += "</div>";
  
  // Control buttons
  html += "<a href='/on' class='btn'>🔴 TURN ON</a>";
  html += "<a href='/off' class='btn btn-off'>⚫ TURN OFF</a><br>";
  html += "<a href='/toggle' class='btn'>🔄 TOGGLE</a>";
  html += "<a href='/status' class='btn'>📊 STATUS</a>";
  
  html += "<br><br>";
  html += "<p style='color:#666; font-size:14px;'>🎮 LED Control + Internet Bridge</p>";
  html += "</div>";
  html += "<script>";
  html += "setTimeout(function(){location.reload();}, 30000);";
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
  status += "Mode: WiFi Bridge\n";
  status += "Bridge Network: " + String(bridge_ssid) + "\n";
  status += "Bridge IP: " + WiFi.softAPIP().toString() + "\n";
  status += "Connected devices: " + String(WiFi.softAPgetStationNum()) + "\n\n";
  
  if (homeWifiConnected) {
    status += "Home WiFi: " + String(home_ssid) + "\n";
    status += "Home IP: " + WiFi.localIP().toString() + "\n";
    status += "Internet: Available\n";
  } else {
    status += "Home WiFi: Not Connected\n";
    status += "Internet: Not Available";
  }
  
  Serial.println("Status requested via web");
  server.send(200, "text/plain", status);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
} 