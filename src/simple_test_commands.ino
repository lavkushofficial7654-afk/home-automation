/*
 * Simple Test Commands - Basic Serial Communication
 * 
 * This is a minimal sketch to test if serial commands work
 * No WiFi, no ultrasonic - just basic LED control
 */

// Pin Definition
const int LED_PIN = 2;  // D4 (GPIO2)

// Global Variables
bool ledState = false;

void setup() {
  Serial.begin(9600);
  delay(3000);
  
  Serial.println("=====================================");
  Serial.println("Simple Test Commands");
  Serial.println("=====================================");
  
  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  ledState = false;
  
  Serial.println("✅ System initialized");
  Serial.println("Type 'on', 'off', 'status', or 'help'");
  Serial.println("=====================================");
}

void loop() {
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
      Serial.println("🔴 LED ON");
    }
    else if (command == "off") {
      digitalWrite(LED_PIN, LOW);
      ledState = false;
      Serial.println("⚫ LED OFF");
    }
    else if (command == "status") {
      Serial.println("=== STATUS ===");
      Serial.print("LED: ");
      Serial.println(ledState ? "ON" : "OFF");
      Serial.println("==============");
    }
    else if (command == "help") {
      Serial.println("Available commands:");
      Serial.println("  on - Turn LED ON");
      Serial.println("  off - Turn LED OFF");
      Serial.println("  status - Show status");
      Serial.println("  help - Show this help");
    }
    else {
      Serial.println("Unknown command. Type 'help' for available commands.");
    }
  }
  
  delay(100);
} 