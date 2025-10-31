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
#if ION_MINIMAL_MODE
    uint32_t portalTimeoutSeconds = 180;    // 3 minutes for minimal mode
    bool autoStartPortal = true;            
    bool blockingPortalMode = false;       
#else
    uint32_t portalTimeoutSeconds = 300;    // 5 minutes default
    bool autoStartPortal = true;            
    bool blockingPortalMode = false;       
#endif
    
    // Connection Settings
#if ION_MINIMAL_MODE
    uint8_t maxReconnectAttempts = 3;       // Reduced for minimal mode
    uint32_t reconnectDelayMs = 2000;       // Longer delay to save power
    bool autoReconnect = true;
    uint32_t connectionTimeoutMs = 15000;   // 15 seconds
#else
    uint8_t maxReconnectAttempts = 5;
    uint32_t reconnectDelayMs = 1000;
    bool autoReconnect = true;
    uint32_t connectionTimeoutMs = 10000;   
#endif
    
    // Features (automatically set by ION_MINIMAL_MODE in IonTypes.h)
    bool enableBLE = ION_ENABLE_BLE;        
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
    
    // Factory method for minimal ESP-01 configuration
    static IonConfig minimal() {
        IonConfig cfg;
        cfg.portalTimeoutSeconds = 180;
        cfg.maxReconnectAttempts = 3;
        cfg.reconnectDelayMs = 2000;
        cfg.connectionTimeoutMs = 15000;
        cfg.enableBLE = false;
        cfg.enableOTA = false;
        cfg.enableMDNS = false;
        cfg.enableDiagnostics = false;
        cfg.enableCaptivePortal = true;  // Still useful for low-mem
        return cfg;
    }
};

} // namespace IonConnect

#endif // ION_CONFIG_H

