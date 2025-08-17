/*
 * ESP8266 WiFi Bridge with Internet Access
 * 
 * This version connects to your home WiFi and provides internet access
 * to devices connected to ESP8266's network
 * Includes internet connectivity checks and fallback modes
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266HTTPClient.h>

// Your home WiFi credentials
const char* home_ssid = "Lav";
const char* home_password = "harharmahadev@468";

// ESP8266 Access Point settings
const char* ap_ssid = "ESP8266_Bridge";
const char* ap_password = "12345678";

// Pin Definition
const int LED_PIN = 2;

// Global Variables
ESP8266WebServer server(80);
DNSServer dnsServer;
bool ledState = false;
bool internetAvailable = false;
bool homeWifiConnected = false;
unsigned long lastInternetCheck = 0;
const unsigned long INTERNET_CHECK_INTERVAL = 30000; // Check every 30 seconds

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
  
  // Try to connect to home WiFi
  connectToHomeWiFi();
  
  // Create Access Point (always works)
  Serial.println("Creating Access Point...");
  WiFi.softAP(ap_ssid, ap_password);
  
  IPAddress AP_IP = WiFi.softAPIP();
  Serial.print("AP IP: ");
  Serial.println(AP_IP);
  
  // Setup DNS server to redirect all requests to ESP8266
  dnsServer.start(53, "*", AP_IP);
  
  // Setup web server
  server.on("/", handleRoot);
  server.on("/on", handleLedOn);
  server.on("/off", handleLedOff);
  server.on("/toggle", handleLedToggle);
  server.on("/status", handleStatus);
  server.on("/reconnect", handleReconnect);
  server.on("/test-internet", handleTestInternet);
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("✅ Web server started!");
  
  // Turn LED ON to show ready
  digitalWrite(LED_PIN, HIGH);
  ledState = true;
  
  // Initial internet check
  checkInternetConnectivity();
  
  printStatus();
  
  Serial.println();
  Serial.println("⚠️  IMPORTANT: This is an Access Point, not a full bridge");
  Serial.println("   Connected devices can access ESP8266 web interface");
  Serial.println("   For full internet access, you need a different approach:");
  Serial.println("   1. Use ESP32 with Ethernet");
  Serial.println("   2. Use a WiFi range extender");
  Serial.println("   3. Configure your router's guest network");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  
  // Periodic internet connectivity check
  if (millis() - lastInternetCheck > INTERNET_CHECK_INTERVAL) {
    checkInternetConnectivity();
    lastInternetCheck = millis();
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
      printStatus();
    }
    else if (command == "reconnect") {
      Serial.println("Reconnecting to home WiFi...");
      connectToHomeWiFi();
      checkInternetConnectivity();
      printStatus();
    }
    else if (command == "test") {
      Serial.println("Testing internet connectivity...");
      checkInternetConnectivity();
      printStatus();
    }
    else if (command == "help") {
      Serial.println("Available commands:");
      Serial.println("  on/off - Control LED");
      Serial.println("  status - Show system status");
      Serial.println("  reconnect - Reconnect to home WiFi");
      Serial.println("  test - Test internet connectivity");
      Serial.println("  help - Show this help");
    }
  }
  
  delay(10);
}

void connectToHomeWiFi() {
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
    Serial.println("Possible issues:");
    Serial.println("1. Wrong SSID or password");
    Serial.println("2. WiFi signal too weak");
    Serial.println("3. Router not broadcasting");
    Serial.println("4. Too many devices connected");
    
    // Turn LED OFF to show no connection
    digitalWrite(LED_PIN, LOW);
    ledState = false;
  }
}

void checkInternetConnectivity() {
  if (!homeWifiConnected) {
    internetAvailable = false;
    return;
  }
  
  Serial.println("Testing internet connectivity...");
  
  WiFiClient client;
  HTTPClient http;
  http.setTimeout(5000); // 5 second timeout
  
  // Try to connect to a reliable test server using the new API
  if (http.begin(client, "http://httpbin.org/ip")) {
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
      internetAvailable = true;
      Serial.println("✅ Internet connection working!");
    } else {
      internetAvailable = false;
      Serial.print("❌ Internet test failed. HTTP Code: ");
      Serial.println(httpCode);
    }
    
    http.end();
  } else {
    internetAvailable = false;
    Serial.println("❌ Failed to begin HTTP client");
  }
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
  Serial.print("Internet: ");
  Serial.println(internetAvailable ? "Available" : "Not Available");
  Serial.print("Bridge Network: ");
  Serial.println(ap_ssid);
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
  html += ".btn-warning { background: linear-gradient(45deg, #ff9800, #f57c00); }";
  html += ".status { background: linear-gradient(45deg, #e8f5e8, #d4edda); padding: 20px; border-radius: 15px; margin: 20px 0; font-size: 16px; color: #333; }";
  html += ".bridge { background: linear-gradient(45deg, #cce5ff, #b3d9ff); color: #004080; padding: 15px; border-radius: 10px; margin: 15px 0; }";
  html += ".warning { background: linear-gradient(45deg, #fff3cd, #ffeaa7); color: #856404; padding: 15px; border-radius: 10px; margin: 15px 0; border-left: 4px solid #ffc107; }";
  html += "h1 { color: #333; margin-bottom: 30px; font-size: 24px; }";
  html += ".status-item { margin: 10px 0; padding: 8px; border-radius: 5px; }";
  html += ".status-ok { background: #d4edda; color: #155724; }";
  html += ".status-error { background: #f8d7da; color: #721c24; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>🌐 ESP8266 Bridge + LED Control</h1>";
  
  // Connection status
  html += "<div class='bridge'>";
  html += "🌐 Bridge Network: " + String(ap_ssid) + "<br>";
  html += "Bridge IP: " + WiFi.softAPIP().toString() + "<br>";
  html += "Connected devices: " + String(WiFi.softAPgetStationNum());
  html += "</div>";
  
  // Home WiFi status
  if (homeWifiConnected) {
    html += "<div class='status-item status-ok'>";
    html += "✅ Home WiFi: " + String(home_ssid) + "<br>";
    html += "IP: " + WiFi.localIP().toString() + "<br>";
    html += "Signal: " + String(WiFi.RSSI()) + " dBm";
    html += "</div>";
  } else {
    html += "<div class='status-item status-error'>";
    html += "❌ Home WiFi: Not Connected<br>";
    html += "No internet access available";
    html += "</div>";
  }
  
  // Internet status
  if (internetAvailable) {
    html += "<div class='status-item status-ok'>";
    html += "✅ Internet: Available";
    html += "</div>";
  } else {
    html += "<div class='status-item status-error'>";
    html += "❌ Internet: Not Available";
    html += "</div>";
  }
  
  html += "<div class='status'>";
  html += "<p><strong>LED Status:</strong> " + String(ledState ? "🔴 ON" : "⚫ OFF") + "</p>";
  html += "</div>";
  
  // Control buttons
  html += "<a href='/on' class='btn'>🔴 TURN ON</a>";
  html += "<a href='/off' class='btn btn-off'>⚫ TURN OFF</a><br>";
  html += "<a href='/toggle' class='btn'>🔄 TOGGLE</a>";
  html += "<a href='/status' class='btn'>📊 STATUS</a><br>";
  html += "<a href='/reconnect' class='btn btn-warning'>🔄 RECONNECT WiFi</a>";
  html += "<a href='/test-internet' class='btn btn-warning'>🌐 TEST INTERNET</a>";
  
  if (!homeWifiConnected) {
    html += "<div class='warning'>";
    html += "<strong>Troubleshooting:</strong><br>";
    html += "1. Check WiFi password<br>";
    html += "2. Move closer to router<br>";
    html += "3. Check router settings<br>";
    html += "4. Try reconnecting";
    html += "</div>";
  }
  
  html += "<div class='warning'>";
  html += "<strong>Note:</strong> This is an Access Point, not a full internet bridge.<br>";
  html += "Connected devices can access this web interface but not external internet.";
  html += "</div>";
  
  html += "<br><br>";
  html += "<p style='color:#666; font-size:14px;'>🎮 LED Control + Access Point</p>";
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
  status += "Mode: Access Point (Not Full Bridge)\n";
  status += "Bridge Network: " + String(ap_ssid) + "\n";
  status += "Bridge IP: " + WiFi.softAPIP().toString() + "\n";
  status += "Connected devices: " + String(WiFi.softAPgetStationNum()) + "\n\n";
  
  if (homeWifiConnected) {
    status += "Home WiFi: " + String(home_ssid) + "\n";
    status += "Home IP: " + WiFi.localIP().toString() + "\n";
    status += "Signal Strength: " + String(WiFi.RSSI()) + " dBm\n";
    status += "Internet: " + String(internetAvailable ? "Available" : "Not Available");
  } else {
    status += "Home WiFi: Not Connected\n";
    status += "Internet: Not Available";
  }
  
  Serial.println("Status requested via web");
  server.send(200, "text/plain", status);
}

void handleReconnect() {
  Serial.println("Reconnect requested via web");
  server.send(200, "text/plain", "Reconnecting to home WiFi...");
  
  // Disconnect first
  WiFi.disconnect();
  delay(1000);
  
  // Try to reconnect
  connectToHomeWiFi();
  checkInternetConnectivity();
}

void handleTestInternet() {
  Serial.println("Internet test requested via web");
  checkInternetConnectivity();
  
  String result = "Internet Test Results:\n";
  result += "Home WiFi: " + String(homeWifiConnected ? "Connected" : "Disconnected") + "\n";
  result += "Internet: " + String(internetAvailable ? "Available" : "Not Available");
  
  server.send(200, "text/plain", result);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
} 