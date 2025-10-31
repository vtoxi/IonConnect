/**
 * @file BLEProvisioning.ino
 * @brief BLE provisioning example for ESP32
 * 
 * This example enables Bluetooth Low Energy provisioning, allowing users
 * to configure WiFi via a mobile app without connecting to the AP.
 * 
 * Use case: Devices in hard-to-reach locations, headless setup
 * 
 * Hardware: ESP32 ONLY (BLE not available on ESP8266)
 * 
 * NOTE: You'll need a mobile app that supports IonConnect BLE protocol
 * Service UUID: 0000ff01-0000-1000-8000-00805f9b34fb
 */

#include <IonConnect.h>

#if !ION_PLATFORM_ESP32
#error "This example requires ESP32 (BLE not available on ESP8266)"
#endif

IonConnectDevice ion;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  IonConnect - BLE Provisioning");
    Serial.println("========================================\n");
    
    // Configure IonConnect with BLE enabled
    IonConfig config;
    config.deviceName = "BLE-IoT-Device";
    config.enableBLE = true;              // Enable BLE provisioning
    config.autoStartPortal = true;        // Also start WiFi portal
    
    // Initialize
    ion.init("BLEDevice", config);
    
    Serial.println("✓ IonConnect with BLE enabled");
    Serial.println();
    Serial.println("📱 Provisioning Options:");
    Serial.println("  1. Via BLE: Look for 'BLE-IoT-Device' in Bluetooth scanner");
    Serial.println("  2. Via WiFi: Connect to 'IonConnect-XXXX' AP");
    Serial.println();
    
    // Register callbacks
    ion.onConnect([]() {
        Serial.println("\n✓ WiFi Connected via BLE!");
        Serial.print("  SSID: ");
        Serial.println(WiFi.SSID());
        Serial.print("  IP: ");
        Serial.println(WiFi.localIP());
        
        // You can disable BLE after successful connection to save power
        // ion.disableBLE();
    });
    
    ion.onDisconnect([]() {
        Serial.println("\n⚠ WiFi Disconnected");
        // BLE remains active for re-provisioning
    });
    
    // Start IonConnect
    if (!ion.begin()) {
        Serial.println("✗ IonConnect initialization failed!");
        return;
    }
    
    Serial.println("✓ IonConnect started");
    Serial.println("✓ BLE advertising started");
    Serial.println();
    Serial.println("🔍 BLE Service Details:");
    Serial.println("  Service UUID: 0000ff01-0000-1000-8000-00805f9b34fb");
    Serial.println("  Schema UUID:  0000ff02-0000-1000-8000-00805f9b34fb (Read)");
    Serial.println("  Config UUID:  0000ff03-0000-1000-8000-00805f9b34fb (Write)");
    Serial.println("  Status UUID:  0000ff04-0000-1000-8000-00805f9b34fb (Notify)");
    Serial.println("  Control UUID: 0000ff05-0000-1000-8000-00805f9b34fb (Write)");
}

void loop() {
    ion.handle();
    
    // Show status every minute
    static uint32_t lastStatus = 0;
    if (millis() - lastStatus > 60000) {
        lastStatus = millis();
        
        Serial.println("\n📊 Status:");
        if (ion.isConnected()) {
            Serial.println("  WiFi: ✓ Connected");
            Serial.print("  SSID: ");
            Serial.println(WiFi.SSID());
            Serial.print("  IP: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("  WiFi: ✗ Not connected");
            Serial.println("  Waiting for BLE or WiFi provisioning...");
        }
    }
    
    // Your application logic here
}

