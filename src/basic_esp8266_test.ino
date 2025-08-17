/*
 * Basic ESP8266 Test
 * 
 * This is a minimal sketch to test if ESP8266 is working
 * Just blinks the built-in LED and prints to Serial
 */

// Built-in LED pin (usually GPIO2 on most ESP8266 boards)
const int LED_PIN = 2;

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);
  
  // Wait for Serial to be ready
  delay(1000);
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  
  // Print startup message
  Serial.println("=====================================");
  Serial.println("Basic ESP8266 Test");
  Serial.println("=====================================");
  Serial.println("If you see this message, ESP8266 is working!");
  Serial.println("Built-in LED should blink every 2 seconds");
  Serial.println("=====================================");
}

void loop() {
  // Turn LED ON
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ON");
  delay(1000);
  
  // Turn LED OFF
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED OFF");
  delay(1000);
} 