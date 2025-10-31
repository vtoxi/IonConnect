#include "WiFiConnectionCore.h"
#include "../utils/Logger.h"
#include <ArduinoJson.h>

namespace IonConnect {

#if ION_PLATFORM_ESP32
WiFiConnectionCore* WiFiConnectionCore::instance = nullptr;
#endif

WiFiConnectionCore::WiFiConnectionCore(ConfigManager* config)
    : config(config), state(WIFI_IDLE), previousState(WIFI_IDLE),
      reconnectAttempts(0), maxReconnectAttempts(5), lastReconnectTime(0),
      reconnectDelay(1000), connectionTimeout(10000), connectionStartTime(0),
      connectedTime(0), scanInProgress(false), scanComplete(false) {
    
    #if ION_PLATFORM_ESP32
    instance = this;
    #endif
}

WiFiConnectionCore::~WiFiConnectionCore() {
    end();
}

bool WiFiConnectionCore::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false); // We handle reconnection
    
    #if ION_PLATFORM_ESP32
    WiFi.onEvent(wifiEventHandler);
    #elif ION_PLATFORM_ESP8266
    connectHandler = WiFi.onStationModeGotIP([this](const WiFiEventStationModeGotIP& event) {
        ION_LOG("WiFi connected - IP: %s", WiFi.localIP().toString().c_str());
        setState(WIFI_CONNECTED);
        if (connectCallback) connectCallback();
    });
    
    disconnectHandler = WiFi.onStationModeDisconnected([this](const WiFiEventStationModeDisconnected& event) {
        ION_LOG("WiFi disconnected");
        if (state == WIFI_CONNECTED) {
            setState(WIFI_RECONNECTING);
            if (disconnectCallback) disconnectCallback();
        }
    });
    #endif
    
    loadNetworks();
    
    ION_LOG("WiFiConnectionCore initialized");
    return true;
}

void WiFiConnectionCore::handle() {
    handleStateTransition();
}

void WiFiConnectionCore::end() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

bool WiFiConnectionCore::connect(const String& ssid, const String& password) {
    ION_LOG("Connecting to: %s", ssid.c_str());
    
    setState(WIFI_CONNECTING);
    connectionStartTime = millis();
    
    WiFi.begin(ssid.c_str(), password.c_str());
    
    // Add/update in saved networks
    addNetwork(ssid, password, 10);
    
    return true;
}

bool WiFiConnectionCore::connectToBest() {
    WiFiCredential* best = findBestNetwork();
    
    if (!best) {
        ION_LOG("No known networks found");
        return false;
    }
    
    ION_LOG("Connecting to best network: %s (RSSI: %d)", 
            best->ssid.c_str(), best->lastRSSI);
    
    return connect(best->ssid, best->password);
}

bool WiFiConnectionCore::disconnect() {
    WiFi.disconnect();
    setState(WIFI_DISCONNECTED);
    return true;
}

bool WiFiConnectionCore::addNetwork(const String& ssid, const String& password, int8_t priority) {
    // Check if already exists
    WiFiCredential* existing = findNetwork(ssid);
    if (existing) {
        existing->password = password;
        existing->priority = priority;
    } else {
        WiFiCredential cred(ssid, password, priority);
        savedNetworks.push_back(cred);
    }
    
    return saveNetworks();
}

bool WiFiConnectionCore::removeNetwork(const String& ssid) {
    for (auto it = savedNetworks.begin(); it != savedNetworks.end(); ++it) {
        if (it->ssid == ssid) {
            savedNetworks.erase(it);
            return saveNetworks();
        }
    }
    return false;
}

std::vector<WiFiCredential> WiFiConnectionCore::getSavedNetworks() {
    return savedNetworks;
}

bool WiFiConnectionCore::saveNetworks() {
    if (!config) return false;
    
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();
    
    for (const auto& net : savedNetworks) {
        JsonObject obj = arr.add<JsonObject>();
        obj["ssid"] = net.ssid;
        obj["pass"] = net.password;
        obj["priority"] = net.priority;
        obj["lastConnected"] = net.lastConnected;
        obj["lastRSSI"] = net.lastRSSI;
    }
    
    String json;
    serializeJson(doc, json);
    
    config->set("saved_networks", json);
    return config->save();
}

