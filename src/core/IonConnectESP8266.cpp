#include "IonConnectESP8266.h"

#if ION_PLATFORM_ESP8266

#include "../utils/Logger.h"
#include "../schemas/default_schema.h"

namespace IonConnect {

IonConnectESP8266::IonConnectESP8266() 
    : initialized(false), portalActive(false), portalStartTime(0), portalTimeout(0) {
    
    storage = new StorageEEPROM();
    configManager = new ConfigManager(storage);
    wifiCore = new WiFiConnectionCore(configManager);
    securityManager = new SecurityManager();
    webPortal = new WebPortal(configManager, wifiCore, securityManager);
    
    #if ION_ENABLE_DIAGNOSTICS
    diagnostics = new DiagnosticsCollector();
    webPortal->setDiagnosticsCollector(diagnostics);
    #endif
}

IonConnectESP8266::~IonConnectESP8266() {
    delete webPortal;
    delete securityManager;
    delete wifiCore;
    delete configManager;
    delete storage;
    
    #if ION_ENABLE_DIAGNOSTICS
    delete diagnostics;
    #endif
}

bool IonConnectESP8266::init(const char* name, const IonConfig& cfg) {
    if (initialized) {
        ION_LOG_W("Already initialized");
        return true;
    }
    
    deviceName = name;
    config = cfg;
    
    ION_LOG("Initializing IonConnect for ESP8266");
    ION_LOG("Device: %s", deviceName.c_str());
    
    // Initialize storage
    if (!storage->begin()) {
        ION_LOG_E("Failed to initialize storage");
        return false;
    }
    
    // Load default schema if no custom schema loaded
    loadDefaultSchema();
    
    // Load configuration
    configManager->load();
    
    // Setup security
    if (!config.portalPassword.isEmpty()) {
        securityManager->setPortalPassword(config.portalPassword);
    }
    if (!config.accessToken.isEmpty()) {
        securityManager->setAccessToken(config.accessToken);
    }
    
    // Setup WiFi core
    wifiCore->setMaxReconnectAttempts(config.maxReconnectAttempts);
    wifiCore->setReconnectDelay(config.reconnectDelayMs);
    wifiCore->setConnectionTimeout(config.connectionTimeoutMs);
    
    // Register WiFi callbacks
    wifiCore->onConnect([this]() {
        ION_LOG("WiFi connected");
        if (portalActive) {
            webPortal->broadcastStatus("connected", wifiCore->getSSID(), 
                                      wifiCore->getIP().toString());
        }
        if (connectCallback) connectCallback();
    });
    
    wifiCore->onDisconnect([this]() {
        ION_LOG("WiFi disconnected");
        if (disconnectCallback) disconnectCallback();
    });
    
    wifiCore->onPortalFallback([this]() {
        ION_LOG("Portal fallback triggered");
        startPortal(config.portalTimeoutSeconds);
    });
    
    initialized = true;
    ION_LOG("IonConnect initialized");
    
    return true;
}

bool IonConnectESP8266::begin() {
    if (!initialized) {
        ION_LOG_E("Not initialized");
        return false;
    }
    
    // Initialize WiFi
    if (!wifiCore->begin()) {
        ION_LOG_E("Failed to initialize WiFi core");
        return false;
    }
    
    // Load saved networks
    wifiCore->loadNetworks();
    
    // Check if we have credentials
    String ssid = configManager->get("wifi_ssid");
    
    if (ssid.isEmpty() && config.autoStartPortal) {
        ION_LOG("No credentials, starting portal");
        return startPortal(config.portalTimeoutSeconds);
    }
    
    // Try to connect
    if (!ssid.isEmpty()) {
        String pass = configManager->get("wifi_pass");
        ION_LOG("Attempting connection to saved network");
        wifiCore->connect(ssid, pass);
    }
    
    return true;
}

void IonConnectESP8266::handle() {
    if (!initialized) return;
    
    // Handle WiFi state machine
    wifiCore->handle();
    
    // Handle web portal
    if (portalActive) {
        webPortal->handle();
        handlePortalTimeout();
    }
    
    #if ION_ENABLE_DIAGNOSTICS
    if (diagnostics) {
        diagnostics->collect();
    }
    #endif
}

void IonConnectESP8266::reset() {
    clearConfig();
    ESP.restart();
}

bool IonConnectESP8266::startPortal(uint32_t timeoutSeconds) {
    if (portalActive) {
        ION_LOG_W("Portal already active");
        return true;
    }
    
    ION_LOG("Starting captive portal");
    
    // Start AP
    if (!startAccessPoint()) {
        ION_LOG_E("Failed to start AP");
        return false;
    }
    
    // Start web portal
    if (!webPortal->begin(config.webServerPort)) {
        ION_LOG_E("Failed to start web portal");
        stopAccessPoint();
        return false;
    }
    
    portalActive = true;
    portalStartTime = millis();
    portalTimeout = timeoutSeconds * 1000;
    
    IPAddress ip = WiFi.softAPIP();
    ION_LOG("Portal started: %s", ip.toString().c_str());
    ION_LOG("SSID: %s", generateAPSSID().c_str());
    
    if (portalStartCallback) portalStartCallback();
    
    return true;
}

bool IonConnectESP8266::stopPortal() {
    if (!portalActive) return false;
    
    ION_LOG("Stopping portal");
    
    webPortal->stop();
    stopAccessPoint();
    
    portalActive = false;
    return true;
}

bool IonConnectESP8266::isPortalActive() {
    return portalActive;
}

String IonConnectESP8266::getConfig(const String& key) {
    return configManager->get(key);
}

bool IonConnectESP8266::setConfig(const String& key, const String& value) {
    return configManager->set(key, value);
}

bool IonConnectESP8266::loadConfigSchema(const char* jsonSchema) {
    return configManager->loadSchema(jsonSchema);
}

String IonConnectESP8266::exportConfig() {
    return configManager->exportJSON();
}

bool IonConnectESP8266::importConfig(const String& json) {
    return configManager->importJSON(json);
}

bool IonConnectESP8266::clearConfig() {
    return configManager->clear();
}

bool IonConnectESP8266::connect() {
    String ssid = configManager->get("wifi_ssid");
    String pass = configManager->get("wifi_pass");
    
    if (ssid.isEmpty()) {
        ION_LOG_E("No SSID configured");
        return false;
    }
    
    return wifiCore->connect(ssid, pass);
}

bool IonConnectESP8266::disconnect() {
    return wifiCore->disconnect();
}

bool IonConnectESP8266::isConnected() {
    return wifiCore->isConnected();
}

WiFiState IonConnectESP8266::getStatus() {
    return wifiCore->getState();
}

bool IonConnectESP8266::addNetwork(const String& ssid, const String& pass, int priority) {
    return wifiCore->addNetwork(ssid, pass, priority);
}

void IonConnectESP8266::onConnect(std::function<void()> cb) {
    connectCallback = cb;
}

void IonConnectESP8266::onDisconnect(std::function<void()> cb) {
    disconnectCallback = cb;
}

void IonConnectESP8266::onConfigSaved(std::function<void()> cb) {
    configSavedCallback = cb;
}

void IonConnectESP8266::onPortalStart(std::function<void()> cb) {
    portalStartCallback = cb;
}

void IonConnectESP8266::onPortalTimeout(std::function<void()> cb) {
    portalTimeoutCallback = cb;
}

void IonConnectESP8266::onError(std::function<void(IonError, const char*)> cb) {
    errorCallback = cb;
}

void IonConnectESP8266::setPortalPassword(const String& password) {
    config.portalPassword = password;
    securityManager->setPortalPassword(password);
}

void IonConnectESP8266::setAccessToken(const String& token) {
    config.accessToken = token;
    securityManager->setAccessToken(token);
}

#if ION_ENABLE_DIAGNOSTICS
DiagnosticsData IonConnectESP8266::getDiagnostics() {
    if (diagnostics) {
        return diagnostics->getData();
    }
    return DiagnosticsData();
}

void IonConnectESP8266::enableDiagnostics(bool enable) {
    config.enableDiagnostics = enable;
}
#endif

bool IonConnectESP8266::startAccessPoint() {
    String ssid = generateAPSSID();
    
    bool success;
    if (config.apPassword.isEmpty()) {
        success = WiFi.softAP(ssid.c_str());
    } else {
        success = WiFi.softAP(ssid.c_str(), config.apPassword.c_str());
    }
    
    if (!success) {
        ION_LOG_E("Failed to start AP");
        return false;
    }
    
    WiFi.softAPConfig(config.apIP, config.apGateway, config.apSubnet);
    
    ION_LOG("AP started: %s", ssid.c_str());
    ION_LOG("IP: %s", WiFi.softAPIP().toString().c_str());
    
    return true;
}

bool IonConnectESP8266::stopAccessPoint() {
    return WiFi.softAPdisconnect(true);
}

void IonConnectESP8266::handlePortalTimeout() {
    if (portalTimeout == 0) return;
    
    uint32_t elapsed = millis() - portalStartTime;
    if (elapsed > portalTimeout) {
        ION_LOG("Portal timeout");
        stopPortal();
        if (portalTimeoutCallback) portalTimeoutCallback();
    }
}

String IonConnectESP8266::generateAPSSID() {
    if (!config.apSSID.isEmpty()) {
        return config.apSSID;
    }
    
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char ssid[32];
    snprintf(ssid, sizeof(ssid), "IonConnect-%02X%02X", mac[4], mac[5]);
    
    return String(ssid);
}

void IonConnectESP8266::loadDefaultSchema() {
    if (configManager->getFields().size() == 0) {
        ION_LOG("Loading default schema");
        configManager->loadSchema(DEFAULT_SCHEMA);
    }
}

} // namespace IonConnect

#endif // ION_PLATFORM_ESP8266

