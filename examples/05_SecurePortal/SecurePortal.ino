/**
 * @file SecurePortal.ino
 * @brief Secure captive portal with password protection
 * 
 * This example demonstrates portal security features:
 * - Portal password protection
 * - API access token
 * - Portal timeout
 * 
 * Use case: Devices in public spaces, commercial products
 * 
 * Hardware: ESP32 or ESP8266
 */

#include <IonConnect.h>

IonConnect ion;

// Security configuration
const char* PORTAL_PASSWORD = "admin123";           // Portal access password
const char* API_TOKEN = "your-secret-token-here";   // API access token
const uint32_t PORTAL_TIMEOUT = 300;                // 5 minutes timeout

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  IonConnect - Secure Portal Example");
    Serial.println("========================================\n");
    
    // Configure IonConnect with security
    IonConfig config;
    config.deviceName = "Secure Device";
    config.portalPassword = PORTAL_PASSWORD;        // Require password for portal
    config.accessToken = API_TOKEN;                 // Require token for API
    config.requireTokenForAPI = true;               // Enforce token
    config.portalTimeoutSeconds = PORTAL_TIMEOUT;   // Auto-close portal
    
    // Initialize
    ion.init("SecureDevice", config);
    
    Serial.println("🔒 Security Configuration:");
    Serial.println("  Portal Password: " + String(PORTAL_PASSWORD));
    Serial.println("  API Token: " + String(API_TOKEN));
    Serial.println("  Portal Timeout: " + String(PORTAL_TIMEOUT) + "s");
    Serial.println();
    Serial.println("⚠ IMPORTANT: Change default passwords before deployment!");
    Serial.println();
    
    // Register callbacks
    ion.onConnect([]() {
        Serial.println("\n✓ WiFi Connected!");
        Serial.print("  IP: ");
        Serial.println(WiFi.localIP());
        
        Serial.println("\n🔒 API Access:");
        Serial.println("  Requires header: Authorization: Bearer " + String(API_TOKEN));
        Serial.println("  Example: curl -H 'Authorization: Bearer " + String(API_TOKEN) + 
                      "' http://" + WiFi.localIP().toString() + "/api/status");
    });
    
    ion.onPortalStart([]() {
        Serial.println("\n🌐 Portal Started");
        Serial.println("  SSID: IonConnect-XXXX");
        Serial.println("  Password required: " + String(PORTAL_PASSWORD));
        Serial.println("  Timeout: " + String(PORTAL_TIMEOUT) + " seconds");
    });
    
    ion.onPortalTimeout([]() {
        Serial.println("\n⏱ Portal Timeout!");
        Serial.println("  Portal closed due to inactivity");
        
        // Option 1: Restart portal
        // ion.startPortal();
        
        // Option 2: Continue without WiFi
        // (device continues running)
    });
    
    // You can also set security dynamically
    // ion.setPortalPassword("newpassword");
    // ion.setAccessToken("new-token");
    
    // Start IonConnect
    if (!ion.begin()) {
        Serial.println("✗ IonConnect initialization failed!");
        return;
    }
    
    Serial.println("✓ IonConnect started with security enabled");
}

void loop() {
    ion.handle();
    
    // Monitor security events
    static bool portalWasActive = false;
    bool portalActive = ion.isPortalActive();
    
    if (portalActive != portalWasActive) {
        portalWasActive = portalActive;
        
        if (portalActive) {
            Serial.println("\n🔓 Portal is ACTIVE - Accepting connections");
        } else {
            Serial.println("\n🔒 Portal is CLOSED");
        }
    }
    
    // Your application logic here
    if (ion.isConnected()) {
        // Do secure cloud communication
    }
}

/**
 * Production Security Best Practices:
 * 
 * 1. Change Default Passwords:
 *    - Never use default passwords in production
 *    - Generate unique passwords per device
 * 
 * 2. Secure Token Storage:
 *    - Store tokens in secure element if available
 *    - Rotate tokens periodically
 * 
 * 3. Network Security:
 *    - Use WPA2 or WPA3 for AP mode
 *    - Enable HTTPS for API (advanced)
 * 
 * 4. Portal Timeout:
 *    - Always set reasonable timeout
 *    - Close portal after successful configuration
 * 
 * 5. Access Logging:
 *    - Monitor API access patterns
 *    - Alert on suspicious activity
 */

