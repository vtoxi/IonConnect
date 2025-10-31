#ifndef ION_PLUGIN_H
#define ION_PLUGIN_H

#include <Arduino.h>

#if ION_PLATFORM_ESP32
    #include <ESPAsyncWebServer.h>
#elif ION_PLATFORM_ESP8266
    #include <ESPAsyncWebServer.h>
#endif

namespace IonConnect {

// Forward declaration
class IonConnectBase;

/**
 * @brief Abstract plugin interface for IonConnect extensibility
 * 
 * Allows third-party code to extend IonConnect functionality by:
 * - Adding custom REST API endpoints
 * - Injecting UI panels into the dashboard
 * - Reacting to WiFi and config events
 * - Running custom logic in the main loop
 */
class IonPlugin {
public:
    virtual ~IonPlugin() {}
    
    /**
     * @brief Get plugin name
     */
    virtual const char* getName() = 0;
    
    /**
     * @brief Get plugin version
     */
    virtual const char* getVersion() = 0;
    
    /**
     * @brief Initialize plugin
     * @param ion Pointer to IonConnect instance
     * @return true if initialization successful
     */
    virtual bool init(IonConnectBase* ion) = 0;
    
    /**
     * @brief Called in main loop() for plugin logic
     */
    virtual void handle() = 0;
    
    /**
     * @brief Register custom REST API routes
     * @param server Pointer to AsyncWebServer
     */
    virtual void registerRoutes(AsyncWebServer* server) {}
    
    /**
     * @brief Get HTML for dashboard UI panel
     * @return HTML string to inject into dashboard
     */
    virtual String getUIPanel() { return ""; }
    
    /**
     * @brief Called when configuration changes
     * @param key Configuration key that changed
     * @param value New value
     */
    virtual void onConfigChanged(const String& key, const String& value) {}
    
    /**
     * @brief Called when WiFi connects
     */
    virtual void onWiFiConnect() {}
    
    /**
     * @brief Called when WiFi disconnects
     */
    virtual void onWiFiDisconnect() {}
};

} // namespace IonConnect

#endif // ION_PLUGIN_H

