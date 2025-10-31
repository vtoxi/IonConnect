#ifndef ION_CONNECT_ESP8266_H
#define ION_CONNECT_ESP8266_H

#include "IonConnectBase.h"

#if ION_PLATFORM_ESP8266

#include "../storage/StorageEEPROM.h"
#include "../modules/ConfigManager.h"
#include "../modules/WiFiConnectionCore.h"
#include "../modules/SecurityManager.h"
#include "../modules/WebPortal.h"

#if ION_ENABLE_DIAGNOSTICS
#include "../modules/DiagnosticsCollector.h"
#endif

#include <ESP8266WiFi.h>

namespace IonConnect {

/**
 * @brief ESP8266 implementation of IonConnect
 */
class IonConnectESP8266 : public IonConnectBase {
public:
    IonConnectESP8266();
    virtual ~IonConnectESP8266();
    
    // Lifecycle
    bool init(const char* deviceName, const IonConfig& config = IonConfig()) override;
    bool begin() override;
    void handle() override;
    void reset() override;
    
    // Portal Control
    bool startPortal(uint32_t timeoutSeconds = 0) override;
    bool stopPortal() override;
    bool isPortalActive() override;
    
    // Configuration
    String getConfig(const String& key) override;
    bool setConfig(const String& key, const String& value) override;
    bool loadConfigSchema(const char* jsonSchema) override;
    String exportConfig() override;
    bool importConfig(const String& json) override;
    bool clearConfig() override;
    
    // WiFi Management
    bool connect() override;
    bool disconnect() override;
    bool isConnected() override;
    WiFiState getStatus() override;
    bool addNetwork(const String& ssid, const String& pass, int priority = 0) override;
    
    // Event Callbacks
    void onConnect(std::function<void()> cb) override;
    void onDisconnect(std::function<void()> cb) override;
    void onConfigSaved(std::function<void()> cb) override;
    void onPortalStart(std::function<void()> cb) override;
    void onPortalTimeout(std::function<void()> cb) override;
    void onError(std::function<void(IonError, const char*)> cb) override;
    
    // Security
    void setPortalPassword(const String& password) override;
    void setAccessToken(const String& token) override;
    
#if ION_ENABLE_DIAGNOSTICS
    // Diagnostics
    DiagnosticsData getDiagnostics() override;
    void enableDiagnostics(bool enable) override;
#endif
    
private:
    IonConfig config;
    String deviceName;
    bool initialized;
    bool portalActive;
    uint32_t portalStartTime;
    uint32_t portalTimeout;
    
    // Core modules
    StorageEEPROM* storage;
    ConfigManager* configManager;
    WiFiConnectionCore* wifiCore;
    SecurityManager* securityManager;
    WebPortal* webPortal;
    
    #if ION_ENABLE_DIAGNOSTICS
    DiagnosticsCollector* diagnostics;
    #endif
    
    // Callbacks
    std::function<void()> connectCallback;
    std::function<void()> disconnectCallback;
    std::function<void()> configSavedCallback;
    std::function<void()> portalStartCallback;
    std::function<void()> portalTimeoutCallback;
    std::function<void(IonError, const char*)> errorCallback;
    
    // Helper methods
    bool startAccessPoint();
    bool stopAccessPoint();
    void handlePortalTimeout();
    String generateAPSSID();
    void loadDefaultSchema();
};

} // namespace IonConnect

#endif // ION_PLATFORM_ESP8266
#endif // ION_CONNECT_ESP8266_H

