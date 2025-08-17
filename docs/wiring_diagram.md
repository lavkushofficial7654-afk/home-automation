# Hardware Wiring Diagram

## ESP8266 to Breadboard Connections

### Basic LED Control Setup

```
ESP8266 WiFi Module (NodeMCU/Wemos D1 Mini)
┌─────────────────────────────────────────┐
│                                         │
│  VIN (5V) ──────────────────────────────┤ 5V Power Supply
│  GND ───────────────────────────────────┤ Ground
│  GPIO2 ──────[220Ω Resistor]─────[LED]─┤ LED Anode (+)
│                                         │
│  GPIO0 ──────[220Ω Resistor]─────[LED]─┤ Status LED (optional)
│                                         │
│  GPIO4 ────────────────────────────────┤ Relay Module (optional)
│                                         │
└─────────────────────────────────────────┘
```

### Detailed Connection Guide

#### 1. Power Connections
- **VIN (5V)** → Breadboard 5V rail
- **GND** → Breadboard Ground rail
- **3.3V** → Breadboard 3.3V rail (if needed)

#### 2. LED Connections
- **GPIO2** → 220Ω Resistor → LED Anode (+)
- **LED Cathode (-)** → Ground
- **GPIO0** → 220Ω Resistor → Status LED Anode (+) (optional)
- **Status LED Cathode (-)** → Ground

#### 3. Relay Module (Optional)
- **GPIO4** → Relay Module Signal Pin
- **VIN (5V)** → Relay Module VCC
- **GND** → Relay Module GND

### Breadboard Layout

```
Breadboard Layout:
┌─────────────────────────────────────────────────────────┐
│ 5V Rail:  ────[VIN]──────────────────────────────────── │
│ 3.3V Rail: ────[3.3V]────────────────────────────────── │
│ GND Rail:  ────[GND]──────────────────────────────────── │
│                                                         │
│ GPIO2 ──[220Ω]──[LED+]──[LED-]──GND                    │
│ GPIO0 ──[220Ω]──[LED+]──[LED-]──GND (optional)         │
│ GPIO4 ───────────────────────────────────────────────── │
│                                                         │
│ Relay Module (optional):                                │
│ GPIO4 ──[Signal]  VIN ──[VCC]  GND ──[GND]             │
└─────────────────────────────────────────────────────────┘
```

### Component Specifications

#### LED
- **Type**: Any standard 5mm LED
- **Forward Voltage**: 2.0V - 3.3V
- **Forward Current**: 20mA
- **Resistor**: 220Ω (for 3.3V supply)

#### Resistor Calculation
For 3.3V supply and 20mA LED current:
- Voltage drop across LED: ~2V
- Voltage drop across resistor: 3.3V - 2V = 1.3V
- Resistor value: 1.3V / 0.02A = 65Ω
- Use 220Ω for safety (standard value)

#### Relay Module (Optional)
- **Operating Voltage**: 5V
- **Control Signal**: 3.3V (ESP8266 compatible)
- **Max Current**: 10A (depends on module)

### Safety Notes

1. **Always use current-limiting resistors** with LEDs
2. **Check voltage levels** - ESP8266 operates at 3.3V
3. **Don't exceed maximum current** ratings
4. **Use proper power supply** for your ESP8266 board
5. **Double-check connections** before powering on

### Testing Connections

1. **Power Test**: LED should not light up until WiFi connects
2. **WiFi Connection**: Built-in LED (GPIO2) should turn on when connected
3. **Web Control**: External LED should respond to web commands
4. **Serial Monitor**: Check for connection messages at 115200 baud

### Troubleshooting

- **LED doesn't light**: Check resistor and LED orientation
- **WiFi won't connect**: Verify credentials and signal strength
- **Web interface not accessible**: Check IP address in Serial Monitor
- **Relay not working**: Verify power supply and connections 