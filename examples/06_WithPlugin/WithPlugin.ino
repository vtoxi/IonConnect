/**
 * @file WithPlugin.ino
 * @brief Plugin system example
 * 
 * This example demonstrates how to create and use plugins with IonConnect.
 * Plugins can add custom API endpoints, UI panels, and react to events.
 * 
 * Hardware: ESP32 or ESP8266
 */

#include <IonConnect.h>

IonConnectDevice ion;

// Simple status plugin example
class StatusPlugin : public IonPlugin {
private:
    IonConnectBase* ion;
    uint32_t bootTime;
    uint32_t connectionCount;
    
public:
    StatusPlugin() : bootTime(0), connectionCount(0) {}
    
    const char* getName() override {
        return "StatusMonitor";
    }
    
    const char* getVersion() override {
        return "1.0.0";
    }
    
    bool init(IonConnect::IonConnectBase* ionInstance) override {
        ion = ionInstance;
        bootTime = millis();
        Serial.println("[Plugin] StatusMonitor initialized");
        return true;
    }
    
    void handle() override {
        // Plugin logic runs in loop()
    }
    
    void registerRoutes(AsyncWebServer* server) override {
        // Add custom API endpoint
        server->on("/api/plugin/stats", HTTP_GET, [this](AsyncWebServerRequest* request) {
            String json = "{";
            json += "\"boot_time\":" + String(bootTime) + ",";
            json += "\"uptime\":" + String((millis() - bootTime) / 1000) + ",";
            json += "\"connections\":" + String(connectionCount) + ",";
            json += "\"heap_free\":" + String(ESP.getFreeHeap());
            json += "}";
            
            request->send(200, "application/json", json);
        });
        
        Serial.println("[Plugin] Custom route registered: /api/plugin/stats");
    }
    
    String getUIPanel() override {
        // Add custom UI panel to dashboard
        return R"html(
            <div class="panel mb-4">
                <h3 class="text-lg font-semibold mb-2">📊 Extended Stats</h3>
                <div id="plugin-stats">
                    <p class="text-sm text-slate-400">Loading stats...</p>
                </div>
                <script>
                    fetch('/api/plugin/stats')
                        .then(r => r.json())
                        .then(data => {
                            document.getElementById('plugin-stats').innerHTML = `
                                <p>Boot Time: ${data.boot_time}ms</p>
                                <p>Uptime: ${data.uptime}s</p>
                                <p>Connections: ${data.connections}</p>
                                <p>Heap Free: ${data.heap_free} bytes</p>
                            `;
                        });
                </script>
            </div>
        )html";
    }
    
    void onConfigChanged(const String& key, const String& value) override {
        Serial.println("[Plugin] Config changed: " + key + " = " + value);
    }
    
    void onWiFiConnect() override {
        connectionCount++;
        Serial.println("[Plugin] WiFi connected (count: " + String(connectionCount) + ")");
    }
    
    void onWiFiDisconnect() override {
        Serial.println("[Plugin] WiFi disconnected");
    }
};

// Create plugin instance
StatusPlugin statusPlugin;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  IonConnect - Plugin Example");
    Serial.println("========================================\n");
    
    // Initialize IonConnect
    ion.init("PluginDevice");
    
    // Register plugin BEFORE begin()
    if (ion.registerPlugin(&statusPlugin)) {
        Serial.println("✓ Plugin registered: " + String(statusPlugin.getName()));
    } else {
        Serial.println("✗ Failed to register plugin");
    }
    
    // Register callbacks
    ion.onConnect([]() {
        Serial.println("\n✓ WiFi Connected!");
        Serial.print("  IP: ");
        Serial.println(WiFi.localIP());
        Serial.println("\n📡 Plugin API available at:");
        Serial.println("  http://" + WiFi.localIP().toString() + "/api/plugin/stats");
    });
    
    // Start IonConnect
    if (!ion.begin()) {
        Serial.println("✗ IonConnect initialization failed!");
        return;
    }
    
    Serial.println("✓ IonConnect started with plugin support");
}

void loop() {
    ion.handle();
    
    // Your application logic here
}

/**
 * Plugin Development Tips:
 * 
 * 1. Lifecycle:
 *    - Create plugin instance globally
 *    - Register BEFORE ion.begin()
 *    - init() called automatically
 *    - handle() called in loop()
 * 
 * 2. API Routes:
 *    - Use /api/plugin/ prefix
 *    - Return JSON for consistency
 *    - Handle auth if needed
 * 
 * 3. UI Panels:
 *    - Return HTML string
 *    - Use Tailwind CSS classes
 *    - Include inline JavaScript
 *    - Fetch data from your API
 * 
 * 4. Events:
 *    - React to config changes
 *    - Monitor WiFi status
 *    - Update internal state
 * 
 * 5. Best Practices:
 *    - Keep plugins lightweight
 *    - Don't block in handle()
 *    - Use async operations
 *    - Handle errors gracefully
 */

