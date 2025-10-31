/**
 * @file MultiNetwork.ino
 * @brief Multi-network WiFi management example
 * 
 * This example shows how to pre-configure multiple WiFi networks.
 * The device will automatically connect to the strongest available network.
 * 
 * Use case: Device that moves between home and office
 * 
 * Hardware: ESP32 or ESP8266
 */

#include <IonConnect.h>

IonConnect ion;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  IonConnect - Multi-Network Example");
    Serial.println("========================================\n");
    
    // Initialize IonConnect
    ion.init("MultiNetDevice");
    
    // Pre-configure multiple networks (priority: higher = preferred)
    // These will be saved to persistent storage
    ion.addNetwork("HomeNetwork", "homepassword123", 10);      // Highest priority
    ion.addNetwork("OfficeWiFi", "officepass456", 5);          // Medium priority
    ion.addNetwork("MobileHotspot", "mobilepass789", 1);       // Lowest priority
    
    Serial.println("✓ Pre-configured networks:");
    Serial.println("  1. HomeNetwork (priority: 10)");
    Serial.println("  2. OfficeWiFi (priority: 5)");
    Serial.println("  3. MobileHotspot (priority: 1)");
    Serial.println();
    
    // Register callbacks
    ion.onConnect([]() {
        Serial.println("\n✓ WiFi Connected!");
        Serial.print("  SSID: ");
        Serial.println(WiFi.SSID());
        Serial.print("  IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("  RSSI: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    });
    
    ion.onDisconnect([]() {
        Serial.println("\n⚠ WiFi Disconnected");
        Serial.println("  Will auto-reconnect...");
    });
    
    // Start IonConnect
    // It will scan for all pre-configured networks and connect to the strongest
    if (!ion.begin()) {
        Serial.println("✗ IonConnect initialization failed!");
        return;
    }
    
    Serial.println("✓ IonConnect started");
    Serial.println("  Scanning for known networks...");
}

void loop() {
    ion.handle();
    
    // Monitor connection status
    static uint32_t lastCheck = 0;
    if (millis() - lastCheck > 30000) { // Every 30 seconds
        lastCheck = millis();
        
        if (ion.isConnected()) {
            Serial.println("\n📊 Status Update:");
            Serial.print("  Connected to: ");
            Serial.println(WiFi.SSID());
            Serial.print("  Signal: ");
            Serial.print(WiFi.RSSI());
            Serial.println(" dBm");
            Serial.print("  Uptime: ");
            Serial.print(millis() / 1000);
            Serial.println(" seconds");
        } else {
            Serial.println("\n⚠ Not connected - waiting for reconnection...");
        }
    }
    
    // Your application logic here
    if (ion.isConnected()) {
        // Do cloud communication, etc.
    }
}

