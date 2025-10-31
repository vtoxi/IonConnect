#ifndef ION_CONNECT_BASE_H
#define ION_CONNECT_BASE_H

#include <Arduino.h>
#include <functional>
#include "IonTypes.h"
#include "IonConfig.h"

namespace IonConnect {

/**
 * @brief Abstract base class defining the IonConnect API
 * 
 * Platform-specific implementations (ESP32/ESP8266) inherit from this
 */
class IonConnectBase {
public:
    virtual ~IonConnectBase() {}
    
    // Lifecycle
    virtual bool init(const char* deviceName, const IonConfig& config = IonConfig()) = 0;
    virtual bool begin() = 0;
    virtual void handle() = 0; // Call in loop() - non-blocking
    virtual void reset() = 0;
    
    // Portal Control
    virtual bool startPortal(uint32_t timeoutSeconds = 0) = 0;
    virtual bool stopPortal() = 0;
    virtual bool isPortalActive() = 0;
    
    // Configuration
    virtual String getConfig(const String& key) = 0;
    virtual bool setConfig(const String& key, const String& value) = 0;
    virtual bool loadConfigSchema(const char* jsonSchema) = 0;
    virtual String exportConfig() = 0; // JSON backup
    virtual bool importConfig(const String& json) = 0; // Restore
    virtual bool clearConfig() = 0;
    
    // WiFi Management
    virtual bool connect() = 0;
    virtual bool disconnect() = 0;
    virtual bool isConnected() = 0;
    virtual WiFiState getStatus() = 0;
    virtual bool addNetwork(const String& ssid, const String& pass, int priority = 0) = 0;
    
    // Event Callbacks
    virtual void onConnect(std::function<void()> cb) = 0;
    virtual void onDisconnect(std::function<void()> cb) = 0;
    virtual void onConfigSaved(std::function<void()> cb) = 0;
    virtual void onPortalStart(std::function<void()> cb) = 0;
    virtual void onPortalTimeout(std::function<void()> cb) = 0;
    virtual void onError(std::function<void(IonError, const char*)> cb) = 0;
    
    // Security
    virtual void setPortalPassword(const String& password) = 0;
    virtual void setAccessToken(const String& token) = 0;
    
#if ION_ENABLE_DIAGNOSTICS
    // Diagnostics
    virtual DiagnosticsData getDiagnostics() = 0;
    virtual void enableDiagnostics(bool enable) = 0;
#endif
};

} // namespace IonConnect

#endif // ION_CONNECT_BASE_H

