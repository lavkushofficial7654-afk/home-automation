/*
 * WiFi Configuration File
 * 
 * Update these values with your WiFi network credentials
 * Make sure to keep this file secure and don't share it publicly
 */

#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

// WiFi Network Credentials
const char* WIFI_SSID = "Lav";
const char* WIFI_PASSWORD = "harharmahadev@468";

// WiFi Connection Settings
const int WIFI_TIMEOUT = 20000;  // 20 seconds timeout
const int WIFI_RETRY_DELAY = 500; // 500ms between retry attempts

// Optional: Static IP Configuration (comment out to use DHCP)
// const IPAddress STATIC_IP(192, 168, 1, 100);
// const IPAddress GATEWAY(192, 168, 1, 1);
// const IPAddress SUBNET(255, 255, 255, 0);
// const IPAddress DNS(8, 8, 8, 8);

#endif // WIFI_CONFIG_H 