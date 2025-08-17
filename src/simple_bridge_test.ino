/*
 * Simple ESP8266 Bridge Test
 * 
 * This tests if ESP8266 can actually provide internet access to connected devices
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Access Point settings
const char* ap_ssid = "ESP8266_Test";
const char* ap_password = "12345678";

// Pin Definition
const int LED_PIN = 2;

// Global Variables
ESP8266WebServer server(80);

void setup() {
  Serial.begin(9600);
  delay(3000);
  
  Serial.println("=====================================");
  Serial.println("ESP8266 Bridge Internet Test");
  Serial.println("=====================================");
  
  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Create Access Point
  Serial.println("Creating Access Point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  
  IPAddress AP_IP = WiFi.softAPIP();
  Serial.print("Access Point IP: ");
  Serial.println(AP_IP);
  Serial.print("Network Name: ");
  Serial.println(ap_ssid);
  Serial.print("Password: ");
  Serial.println(ap_password);
  
  // Setup web server
  server.on("/", handleRoot);
  server.on("/test", handleTest);
  server.begin();
  Serial.println("✅ Web server started!");
  
  // Turn LED ON to show ready
  digitalWrite(LED_PIN, HIGH);
  
  Serial.println();
  Serial.println("=== TEST READY ===");
  Serial.println("1. Connect to WiFi: " + String(ap_ssid));
  Serial.println("2. Password: " + String(ap_password));
  Serial.println("3. Open browser: http://" + AP_IP.toString());
  Serial.println("4. Try to access any website (like google.com)");
  Serial.println("5. If you can't access internet, ESP8266 bridge doesn't work");
  Serial.println("====================");
}

void loop() {
  server.handleClient();
  delay(10);
}

void handleRoot() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>ESP8266 Bridge Test</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; margin: 20px; background: #f0f0f0; }";
  html += ".container { max-width: 400px; margin: auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
  html += ".test-btn { background: #4CAF50; color: white; padding: 15px 30px; border: none; border-radius: 5px; font-size: 16px; margin: 10px; cursor: pointer; }";
  html += ".warning { background: #fff3cd; color: #856404; padding: 15px; border-radius: 5px; margin: 15px 0; }";
  html += "</style></head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>🌐 ESP8266 Bridge Test</h1>";
  html += "<p>This tests if ESP8266 can provide internet access.</p>";
  
  html += "<div class='warning'>";
  html += "<strong>Test Steps:</strong><br>";
  html += "1. Try clicking the button below<br>";
  html += "2. Try opening google.com in a new tab<br>";
  html += "3. If both fail, ESP8266 bridge doesn't work";
  html += "</div>";
  
  html += "<button class='test-btn' onclick='testInternet()'>Test Internet Access</button>";
  html += "<br><br>";
  html += "<div id='result'></div>";
  
  html += "<script>";
  html += "function testInternet() {";
  html += "  document.getElementById('result').innerHTML = 'Testing...';";
  html += "  fetch('https://httpbin.org/ip')";
  html += "    .then(response => response.json())";
  html += "    .then(data => {";
  html += "      document.getElementById('result').innerHTML = '<p style=\"color: green;\">✅ Internet works! Your IP: ' + data.origin + '</p>';";
  html += "    })";
  html += "    .catch(error => {";
  html += "      document.getElementById('result').innerHTML = '<p style=\"color: red;\">❌ No internet access. Error: ' + error.message + '</p>';";
  html += "    });";
  html += "}";
  html += "</script>";
  html += "</div>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleTest() {
  server.send(200, "text/plain", "ESP8266 web server is working!");
} 