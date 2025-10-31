#ifndef ION_CONFIG_H
#define ION_CONFIG_H

#include <Arduino.h>
#include <IPAddress.h>

namespace IonConnect {

struct IonConfig {
    // Portal Settings
    String apSSID = "";                     // Auto-generated if empty: IonConnect-XXXX
    String apPassword = "";                 // Open AP by default
    IPAddress apIP = IPAddress(192, 168, 4, 1);
    IPAddress apGateway = IPAddress(192, 168, 4, 1);
    IPAddress apSubnet = IPAddress(255, 255, 255, 0);
    
    // Portal Behavior
    uint32_t portalTimeoutSeconds = 300;    // 5 minutes default
    bool autoStartPortal = true;            // Start portal if no credentials
    bool blockingPortalMode = false;        // Allow app logic during portal
    
    // Connection Settings
    uint8_t maxReconnectAttempts = 5;
    uint32_t reconnectDelayMs = 1000;
    bool autoReconnect = true;
    uint32_t connectionTimeoutMs = 10000;   // 10 seconds
    
    // Features
    bool enableBLE = ION_ENABLE_BLE;        // ESP32 only
    bool enableOTA = ION_ENABLE_OTA;
    bool enableMDNS = ION_ENABLE_MDNS;
    bool enableDiagnostics = ION_ENABLE_DIAGNOSTICS;
    
    // Security
    String portalPassword = "";             // Empty = no auth
    String accessToken = "";                // For API access
    bool requireTokenForAPI = false;
    
    // UI
    String deviceName = "IonConnect Device";
    String deviceLogoURL = "";              // Optional logo URL
    bool enableDarkMode = true;
    
    // Advanced
    String hostname = "";                   // mDNS hostname (auto-generated if empty)
    bool enableCaptivePortal = true;        // DNS redirect
    uint16_t webServerPort = 80;
    
    IonConfig() {}
};

} // namespace IonConnect

#endif // ION_CONFIG_H

