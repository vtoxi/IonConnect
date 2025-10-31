#ifndef ION_TYPES_H
#define ION_TYPES_H

#include <Arduino.h>
#include <functional>

// Platform detection
#if defined(ESP32)
    #define ION_PLATFORM_ESP32 1
    #define ION_PLATFORM_ESP8266 0
#elif defined(ESP8266)
    #define ION_PLATFORM_ESP32 0
    #define ION_PLATFORM_ESP8266 1
#else
    #error "Unsupported platform. IonConnect supports ESP32 and ESP8266 only."
#endif

// Feature flags
#if ION_PLATFORM_ESP32
    #define ION_HAS_BLE 1
    #define ION_HAS_NVS 1
    #define ION_HAS_WPS 1
    #define ION_HAS_PSRAM 1
#else
    #define ION_HAS_BLE 0
    #define ION_HAS_NVS 0
    #define ION_HAS_WPS 0
    #define ION_HAS_PSRAM 0
#endif

// Optional features (can be disabled via build flags)
#ifndef ION_ENABLE_BLE
    #define ION_ENABLE_BLE ION_HAS_BLE
#endif

#ifndef ION_ENABLE_OTA
    #define ION_ENABLE_OTA 1
#endif

#ifndef ION_ENABLE_DIAGNOSTICS
    #define ION_ENABLE_DIAGNOSTICS 1
#endif

#ifndef ION_ENABLE_PLUGINS
    #define ION_ENABLE_PLUGINS 1
#endif

#ifndef ION_ENABLE_MDNS
    #define ION_ENABLE_MDNS 1
#endif

#ifndef ION_USE_LITTLEFS
    #define ION_USE_LITTLEFS 1
#endif

#ifndef ION_DEBUG
    #define ION_DEBUG 0
#endif

namespace IonConnect {

// WiFi States
enum WiFiState {
    WIFI_IDLE,
    WIFI_SCANNING,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_RECONNECTING,
    WIFI_PORTAL_FALLBACK,
    WIFI_DISCONNECTED
};

// Error codes
enum IonError {
    ION_OK = 0,
    ION_ERR_NO_CREDENTIALS,
    ION_ERR_WIFI_FAILED,
    ION_ERR_CONFIG_INVALID,
    ION_ERR_STORAGE_FAILED,
    ION_ERR_TIMEOUT,
    ION_ERR_AUTH_FAILED,
    ION_ERR_OTA_FAILED,
    ION_ERR_UNKNOWN
};

// WiFi Credential structure
struct WiFiCredential {
    String ssid;
    String password;
    int8_t priority = 0;        // Higher = preferred
    uint32_t lastConnected = 0; // Timestamp
    int8_t lastRSSI = 0;
    
    WiFiCredential() {}
    WiFiCredential(const String& s, const String& p, int8_t prio = 0) 
        : ssid(s), password(p), priority(prio) {}
};

// Network scan result
struct NetworkInfo {
    String ssid;
    int8_t rssi;
    uint8_t encryption;
    uint8_t channel;
    
    NetworkInfo() : rssi(0), encryption(0), channel(0) {}
    NetworkInfo(const String& s, int8_t r, uint8_t e, uint8_t c) 
        : ssid(s), rssi(r), encryption(e), channel(c) {}
};

// Config field definition
struct ConfigField {
    String id;
    String label;
    String type;
    String defaultValue;
    String placeholder;
    bool required = false;
    bool encrypted = false;
    int minLength = 0;
    int maxLength = 0;
    int min = 0;
    int max = 0;
    String pattern;
    String visibleIf;
    String* options = nullptr;
    size_t optionsCount = 0;
    
    ConfigField() {}
    ~ConfigField() {
        if (options) delete[] options;
    }
};

#if ION_ENABLE_DIAGNOSTICS
// Diagnostics data structure
struct DiagnosticsData {
    // Memory
    uint32_t heapFree = 0;
    uint32_t heapSize = 0;
    uint32_t heapMinFree = 0;
    
    // WiFi
    int8_t rssi = 0;
    uint32_t wifiReconnects = 0;
    uint32_t wifiConnectedTime = 0;
    
    // System
    uint32_t uptime = 0;
    uint32_t cpuFreq = 0;
    float cpuLoad = 0.0f;
    
    // Application
    uint32_t apiRequests = 0;
    uint32_t apiErrors = 0;
    uint32_t portalSessions = 0;
    
    // Errors
    uint32_t totalErrors = 0;
    String lastError;
};
#endif

// Version info
#define ION_VERSION_MAJOR 1
#define ION_VERSION_MINOR 0
#define ION_VERSION_PATCH 0
#define ION_VERSION_STRING "1.0.0"

} // namespace IonConnect

#endif // ION_TYPES_H

