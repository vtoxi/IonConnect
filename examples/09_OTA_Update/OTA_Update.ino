/**
 * @file OTA_Update.ino
 * @brief Over-The-Air firmware update example
 * 
 * This example demonstrates:
 * - Web-based OTA updates
 * - Secure update with authentication
 * - Update progress monitoring
 * - Update callbacks
 * - Rollback on failure
 * 
 * Hardware: ESP32 or ESP8266
 */

#include <IonConnect.h>

IonConnectDevice ion;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  IonConnect - OTA Update Example");
    Serial.println("========================================\n");
    
    Serial.print("Current Firmware Version: ");
    Serial.println(ION_VERSION_STRING);
    Serial.print("Chip: ");
    
    #if ION_PLATFORM_ESP32
    Serial.println("ESP32");
    Serial.print("Flash Size: ");
    Serial.print(ESP.getFlashChipSize() / 1024 / 1024);
    Serial.println(" MB");
    #else
    Serial.println("ESP8266");
    Serial.print("Sketch Size: ");
    Serial.print(ESP.getSketchSize() / 1024);
    Serial.println(" KB");
    Serial.print("Free Sketch Space: ");
    Serial.print(ESP.getFreeSketchSpace() / 1024);
    Serial.println(" KB");
    #endif
    
    Serial.println();
    
    // Configure IonConnect with OTA enabled
    IonConfig config;
    config.enableOTA = true;
    config.accessToken = "secure-ota-token";  // Require token for OTA
    
    ion.init("OTA-Device", config);
    
    // Register callbacks
    ion.onConnect([]() {
        Serial.println("\n✓ WiFi Connected!");
        Serial.print("  IP: ");
        Serial.println(WiFi.localIP());
        
        Serial.println("\n🔧 OTA Update Available:");
        Serial.println("  1. Web Interface:");
        Serial.println("     http://" + WiFi.localIP().toString() + "/update");
        Serial.println();
        Serial.println("  2. curl Command:");
        Serial.println("     curl -F 'update=@firmware.bin' \\");
        Serial.println("          -H 'Authorization: Bearer secure-ota-token' \\");
        Serial.println("          http://" + WiFi.localIP().toString() + "/update");
        Serial.println();
        Serial.println("⚠️  Authentication required (token: secure-ota-token)");
    });
    
    // OTA callbacks (if using custom OTA handler)
    // ion.onOTAStart([]() {
    //     Serial.println("\n🔄 OTA Update Started");
    //     Serial.println("  Please wait, do not power off...");
    // });
    
    // ion.onOTAProgress([](size_t current, size_t total) {
    //     static uint8_t lastPercent = 0;
    //     uint8_t percent = (current * 100) / total;
    //     
    //     if (percent != lastPercent && percent % 10 == 0) {
    //         Serial.print("  Progress: ");
    //         Serial.print(percent);
    //         Serial.println("%");
    //         lastPercent = percent;
    //     }
    // });
    
    // ion.onOTAEnd([]() {
    //     Serial.println("\n✓ OTA Update Complete!");
    //     Serial.println("  Device will reboot...");
    // });
    
    // ion.onOTAError([](int error) {
    //     Serial.print("\n✗ OTA Update Failed! Error: ");
    //     Serial.println(error);
    // });
    
    // Start IonConnect
    if (!ion.begin()) {
        Serial.println("✗ IonConnect initialization failed!");
        return;
    }
    
    Serial.println("✓ IonConnect started with OTA enabled");
}

void loop() {
    ion.handle();
    
    // Show uptime every minute
    static uint32_t lastUpdate = 0;
    if (millis() - lastUpdate > 60000) {
        lastUpdate = millis();
        
        Serial.print("⏱  Uptime: ");
        Serial.print(millis() / 1000);
        Serial.println(" seconds");
        
        if (ion.isConnected()) {
            Serial.print("📡 IP: ");
            Serial.println(WiFi.localIP());
        }
    }
}

/**
 * OTA Update Process:
 * 
 * 1. Preparation:
 *    - Build firmware (.bin file)
 *    - Test in development environment
 *    - Document version changes
 * 
 * 2. Upload Methods:
 *    a) Web Interface:
 *       - Navigate to http://device-ip/update
 *       - Select firmware.bin file
 *       - Click Upload
 *    
 *    b) Command Line:
 *       - Use curl or wget
 *       - Include auth token
 *       - Monitor progress
 *    
 *    c) Programmatic:
 *       - Use HTTP client library
 *       - Automate fleet updates
 *       - Implement staged rollout
 * 
 * 3. Security:
 *    - Always use authentication
 *    - Validate firmware before flashing
 *    - Use HTTPS in production
 *    - Implement version checks
 *    - Sign firmware images
 * 
 * 4. Error Handling:
 *    - Verify checksum
 *    - Check file size
 *    - Validate magic bytes
 *    - Rollback on failure
 *    - Log update attempts
 * 
 * 5. Best Practices:
 *    - Test updates thoroughly
 *    - Implement version tracking
 *    - Keep old firmware backup
 *    - Use staged deployment
 *    - Monitor update success rate
 *    - Implement automatic rollback
 *    - Notify users of updates
 *    - Schedule during low-usage
 * 
 * Building Firmware:
 * 
 * Arduino IDE:
 *   Sketch > Export Compiled Binary
 *   Look for .bin file in sketch folder
 * 
 * PlatformIO:
 *   pio run
 *   Firmware: .pio/build/[env]/firmware.bin
 * 
 * ESP-IDF:
 *   idf.py build
 *   Firmware: build/[project].bin
 */

