#ifndef WEB_PORTAL_H
#define WEB_PORTAL_H

#include <Arduino.h>
#include "../core/IonTypes.h"
#include "ConfigManager.h"
#include "WiFiConnectionCore.h"
#include "SecurityManager.h"
#include "DNSHandler.h"
#include "../web/AssetManager.h"

#if ION_ENABLE_DIAGNOSTICS
#include "DiagnosticsCollector.h"
#endif

#if ION_PLATFORM_ESP32
    #include <WiFi.h>
    #include <ESPAsyncWebServer.h>
    #include <AsyncTCP.h>
#elif ION_PLATFORM_ESP8266
    #include <ESP8266WiFi.h>
    #include <ESPAsyncWebServer.h>
    #include <ESPAsyncTCP.h>
#endif

namespace IonConnect {

/**
 * @brief Manages web server, REST API, SSE, and captive portal
 */
class WebPortal {
public:
    WebPortal(ConfigManager* config, WiFiConnectionCore* wifi, SecurityManager* security);
    ~WebPortal();
    
    bool begin(uint16_t port = 80);
    void handle();
    bool stop();
    
    bool isRunning();
    uint16_t getPort();
    
    // Event Broadcasting
    void broadcastStatus(const String& state, const String& ssid = "", 
                        const String& ip = "", const String& message = "");
    
    #if ION_ENABLE_DIAGNOSTICS
    void setDiagnosticsCollector(DiagnosticsCollector* diag);
    #endif
    
private:
    ConfigManager* config;
    WiFiConnectionCore* wifi;
    SecurityManager* security;
    DNSHandler* dnsHandler;
    AssetManager* assetManager;
    
    #if ION_ENABLE_DIAGNOSTICS
    DiagnosticsCollector* diagnostics;
    #endif
    
    AsyncWebServer* server;
    AsyncEventSource* events;
    
    bool running;
    uint16_t port;
    
    void setupRoutes();
    void setupPortalRoutes();
    void setupAPIRoutes();
    void setupSSE();
    
    // Route handlers
    void handleRoot(AsyncWebServerRequest* request);
    void handleJS(AsyncWebServerRequest* request);
    void handleSchema(AsyncWebServerRequest* request);
    void handleConfig(AsyncWebServerRequest* request);
    void handleConfigPost(AsyncWebServerRequest* request, uint8_t* data, size_t len);
    void handleScan(AsyncWebServerRequest* request);
    void handleNetworks(AsyncWebServerRequest* request);
    void handleStatus(AsyncWebServerRequest* request);
    void handleClear(AsyncWebServerRequest* request);
    void handleExport(AsyncWebServerRequest* request);
    void handleImport(AsyncWebServerRequest* request, uint8_t* data, size_t len);
    void handleReboot(AsyncWebServerRequest* request);
    void handleInfo(AsyncWebServerRequest* request);
    
    #if ION_ENABLE_DIAGNOSTICS
    void handleDiagnostics(AsyncWebServerRequest* request);
    #endif
    
    // Helper methods
    bool checkAuth(AsyncWebServerRequest* request);
    void sendJSON(AsyncWebServerRequest* request, const String& json, int code = 200);
    void sendError(AsyncWebServerRequest* request, const String& error, int code = 400);
};

} // namespace IonConnect

#endif // WEB_PORTAL_H

