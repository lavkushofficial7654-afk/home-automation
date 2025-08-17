# Complete Setup Guide

## Step-by-Step Instructions for WiFi LED Control Project

### Prerequisites

Before starting, ensure you have:
- ESP8266 WiFi module (NodeMCU, Wemos D1 Mini, etc.)
- Breadboard
- LED (any color)
- 220Ω resistor
- Jumper wires
- USB cable
- Computer with internet connection

### Step 1: Hardware Setup

#### 1.1 Basic Connections
1. **Power the ESP8266:**
   - Connect VIN (5V) to breadboard 5V rail
   - Connect GND to breadboard ground rail

2. **Connect LED:**
   - Connect GPIO2 to 220Ω resistor
   - Connect resistor to LED anode (longer leg)
   - Connect LED cathode (shorter leg) to ground

3. **Optional - Status LED:**
   - Connect GPIO0 to 220Ω resistor
   - Connect resistor to second LED anode
   - Connect LED cathode to ground

#### 1.2 Verify Connections
- Double-check all connections
- Ensure LED orientation is correct
- Verify resistor is connected in series with LED

### Step 2: Software Setup

#### 2.1 Install Arduino IDE
1. Download Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software)
2. Install and launch Arduino IDE

#### 2.2 Add ESP8266 Board Support
1. Open Arduino IDE
2. Go to **File > Preferences**
3. In "Additional Board Manager URLs" add:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Click OK
5. Go to **Tools > Board > Boards Manager**
6. Search for "ESP8266"
7. Install "ESP8266 by ESP8266 Community"
8. Select your board: **Tools > Board > ESP8266 Boards > NodeMCU 1.0**

#### 2.3 Configure Board Settings
1. **Board:** NodeMCU 1.0 (or your specific board)
2. **Upload Speed:** 115200
3. **Port:** Select the port where ESP8266 is connected
4. **CPU Frequency:** 80 MHz
5. **Flash Frequency:** 40 MHz
6. **Flash Mode:** DIO
7. **Flash Size:** 4MB (FS:2MB OTA:~1019KB)

### Step 3: Upload Test Code

#### 3.1 Test Basic Functionality
1. Open `src/simple_test.ino` in Arduino IDE
2. Update WiFi credentials if needed:
   ```cpp
   const char* ssid = "YOUR_WIFI_NAME";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
3. Click **Upload** button
4. Monitor Serial output (Tools > Serial Monitor, 115200 baud)

#### 3.2 Verify Test Results
- LED should turn on when WiFi connects
- Serial Monitor should show IP address
- Open web browser and navigate to the IP address
- Test LED control buttons on web page

### Step 4: Upload Main Program

#### 4.1 Upload Main Sketch
1. Open `src/wifi_led_control.ino` in Arduino IDE
2. Update WiFi credentials in the code:
   ```cpp
   const char* ssid = "Lav";
   const char* password = "harharmahadev@468";
   ```
3. Click **Upload** button
4. Monitor Serial output

#### 4.2 Test Main Program
- LED should blink during WiFi connection
- LED should stay on when connected
- Web interface should be accessible
- Test all control functions

### Step 5: Using the Web Interface

#### 5.1 Access Control Panel
1. Note the IP address from Serial Monitor
2. Open web browser
3. Navigate to `http://[IP_ADDRESS]/`
4. You should see the control panel

#### 5.2 Available Commands
- **LED ON/OFF:** Control the main LED
- **TOGGLE LED:** Switch LED state
- **RELAY ON/OFF:** Control relay module (if connected)
- **Status:** View system information

### Step 6: Troubleshooting

#### 6.1 Common Issues

**WiFi Connection Fails:**
- Check WiFi credentials
- Ensure router supports 2.4GHz
- Move ESP8266 closer to router
- Check power supply

**LED Doesn't Light:**
- Check LED orientation
- Verify resistor connection
- Test with simple blink code
- Check GPIO pin assignment

**Web Interface Not Accessible:**
- Check IP address in Serial Monitor
- Ensure you're on same network
- Try different browser
- Check firewall settings

**Upload Fails:**
- Hold FLASH button while pressing RESET
- Release RESET, then release FLASH
- Try different USB cable
- Check board selection

#### 6.2 Using the Upload Script
1. Install arduino-cli:
   ```bash
   # macOS
   brew install arduino-cli
   
   # Linux
   curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
   ```

2. Use the upload script:
   ```bash
   cd tools
   ./upload_script.sh
   ```

### Step 7: Advanced Features

#### 7.1 Adding Relay Module
1. Connect relay module:
   - GPIO4 → Signal pin
   - VIN (5V) → VCC
   - GND → GND

2. Test relay control via web interface

#### 7.2 Customizing the Code
1. Modify pin assignments in the code
2. Add additional sensors or actuators
3. Customize web interface
4. Add authentication

#### 7.3 Power Supply Options
- **USB Power:** Good for development
- **External 5V:** Better for production
- **Battery Power:** Add voltage regulator

### Step 8: Production Deployment

#### 8.1 Enclosure
- Choose appropriate enclosure
- Ensure proper ventilation
- Secure connections

#### 8.2 Power Supply
- Use stable 5V power supply
- Add power indicator LED
- Consider backup power

#### 8.3 Network Security
- Change default passwords
- Use WPA2 encryption
- Consider static IP assignment
- Monitor network traffic

### Step 9: Monitoring and Maintenance

#### 9.1 Regular Checks
- Monitor WiFi signal strength
- Check for firmware updates
- Verify all connections
- Test backup systems

#### 9.2 Logging
- Enable Serial logging for debugging
- Consider adding SD card logging
- Monitor system uptime
- Track usage patterns

### Success Criteria

Your project is working correctly when:
- ✅ ESP8266 connects to WiFi automatically
- ✅ LED indicates connection status
- ✅ Web interface is accessible
- ✅ LED responds to web commands
- ✅ Serial Monitor shows proper output
- ✅ System remains stable over time

### Next Steps

Once basic functionality is working:
1. Add more sensors (temperature, humidity, etc.)
2. Implement mobile app control
3. Add cloud integration
4. Create automation rules
5. Scale to multiple devices

### Support Resources

- **Documentation:** Check `docs/` folder
- **Troubleshooting:** See `docs/troubleshooting.md`
- **Community:** Arduino and ESP8266 forums
- **Examples:** Arduino IDE examples folder 