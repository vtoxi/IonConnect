/**
 * @file BackupRestore.ino
 * @brief Configuration backup and restore example
 * 
 * This example demonstrates:
 * - Exporting configuration to JSON
 * - Importing configuration from JSON
 * - Saving/loading config via serial
 * - Factory reset functionality
 * 
 * Hardware: ESP32 or ESP8266
 */

#include <IonConnect.h>

IonConnectDevice ion;

// Forward declarations
void printHelp();
void processSerialCommand();

// Serial commands
void processSerialCommand() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        
        if (cmd == "export") {
            // Export configuration to JSON
            Serial.println("\n📥 Exporting configuration...");
            String config = ion.exportConfig();
            Serial.println(config);
            Serial.println("\n✓ Configuration exported (copy and save this JSON)");
            
        } else if (cmd.startsWith("import ")) {
            // Import configuration from JSON
            String json = cmd.substring(7);
            Serial.println("\n📤 Importing configuration...");
            
            if (ion.importConfig(json)) {
                Serial.println("✓ Configuration imported successfully!");
                Serial.println("  Device will reconnect with new settings...");
            } else {
                Serial.println("✗ Failed to import configuration");
            }
            
        } else if (cmd == "clear") {
            // Clear all configuration
            Serial.println("\n🗑 Clearing all configuration...");
            if (ion.clearConfig()) {
                Serial.println("✓ Configuration cleared");
                Serial.println("  Device will restart...");
                delay(1000);
                ESP.restart();
            }
            
        } else if (cmd == "show") {
            // Show current configuration
            Serial.println("\n📋 Current Configuration:");
            Serial.println("  SSID: " + ion.getConfig("wifi_ssid"));
            Serial.println("  Device Name: " + ion.getConfig("device_name"));
            
            if (ion.isConnected()) {
                Serial.println("\n📡 Connection:");
                Serial.println("  Status: Connected");
                Serial.println("  IP: " + WiFi.localIP().toString());
                Serial.println("  RSSI: " + String(WiFi.RSSI()) + " dBm");
            } else {
                Serial.println("\n📡 Connection: Not connected");
            }
            
        } else if (cmd == "help") {
            printHelp();
            
        } else {
            Serial.println("Unknown command: " + cmd);
            Serial.println("Type 'help' for available commands");
        }
    }
}

void printHelp() {
    Serial.println("\n========================================");
    Serial.println("  Available Commands");
    Serial.println("========================================");
    Serial.println();
    Serial.println("export          - Export configuration to JSON");
    Serial.println("import <json>   - Import configuration from JSON");
    Serial.println("show            - Show current configuration");
    Serial.println("clear           - Clear all configuration (factory reset)");
    Serial.println("help            - Show this help");
    Serial.println();
    Serial.println("Example import:");
    Serial.println("  import {\"wifi_ssid\":\"MyNetwork\",\"wifi_pass\":\"password123\"}");
    Serial.println();
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  IonConnect - Backup/Restore Example");
    Serial.println("========================================\n");
    
    printHelp();
    
    // Initialize IonConnect
    ion.init("BackupDevice");
    
    // Register callbacks
    ion.onConnect([]() {
        Serial.println("\n✓ WiFi Connected!");
        Serial.print("  IP: ");
        Serial.println(WiFi.localIP());
        
        Serial.println("\n💡 Web-based backup/restore:");
        Serial.println("  Go to: http://" + WiFi.localIP().toString());
        Serial.println("  Use 'Export Config' and 'Import Config' buttons");
    });
    
    ion.onConfigSaved([]() {
        Serial.println("✓ Configuration saved!");
    });
    
    // Start IonConnect
    if (!ion.begin()) {
        Serial.println("✗ IonConnect initialization failed!");
        return;
    }
    
    Serial.println("✓ IonConnect started");
    Serial.println("\n💬 Enter commands (type 'help' for list):");
}

void loop() {
    ion.handle();
    
    // Process serial commands
    processSerialCommand();
    
    // Your application logic here
}

/**
 * Use Cases:
 * 
 * 1. Manufacturing:
 *    - Pre-configure devices with default settings
 *    - Import base configuration during production
 * 
 * 2. Deployment:
 *    - Export config from one device
 *    - Import to multiple devices (fleet management)
 * 
 * 3. Backup:
 *    - Regular config backups
 *    - Disaster recovery
 * 
 * 4. Migration:
 *    - Move settings between devices
 *    - Upgrade to new hardware
 * 
 * 5. Testing:
 *    - Quickly switch between test configs
 *    - A/B testing different settings
 * 
 * JSON Format:
 * {
 *   "version": "1.0",
 *   "timestamp": 12345678,
 *   "config": {
 *     "wifi_ssid": "MyNetwork",
 *     "wifi_pass": "encrypted:...",
 *     "device_name": "MyDevice"
 *   }
 * }
 */

