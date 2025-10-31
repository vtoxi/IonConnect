/**
 * @file Diagnostics.ino
 * @brief System diagnostics and monitoring example
 * 
 * This example demonstrates:
 * - Real-time system health monitoring
 * - Memory usage tracking
 * - WiFi signal strength monitoring
 * - Error logging
 * - Performance metrics
 * 
 * Hardware: ESP32 or ESP8266
 */

#include <IonConnect.h>

IonConnectDevice ion;

void printDiagnostics() {
    #if ION_ENABLE_DIAGNOSTICS
    Serial.println("\n========================================");
    Serial.println("  📊 System Diagnostics");
    Serial.println("========================================");
    
    DiagnosticsData diag = ion.getDiagnostics();
    
    // Memory Information
    Serial.println("\n💾 Memory:");
    Serial.print("  Heap Free: ");
    Serial.print(diag.heapFree);
    Serial.println(" bytes");
    
    Serial.print("  Heap Size: ");
    Serial.print(diag.heapSize);
    Serial.println(" bytes");
    
    float heapUsage = (1.0 - ((float)diag.heapFree / diag.heapSize)) * 100;
    Serial.print("  Heap Usage: ");
    Serial.print(heapUsage, 1);
    Serial.println("%");
    
    #if ION_PLATFORM_ESP32
    Serial.print("  Min Free Heap: ");
    Serial.print(diag.heapMinFree);
    Serial.println(" bytes");
    #endif
    
    // WiFi Information
    if (ion.isConnected()) {
        Serial.println("\n📡 WiFi:");
        Serial.print("  SSID: ");
        Serial.println(WiFi.SSID());
        
        Serial.print("  RSSI: ");
        Serial.print(diag.rssi);
        Serial.print(" dBm (");
        
        // Signal quality
        if (diag.rssi >= -50) Serial.print("Excellent");
        else if (diag.rssi >= -60) Serial.print("Good");
        else if (diag.rssi >= -70) Serial.print("Fair");
        else Serial.print("Weak");
        Serial.println(")");
        
        Serial.print("  Reconnects: ");
        Serial.println(diag.wifiReconnects);
        
        Serial.print("  Connected Time: ");
        Serial.print(diag.wifiConnectedTime);
        Serial.println(" seconds");
    } else {
        Serial.println("\n📡 WiFi: Not connected");
    }
    
    // System Information
    Serial.println("\n⚙️ System:");
    Serial.print("  Uptime: ");
    printUptime(diag.uptime);
    
    Serial.print("  CPU Freq: ");
    Serial.print(diag.cpuFreq);
    Serial.println(" MHz");
    
    #if ION_PLATFORM_ESP32
    Serial.print("  Chip Model: ");
    Serial.println(ESP.getChipModel());
    
    Serial.print("  Chip Revision: ");
    Serial.println(ESP.getChipRevision());
    
    Serial.print("  Flash Size: ");
    Serial.print(ESP.getFlashChipSize() / 1024 / 1024);
    Serial.println(" MB");
    #endif
    
    // Application Statistics
    Serial.println("\n📈 Application:");
    Serial.print("  API Requests: ");
    Serial.println(diag.apiRequests);
    
    Serial.print("  API Errors: ");
    Serial.println(diag.apiErrors);
    
    Serial.print("  Portal Sessions: ");
    Serial.println(diag.portalSessions);
    
    Serial.print("  Total Errors: ");
    Serial.println(diag.totalErrors);
    
    if (!diag.lastError.isEmpty()) {
        Serial.print("  Last Error: ");
        Serial.println(diag.lastError);
    }
    
    Serial.println("========================================\n");
    #else
    Serial.println("⚠️ Diagnostics not enabled");
    Serial.println("  Enable with: #define ION_ENABLE_DIAGNOSTICS 1");
    #endif
}

void printUptime(uint32_t seconds) {
    uint32_t days = seconds / 86400;
    seconds %= 86400;
    uint32_t hours = seconds / 3600;
    seconds %= 3600;
    uint32_t minutes = seconds / 60;
    seconds %= 60;
    
    if (days > 0) {
        Serial.print(days);
        Serial.print("d ");
    }
    Serial.print(hours);
    Serial.print("h ");
    Serial.print(minutes);
    Serial.print("m ");
    Serial.print(seconds);
    Serial.println("s");
}

void monitorMemory() {
    static uint32_t minHeap = 0xFFFFFFFF;
    uint32_t freeHeap = ESP.getFreeHeap();
    
    if (freeHeap < minHeap) {
        minHeap = freeHeap;
        Serial.print("⚠️ New minimum heap: ");
        Serial.print(minHeap);
        Serial.println(" bytes");
    }
    
    // Warning if heap gets low
    #if ION_PLATFORM_ESP32
    if (freeHeap < 20000) {  // Less than 20KB free
        Serial.println("🔴 WARNING: Low memory!");
    }
    #else
    if (freeHeap < 5000) {   // Less than 5KB free (ESP8266)
        Serial.println("🔴 WARNING: Low memory!");
    }
    #endif
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  IonConnect - Diagnostics Example");
    Serial.println("========================================\n");
    
    // Initialize IonConnect with diagnostics enabled
    IonConfig config;
    config.enableDiagnostics = true;
    
    ion.init("DiagDevice", config);
    
    // Register callbacks
    ion.onConnect([]() {
        Serial.println("\n✓ WiFi Connected!");
        Serial.print("  IP: ");
        Serial.println(WiFi.localIP());
        Serial.println("\n💡 Web diagnostics available at:");
        Serial.println("  http://" + WiFi.localIP().toString() + "/api/diagnostics");
    });
    
    // Start IonConnect
    if (!ion.begin()) {
        Serial.println("✗ IonConnect initialization failed!");
        return;
    }
    
    Serial.println("✓ IonConnect started with diagnostics enabled");
    Serial.println("\n💡 Commands:");
    Serial.println("  Type 'd' for diagnostics report");
    Serial.println("  Type 'm' for memory status");
}

void loop() {
    ion.handle();
    
    // Check for serial commands
    if (Serial.available()) {
        char cmd = Serial.read();
        if (cmd == 'd') {
            printDiagnostics();
        } else if (cmd == 'm') {
            monitorMemory();
        }
    }
    
    // Automatic diagnostics every 5 minutes
    static uint32_t lastReport = 0;
    if (millis() - lastReport > 300000) {
        lastReport = millis();
        printDiagnostics();
    }
    
    // Memory monitoring every 10 seconds
    static uint32_t lastMemCheck = 0;
    if (millis() - lastMemCheck > 10000) {
        lastMemCheck = millis();
        monitorMemory();
    }
}

/**
 * Monitoring Best Practices:
 * 
 * 1. Memory:
 *    - Track minimum free heap
 *    - Alert on low memory
 *    - Look for memory leaks
 * 
 * 2. WiFi:
 *    - Monitor signal strength
 *    - Track reconnection frequency
 *    - Log disconnection causes
 * 
 * 3. Performance:
 *    - Measure API response times
 *    - Track error rates
 *    - Monitor uptime
 * 
 * 4. Logging:
 *    - Log to serial for debugging
 *    - Send to cloud for analytics
 *    - Store locally for forensics
 * 
 * 5. Alerting:
 *    - Set thresholds for warnings
 *    - Trigger actions on critical issues
 *    - Implement watchdog timers
 */

