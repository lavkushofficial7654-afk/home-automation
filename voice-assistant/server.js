const express = require('express');
const fetch = require('node-fetch');
const path = require('path');

const app = express();

// ESP8266 Configuration - guest network IP
const ESP_IPS = ['http://192.168.0.101'];

// Serve static files
app.use(express.static(__dirname));

// Function to try ESP8266 at different IPs
async function tryESP8266(action) {
    for (const espIP of ESP_IPS) {
        try {
            console.log(`Trying ESP8266 at: ${espIP}/${action}`);
            const response = await fetch(`${espIP}/${action}`);
            if (response.ok) {
                console.log(`ESP8266 responded from: ${espIP}`);
                return { success: true, ip: espIP };
            }
        } catch (error) {
            console.log(`Failed to connect to ${espIP}: ${error.message}`);
        }
    }
    return { success: false, error: 'No ESP8266 found' };
}

// API endpoint for ESP8266 control
app.get('/api/control/:action', async (req, res) => {
    const action = req.params.action;
    console.log(`API request to control ESP8266: ${action}`);
    
    const result = await tryESP8266(action);
    
    if (result.success) {
        res.json({ success: true, message: `LED turned ${action.toUpperCase()}`, ip: result.ip });
    } else {
        res.status(500).json({ success: false, error: result.error });
    }
});

// Start server
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
    console.log(`🚀 Voice Assistant Server running on http://localhost:${PORT}`);
    console.log(`📡 ESP8266 IPs to try: ${ESP_IPS.join(', ')}`);
    console.log(`🌐 Make sure you're connected to "Lav" WiFi for internet access`);
}); 