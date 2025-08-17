# WiFi Module LED Control Project

This project demonstrates how to control an LED on a breadboard using an ESP8266 WiFi module. When the WiFi module connects to the internet, it blinks an LED and provides web-based control.

## Hardware Requirements

### Components Needed:
1. **ESP8266 WiFi Module** (NodeMCU, Wemos D1 Mini, or similar)
2. **Breadboard**
3. **LED** (any color)
4. **220Ω Resistor** (for LED current limiting)
5. **Jumper Wires**
6. **USB Cable** (for programming ESP8266)
7. **Power Supply** (5V for NodeMCU, 3.3V for some ESP8266 modules)

### Optional Components:
- **Relay Module** (for controlling higher voltage devices)
- **Additional LEDs** for status indication

## Software Requirements

### Development Environment:
1. **Arduino IDE** (with ESP8266 board support)
2. **ESP8266 Board Package** (version 2.7.4 or later)
3. **Required Libraries:**
   - ESP8266WiFi (included with ESP8266 board package)
   - ESP8266WebServer (included with ESP8266 board package)

## Project Structure

```
wifi-program/
├── README.md                 # This file
├── src/
│   ├── wifi_led_control.ino  # Main Arduino sketch
│   └── web_interface.ino     # Enhanced web interface version
├── docs/
│   ├── wiring_diagram.md     # Hardware connection guide
│   └── troubleshooting.md    # Common issues and solutions
├── config/
│   └── wifi_config.h         # WiFi credentials (create this)
└── tools/
    └── upload_script.sh      # Automated upload script
```

## Quick Start Guide

1. **Hardware Setup:**
   - Connect LED to GPIO2 (with 220Ω resistor)
   - Connect power and ground
   - See `docs/wiring_diagram.md` for detailed connections

2. **Software Setup:**
   - Install Arduino IDE
   - Add ESP8266 board support
   - Copy WiFi credentials to `config/wifi_config.h`

3. **Upload Code:**
   - Open `src/wifi_led_control.ino` in Arduino IDE
   - Select correct board and port
   - Upload the code

4. **Test:**
   - Monitor Serial output (115200 baud)
   - LED should blink when WiFi connects
   - Access web interface at ESP8266's IP address

## Features

- **WiFi Connection Indicator:** LED blinks when connecting, stays on when connected
- **Web Control Interface:** Control LED via web browser
- **Serial Monitoring:** Debug information via Serial Monitor
- **Relay Control:** Optional relay module support for higher voltage devices

## Usage

1. **Power on the ESP8266**
2. **Wait for WiFi connection** (LED will blink during connection)
3. **Note the IP address** from Serial Monitor
4. **Open web browser** and navigate to the IP address
5. **Use web interface** to control the LED

## Troubleshooting

See `docs/troubleshooting.md` for common issues and solutions.

## License

This project is open source and available under the MIT License. 