#ifndef WIFI_CONNECTION_CORE_H
#define WIFI_CONNECTION_CORE_H

#include <Arduino.h>
#include <vector>
#include <functional>
#include "../core/IonTypes.h"
#include "ConfigManager.h"

#if ION_PLATFORM_ESP32
    #include <WiFi.h>
#elif ION_PLATFORM_ESP8266
    #include <ESP8266WiFi.h>
#endif

namespace IonConnect {

/**
 * @brief Smart WiFi connection manager with auto-reconnect and multi-network support
 * 
 * Features:
 * - Multi-network credential storage (prioritized)
 * - Exponential backoff reconnection
 * - Automatic strongest network selection
 * - Portal fallback after max failures
 * - Connection state machine
 */
class WiFiConnectionCore {
public:
    WiFiConnectionCore(ConfigManager* config);
    ~WiFiConnectionCore();
    
    // Lifecycle
    bool begin();
    void handle(); // Non-blocking state machine
    void end();
    
    // Connection Management
    bool connect(const String& ssid, const String& password);
    bool connectToBest(); // Scan and connect to strongest known network
    bool disconnect();
    
    // Network Management
    bool addNetwork(const String& ssid, const String& password, int8_t priority = 0);
    bool removeNetwork(const String& ssid);
    std::vector<WiFiCredential> getSavedNetworks();
    bool saveNetworks();
    bool loadNetworks();
    
    // Scanning
    bool startScan();
    bool isScanComplete();
    std::vector<NetworkInfo> getScanResults();
    
    // Status
    WiFiState getState();
    String getSSID();
    int8_t getRSSI();
    IPAddress getIP();
    IPAddress getGateway();
    IPAddress getSubnet();
    String getMACAddress();
    bool isConnected();
    uint32_t getUptime();
    
    // Callbacks
    void onConnect(std::function<void()> cb);
    void onDisconnect(std::function<void()> cb);
    void onReconnecting(std::function<void(uint8_t)> cb);
    void onPortalFallback(std::function<void()> cb);
    
    // Configuration
    void setMaxReconnectAttempts(uint8_t attempts);
    void setReconnectDelay(uint32_t delayMs);
    void setConnectionTimeout(uint32_t timeoutMs);
    
private:
    ConfigManager* config;
    WiFiState state;
    WiFiState previousState;
    std::vector<WiFiCredential> savedNetworks;
    
    uint8_t reconnectAttempts;
    uint8_t maxReconnectAttempts;
    uint32_t lastReconnectTime;
    uint32_t reconnectDelay;
    uint32_t connectionTimeout;
    uint32_t connectionStartTime;
    uint32_t connectedTime;
    
    bool scanInProgress;
    bool scanComplete;
    
    // Callbacks
    std::function<void()> connectCallback;
    std::function<void()> disconnectCallback;
    std::function<void(uint8_t)> reconnectingCallback;
    std::function<void()> portalFallbackCallback;
    
    void handleStateTransition();
    void startReconnect();
    void incrementBackoff();
    void resetBackoff();
    void setState(WiFiState newState);
    WiFiCredential* findNetwork(const String& ssid);
    WiFiCredential* findBestNetwork();
    
    #if ION_PLATFORM_ESP32
    static void wifiEventHandler(WiFiEvent_t event);
    static WiFiConnectionCore* instance;
    #elif ION_PLATFORM_ESP8266
    WiFiEventHandler connectHandler;
    WiFiEventHandler disconnectHandler;
    #endif
};

} // namespace IonConnect

#endif // WIFI_CONNECTION_CORE_H