bool WiFiConnectionCore::loadNetworks() {
    if (!config) return false;
    
    String json = config->get("saved_networks", "[]");
    JsonDocument doc;
    
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
        ION_LOG_W("Failed to load saved networks");
        return false;
    }
    
    savedNetworks.clear();
    JsonArray arr = doc.as<JsonArray>();
    
    for (JsonVariant v : arr) {
        JsonObject obj = v.as<JsonObject>();
        WiFiCredential cred;
        cred.ssid = obj["ssid"].as<String>();
        cred.password = obj["pass"].as<String>();
        cred.priority = obj["priority"] | 0;
        cred.lastConnected = obj["lastConnected"] | 0;
        cred.lastRSSI = obj["lastRSSI"] | 0;
        savedNetworks.push_back(cred);
    }
    
    ION_LOG("Loaded %d saved networks", savedNetworks.size());
    return true;
}

bool WiFiConnectionCore::startScan() {
    if (scanInProgress) return false;
    
    scanInProgress = true;
    scanComplete = false;
    
    #if ION_PLATFORM_ESP32
    WiFi.scanNetworks(true); // Async scan
    #elif ION_PLATFORM_ESP8266
    WiFi.scanNetworksAsync([this](int n) {
        scanComplete = true;
        scanInProgress = false;
    });
    #endif
    
    return true;
}

bool WiFiConnectionCore::isScanComplete() {
    #if ION_PLATFORM_ESP32
    int n = WiFi.scanComplete();
    if (n >= 0) {
        scanComplete = true;
        scanInProgress = false;
        return true;
    }
    return false;
    #else
    return scanComplete;
    #endif
}

std::vector<NetworkInfo> WiFiConnectionCore::getScanResults() {
    std::vector<NetworkInfo> results;
    
    int n = WiFi.scanComplete();
    if (n < 0) return results;
    
    for (int i = 0; i < n; i++) {
        NetworkInfo info;
        info.ssid = WiFi.SSID(i);
        info.rssi = WiFi.RSSI(i);
        info.encryption = WiFi.encryptionType(i);
        info.channel = WiFi.channel(i);
        results.push_back(info);
    }
    
    // Sort by RSSI (strongest first)
    std::sort(results.begin(), results.end(), [](const NetworkInfo& a, const NetworkInfo& b) {
        return a.rssi > b.rssi;
    });
    
    return results;
}

WiFiState WiFiConnectionCore::getState() {
    return state;
}

String WiFiConnectionCore::getSSID() {
    return WiFi.SSID();
}

int8_t WiFiConnectionCore::getRSSI() {
    return WiFi.RSSI();
}

IPAddress WiFiConnectionCore::getIP() {
    return WiFi.localIP();
}

IPAddress WiFiConnectionCore::getGateway() {
    return WiFi.gatewayIP();
}

IPAddress WiFiConnectionCore::getSubnet() {
    return WiFi.subnetMask();
}

String WiFiConnectionCore::getMACAddress() {
    return WiFi.macAddress();
}

bool WiFiConnectionCore::isConnected() {
    return state == WIFI_CONNECTED && WiFi.status() == WL_CONNECTED;
}

uint32_t WiFiConnectionCore::getUptime() {
    if (state == WIFI_CONNECTED && connectedTime > 0) {
        return (millis() - connectedTime) / 1000;
    }
    return 0;
}

void WiFiConnectionCore::onConnect(std::function<void()> cb) {
    connectCallback = cb;
}

void WiFiConnectionCore::onDisconnect(std::function<void()> cb) {
    disconnectCallback = cb;
}

void WiFiConnectionCore::onReconnecting(std::function<void(uint8_t)> cb) {
    reconnectingCallback = cb;
}

void WiFiConnectionCore::onPortalFallback(std::function<void()> cb) {
    portalFallbackCallback = cb;
}

void WiFiConnectionCore::setMaxReconnectAttempts(uint8_t attempts) {
    maxReconnectAttempts = attempts;
}

void WiFiConnectionCore::setReconnectDelay(uint32_t delayMs) {
    reconnectDelay = delayMs;
}

void WiFiConnectionCore::setConnectionTimeout(uint32_t timeoutMs) {
    connectionTimeout = timeoutMs;
}

