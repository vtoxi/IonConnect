/**
 * @file BasicSetup.ino
 * @brief Minimal IonConnect setup example
 * 
 * This example demonstrates the simplest possible IonConnect configuration.
 * The library will:
 * 1. Try to connect to saved WiFi credentials
 * 2. If no credentials or connection fails, start captive portal
 * 3. User configures WiFi via browser
 * 4. Device connects and runs your application code
 * 
 * Hardware: ESP32 or ESP8266
 * 
 * Instructions:
 * 1. Upload this sketch
 * 2. Connect to "IonConnect-XXXX" WiFi network
 * 3. Browser should open automatically (or go to 192.168.4.1)
 * 4. Enter your WiFi credentials
 * 5. Device connects and shows IP in serial monitor
 */

#include <IonConnect.h>

// Create IonConnect instance
IonConnect ion;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  IonConnect - Basic Setup Example");
    Serial.println("========================================\n");
    
    // Initialize with device name
    ion.init("MyIoTDevice");
    
    // Register connection callback
    ion.onConnect([]() {
        Serial.println("\n✓ WiFi Connected!");
        Serial.print("  IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.print("  Signal: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    });
    
    // Register portal start callback
    ion.onPortalStart([]() {
        Serial.println("\n→ Captive Portal Started");
        Serial.println("  Connect to: IonConnect-XXXX");
        Serial.println("  Go to: 192.168.4.1");
    });
    
    // Start IonConnect
    // This will either connect to saved WiFi or start portal
    if (!ion.begin()) {
        Serial.println("✗ IonConnect initialization failed!");
        return;
    }
    
    Serial.println("✓ IonConnect initialized");
}

void loop() {
    // Must call handle() to process async events
    ion.handle();
    
    // Your application code here
    // This runs whether connected or in portal mode
    
    // Example: Blink LED when connected
    static uint32_t lastBlink = 0;
    if (ion.isConnected() && millis() - lastBlink > 1000) {
        lastBlink = millis();
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
}

