# Troubleshooting Guide

## Common Issues and Solutions

### 1. WiFi Connection Problems

#### Issue: ESP8266 won't connect to WiFi
**Symptoms:**
- LED keeps blinking indefinitely
- Serial Monitor shows "WiFi connection failed"
- No IP address assigned

**Solutions:**
1. **Check WiFi credentials**
   - Verify SSID and password in `config/wifi_config.h`
   - Ensure no extra spaces or special characters
   - Try connecting with your phone/computer to verify network

2. **Check WiFi signal strength**
   - Move ESP8266 closer to router
   - Check if router supports 2.4GHz (ESP8266 doesn't support 5GHz)
   - Try different WiFi channels

3. **Check router settings**
   - Ensure router allows new devices
   - Check if MAC filtering is enabled
   - Verify router isn't blocking ESP8266

4. **Hardware issues**
   - Check power supply (ESP8266 needs stable 3.3V)
   - Try different USB cable
   - Check if ESP8266 is damaged

#### Issue: WiFi disconnects frequently
**Solutions:**
1. **Add WiFi reconnection code**
2. **Check power supply stability**
3. **Move closer to router**
4. **Reduce WiFi power consumption**

### 2. LED Control Issues

#### Issue: LED doesn't light up
**Symptoms:**
- LED remains off even when WiFi connects
- No response to web commands

**Solutions:**
1. **Check wiring**
   - Verify LED orientation (longer leg = anode/+)
   - Ensure 220Ω resistor is connected
   - Check all connections are secure

2. **Check GPIO pins**
   - Verify correct pin assignments in code
   - Some ESP8266 boards have different pin mappings
   - Check if pin is being used by other functions

3. **Test with simple code**
   ```cpp
   void setup() {
     pinMode(2, OUTPUT);
   }
   void loop() {
     digitalWrite(2, HIGH);
     delay(1000);
     digitalWrite(2, LOW);
     delay(1000);
   }
   ```

#### Issue: LED flickers or behaves erratically
**Solutions:**
1. **Check power supply**
   - Ensure adequate current (ESP8266 needs ~200mA)
   - Use external power supply if using USB
   - Add capacitors for stability

2. **Check code logic**
   - Ensure no conflicting pin operations
   - Check for infinite loops
   - Verify delay() functions

### 3. Web Interface Issues

#### Issue: Can't access web interface
**Symptoms:**
- Browser shows "connection refused"
- No response from ESP8266 IP address

**Solutions:**
1. **Check IP address**
   - Note IP address from Serial Monitor
   - Ensure you're on same network
   - Try pinging the IP address

2. **Check firewall settings**
   - Disable firewall temporarily
   - Check router settings
   - Try different browser

3. **Check web server code**
   - Verify server.begin() is called
   - Check port 80 isn't blocked
   - Ensure proper route handlers

#### Issue: Web interface loads but controls don't work
**Solutions:**
1. **Check JavaScript errors**
   - Open browser developer tools
   - Look for console errors
   - Check network tab for failed requests

2. **Verify route handlers**
   - Check Serial Monitor for request logs
   - Ensure all routes are properly defined
   - Test with simple GET requests

### 4. Serial Communication Issues

#### Issue: No output in Serial Monitor
**Symptoms:**
- Serial Monitor shows nothing
- Can't see debug messages

**Solutions:**
1. **Check baud rate**
   - Set Serial Monitor to 115200 baud
   - Ensure code uses same baud rate
   - Try different baud rates

2. **Check USB connection**
   - Try different USB cable
   - Check USB port
   - Install proper USB drivers

3. **Check board selection**
   - Select correct ESP8266 board in Arduino IDE
   - Verify COM port selection
   - Try uploading basic sketch first

### 5. Upload Issues

#### Issue: Can't upload code to ESP8266
**Symptoms:**
- Upload fails with timeout
- "Connection failed" errors
- Board not recognized

**Solutions:**
1. **Check board settings**
   - Select correct board (NodeMCU 1.0, Wemos D1 Mini, etc.)
   - Set correct upload speed (115200 or 921600)
   - Choose correct COM port

2. **Enter upload mode**
   - Hold FLASH button while pressing RESET
   - Release RESET, then release FLASH
   - Try automatic upload mode

3. **Check drivers**
   - Install CH340 or CP2102 drivers
   - Update Arduino IDE
   - Try different USB cable

### 6. Power Issues

#### Issue: ESP8266 resets randomly
**Symptoms:**
- Device restarts unexpectedly
- WiFi disconnects frequently
- Unpredictable behavior

**Solutions:**
1. **Check power supply**
   - Use 5V/1A or higher power supply
   - Add 100-1000μF capacitor across power
   - Use shorter, thicker wires

2. **Reduce power consumption**
   - Disable WiFi sleep mode
   - Reduce Serial output frequency
   - Use deep sleep when possible

3. **Check voltage levels**
   - Measure voltage at ESP8266 pins
   - Ensure stable 3.3V supply
   - Check for voltage drops

### 7. Code Issues

#### Issue: Code compiles but doesn't work as expected
**Solutions:**
1. **Check variable scope**
   - Ensure variables are declared properly
   - Check for naming conflicts
   - Verify data types

2. **Check function calls**
   - Ensure all functions are defined
   - Check parameter types
   - Verify return values

3. **Debug with Serial output**
   - Add Serial.println() statements
   - Check variable values
   - Monitor program flow

### 8. Hardware Compatibility

#### Issue: Code works on one board but not another
**Solutions:**
1. **Check pin mappings**
   - Different ESP8266 boards have different pin layouts
   - Verify GPIO numbers
   - Check built-in LED pin

2. **Check board specifications**
   - Verify flash memory size
   - Check available GPIO pins
   - Ensure compatibility

### General Debugging Tips

1. **Start simple**
   - Test with basic WiFi connection first
   - Add features one by one
   - Use Serial Monitor extensively

2. **Check documentation**
   - Refer to ESP8266 documentation
   - Check library examples
   - Search for similar issues online

3. **Use debugging tools**
   - Serial Monitor for code debugging
   - Multimeter for voltage/current checks
   - Logic analyzer for signal analysis

4. **Keep backups**
   - Save working code versions
   - Document changes made
   - Test thoroughly before deployment 