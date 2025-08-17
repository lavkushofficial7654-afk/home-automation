# Alka Voice Assistant for ESP8266 LED Control

A voice assistant that lets you control your ESP8266 LED using voice commands. Just say "Hey Alka" followed by your command!

## Features

- Voice command recognition using Web Speech API (works in Chrome/Edge)
- Real-time LED control via ESP8266
- Beautiful web interface with status indicators
- Command logging and error reporting

## Prerequisites

1. Node.js installed on your computer
2. Google Chrome or Microsoft Edge browser (for voice recognition)
3. ESP8266 running the LED control server (on IP 192.168.4.1)

## Setup

1. Install Node.js dependencies:
   ```bash
   npm install
   ```

2. Make sure your ESP8266 is running and creating the WiFi network "ESP8266_LED_Control"

3. Start the server:
   ```bash
   npm start
   ```

4. Open Chrome or Edge browser and visit:
   ```
   http://localhost:3000
   ```

## Usage

1. Connect your computer to the ESP8266's WiFi network (ESP8266_LED_Control)
2. Open the web interface in Chrome/Edge
3. Allow microphone access when prompted
4. Click "Start Listening"
5. Say commands like:
   - "Hey Alka, switch on the light"
   - "Hey Alka, turn off the light"
   - "Hey Alka, lights on"
   - "Hey Alka, lights off"

## Troubleshooting

1. If the microphone isn't working:
   - Make sure you're using Chrome or Edge browser
   - Check browser microphone permissions
   - Try refreshing the page

2. If commands aren't recognized:
   - Speak clearly and wait for the "listening" indicator
   - Make sure you're starting with "Hey Alka"
   - Check that your microphone is working in other apps

3. If LED control isn't working:
   - Verify you're connected to the ESP8266's WiFi network
   - Check if the ESP8266's web interface works directly
   - Verify the ESP8266's IP address (default: 192.168.4.1)

## How it Works

This project uses:
- Web Speech API for voice recognition (built into Chrome/Edge)
- Socket.IO for real-time communication
- Express for the web server
- Simple HTTP requests to control the ESP8266

No external speech recognition services or API keys required! ��

## License

ISC 