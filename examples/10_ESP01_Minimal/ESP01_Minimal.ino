/*
 * IonConnect - ESP-01 Minimal Memory Example
 * 
 * Demonstrates IonConnect running on ESP-01 with minimal memory footprint.
 * Perfect for low-RAM devices (ESP-01, ESP-01S with ~80KB RAM).
 * 
 * Features:
 * - Minimal mode enabled (ION_MINIMAL_MODE=1)
 * - ~6KB embedded HTML (vs ~15KB standard)
 * - Reduced JSON buffers (512-1024 bytes vs 2048-4096)
 * - No OTA, mDNS, diagnostics, or BLE
 * - Core WiFi provisioning via captive portal
 * 
 * Hardware:
 * - ESP-01 or ESP-01S
 * - 1MB flash minimum
 * 
 * Memory Usage:
 * - Baseline: ~25KB (ESP8266 SDK)
 * - IonConnect: ~12KB (minimal mode)
 * - AsyncWebServer: ~15KB
 * - Available: ~28KB for app
 * 
 * Setup:
 * 1. Connect ESP-01 to USB adapter
 * 2. Upload this sketch
 * 3. Power cycle ESP-01
 * 4. Connect to "IonConnect-XXXX" WiFi
 * 5. Open http://192.168.4.1
 * 6. Enter WiFi credentials
 * 
 * Author: IonConnect Team
 * License: MIT
 */

// IMPORTANT: Must be defined BEFORE including IonConnect.h
#define ION_MINIMAL_MODE 1

#include <ESP8266WiFi.h>
#include <IonConnect.h>

// Create IonConnect instance
IonConnectDevice ion;

// Configuration
const char* DEVICE_NAME = "ESP01-Sensor";

// Simple LED indicator (if available on your ESP-01)
#define LED_PIN 2  // GPIO2 (often connected to LED on ESP-01S)
bool ledState = false;

void setup() {
    // Initialize serial for debugging
    Serial.begin(115200);
    delay(100);
    Serial.println();
    Serial.println("==============================================");
    Serial.println("  IonConnect - ESP-01 Minimal Mode Example");
    Serial.println("==============================================");
    
    // Setup LED (optional)
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);  // LED off (active low)
    
    // Print initial memory status
    printMemoryStatus("Startup");
    
    // Initialize IonConnect with minimal configuration
    Serial.println("\n[1/3] Initializing IonConnect...");
    IonConfig config = IonConfig::minimal();
    
    // Optional: Customize configuration
    config.deviceName = DEVICE_NAME;
    config.portalTimeoutSeconds = 180;  // 3 minutes
    config.maxReconnectAttempts = 3;
    
    if (!ion.init(DEVICE_NAME, config)) {
        Serial.println("✗ Initialization failed!");
        blinkError();
        return;
    }
    Serial.println("✓ Initialized successfully");
    printMemoryStatus("After Init");
    
    // Register event callbacks
    Serial.println("\n[2/3] Setting up callbacks...");
    setupCallbacks();
    
    // Start IonConnect
    Serial.println("\n[3/3] Starting IonConnect...");
    if (!ion.begin()) {
        Serial.println("✗ Begin failed!");
        blinkError();
        return;
    }
    
    Serial.println("✓ IonConnect started successfully");
    printMemoryStatus("After Begin");
    
    Serial.println("\n==============================================");
    Serial.println("Setup complete! Entering main loop...");
    Serial.println("==============================================\n");
}

void loop() {
    // Let IonConnect handle its tasks (non-blocking)
    ion.handle();
    
    // Your application code here
    // Keep it lightweight to preserve memory!
    
    // Example: Blink LED when connected
    if (ion.isConnected()) {
        blinkConnected();
    }
    
    // Example: Print status every 30 seconds
    static unsigned long lastStatus = 0;
    if (millis() - lastStatus > 30000) {
        printStatus();
        lastStatus = millis();
    }
    
    // Small delay to prevent watchdog issues
    delay(10);
}

// ============================================
// Event Callbacks
// ============================================

void setupCallbacks() {
    // Called when WiFi connects
    ion.onConnect([]() {
        Serial.println("\n========================================");
        Serial.println("✓ WiFi Connected!");
        Serial.println("========================================");
        Serial.printf("SSID:      %s\n", WiFi.SSID().c_str());
        Serial.printf("IP:        %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("Gateway:   %s\n", WiFi.gatewayIP().toString().c_str());
        Serial.printf("DNS:       %s\n", WiFi.dnsIP().toString().c_str());
        Serial.printf("Signal:    %d dBm\n", WiFi.RSSI());
        Serial.printf("Channel:   %d\n", WiFi.channel());
        printMemoryStatus("Connected");
        Serial.println("========================================\n");
        
        // Blink LED to indicate connection
        for (int i = 0; i < 3; i++) {
            digitalWrite(LED_PIN, LOW);
            delay(100);
            digitalWrite(LED_PIN, HIGH);
            delay(100);
        }
    });
    
    // Called when WiFi disconnects
    ion.onDisconnect([]() {
        Serial.println("\n⚠ WiFi Disconnected");
        printMemoryStatus("Disconnected");
    });
    
    // Called when captive portal starts
    ion.onPortalStart([]() {
        Serial.println("\n========================================");
        Serial.println("→ Captive Portal Started");
        Serial.println("========================================");
        Serial.println("1. Connect to WiFi: IonConnect-XXXX");
        Serial.println("2. Open browser to: http://192.168.4.1");
        Serial.println("3. Enter your WiFi credentials");
        Serial.println("4. Click 'Save & Connect'");
        printMemoryStatus("Portal Active");
        Serial.println("========================================\n");
    });
    
    // Called when portal times out
    ion.onPortalTimeout([]() {
        Serial.println("\n⚠ Portal timeout - restarting...");
        delay(1000);
        ESP.restart();
    });
    
    // Called when errors occur
    ion.onError([](IonError error, const char* message) {
        Serial.printf("\n✗ Error %d: %s\n", error, message);
    });
}

// ============================================
// Utility Functions
// ============================================

void printMemoryStatus(const char* label) {
    uint32_t free = ESP.getFreeHeap();
    uint32_t total = 81920;  // ESP-01 typical total (~80KB)
    uint32_t used = total - free;
    float percent = (used * 100.0) / total;
    
    Serial.println("----------------------------------------");
    Serial.printf("Memory [%s]:\n", label);
    Serial.printf("  Free:  %5d bytes (%.1f%%)\n", free, 100.0 - percent);
    Serial.printf("  Used:  %5d bytes (%.1f%%)\n", used, percent);
    Serial.println("----------------------------------------");
}

void printStatus() {
    Serial.println("\n========== Status ==========");
    Serial.printf("Uptime:    %lu seconds\n", millis() / 1000);
    Serial.printf("WiFi:      %s\n", ion.isConnected() ? "Connected" : "Disconnected");
    
    if (ion.isConnected()) {
        Serial.printf("IP:        %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("Signal:    %d dBm\n", WiFi.RSSI());
    }
    
    Serial.printf("Portal:    %s\n", ion.isPortalActive() ? "Active" : "Inactive");
    printMemoryStatus("Current");
    Serial.println("============================\n");
}

void blinkConnected() {
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 2000) {  // Slow blink when connected
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? LOW : HIGH);
        lastBlink = millis();
    }
}

void blinkError() {
    // Fast blink to indicate error
    while (true) {
        digitalWrite(LED_PIN, LOW);
        delay(200);
        digitalWrite(LED_PIN, HIGH);
        delay(200);
    }
}