void WiFiConnectionCore::handleStateTransition() {
    switch (state) {
        case WIFI_IDLE:
            // Do nothing
            break;
            
        case WIFI_SCANNING:
            if (isScanComplete()) {
                setState(WIFI_IDLE);
            }
            break;
            
        case WIFI_CONNECTING:
            if (WiFi.status() == WL_CONNECTED) {
                setState(WIFI_CONNECTED);
                resetBackoff();
                connectedTime = millis();
                
                // Update network info
                WiFiCredential* net = findNetwork(WiFi.SSID());
                if (net) {
                    net->lastConnected = millis() / 1000;
                    net->lastRSSI = WiFi.RSSI();
                    saveNetworks();
                }
                
                if (connectCallback) connectCallback();
            } else if (millis() - connectionStartTime > connectionTimeout) {
                ION_LOG_W("Connection timeout");
                setState(WIFI_RECONNECTING);
            }
            break;
            
        case WIFI_CONNECTED:
            // Monitor connection
            if (WiFi.status() != WL_CONNECTED) {
                ION_LOG_W("Connection lost");
                setState(WIFI_RECONNECTING);
                if (disconnectCallback) disconnectCallback();
            }
            break;
            
        case WIFI_RECONNECTING:
            if (millis() - lastReconnectTime < reconnectDelay) {
                break; // Wait for backoff
            }
            
            reconnectAttempts++;
            
            if (reconnectAttempts > maxReconnectAttempts) {
                ION_LOG_E("Max reconnect attempts reached");
                setState(WIFI_PORTAL_FALLBACK);
                if (portalFallbackCallback) portalFallbackCallback();
            } else {
                ION_LOG("Reconnect attempt %d/%d", reconnectAttempts, maxReconnectAttempts);
                if (reconnectingCallback) reconnectingCallback(reconnectAttempts);
                
                connectToBest();
                incrementBackoff();
            }
            break;
            
        case WIFI_PORTAL_FALLBACK:
            // Portal should be started by external handler
            break;
            
        case WIFI_DISCONNECTED:
            // Do nothing
            break;
    }
}

void WiFiConnectionCore::startReconnect() {
    reconnectAttempts = 0;
    lastReconnectTime = millis();
    setState(WIFI_RECONNECTING);
}

void WiFiConnectionCore::incrementBackoff() {
    lastReconnectTime = millis();
    reconnectDelay = min(reconnectDelay * 2, (uint32_t)32000); // Max 32s
}

void WiFiConnectionCore::resetBackoff() {
    reconnectAttempts = 0;
    reconnectDelay = 1000;
}

void WiFiConnectionCore::setState(WiFiState newState) {
    if (state != newState) {
        previousState = state;
        state = newState;
        ION_LOG("WiFi state: %d -> %d", previousState, state);
    }
}

WiFiCredential* WiFiConnectionCore::findNetwork(const String& ssid) {
    for (auto& net : savedNetworks) {
        if (net.ssid == ssid) {
            return &net;
        }
    }
    return nullptr;
}

WiFiCredential* WiFiConnectionCore::findBestNetwork() {
    // Start scan if not done
    if (!scanComplete) {
        startScan();
        // Wait a bit for scan
        uint32_t start = millis();
        while (!isScanComplete() && millis() - start < 5000) {
            delay(100);
        }
    }
    
    auto scanResults = getScanResults();
    WiFiCredential* best = nullptr;
    int8_t bestRSSI = -100;
    
    // Find the strongest network we have credentials for
    for (const auto& result : scanResults) {
        WiFiCredential* net = findNetwork(result.ssid);
        if (net && result.rssi > bestRSSI) {
            best = net;
            bestRSSI = result.rssi;
            best->lastRSSI = result.rssi;
        }
    }
    
    return best;
}

#if ION_PLATFORM_ESP32
void WiFiConnectionCore::wifiEventHandler(WiFiEvent_t event) {
    if (!instance) return;
    
    switch (event) {
        case SYSTEM_EVENT_STA_GOT_IP:
            ION_LOG("WiFi connected - IP: %s", WiFi.localIP().toString().c_str());
            instance->setState(WIFI_CONNECTED);
            if (instance->connectCallback) instance->connectCallback();
            break;
            
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ION_LOG("WiFi disconnected");
            if (instance->state == WIFI_CONNECTED) {
                instance->setState(WIFI_RECONNECTING);
                if (instance->disconnectCallback) instance->disconnectCallback();
            }
            break;
            
        default:
            break;
    }
}
#endif

} // namespace IonConnect

