#include "DiagnosticsCollector.h"
#include "../utils/Logger.h"

#if ION_ENABLE_DIAGNOSTICS

#if ION_PLATFORM_ESP32
    #include <WiFi.h>
#elif ION_PLATFORM_ESP8266
    #include <ESP8266WiFi.h>
#endif

namespace IonConnect {

DiagnosticsCollector::DiagnosticsCollector() : lastCollectTime(0) {
}

void DiagnosticsCollector::collect() {
    uint32_t now = millis();
    
    // Collect every 5 seconds
    if (now - lastCollectTime < 5000) {
        return;
    }
    
    lastCollectTime = now;
    
    collectMemoryStats();
    collectWiFiStats();
    collectSystemStats();
}

DiagnosticsData DiagnosticsCollector::getData() {
    collect();
    return data;
}

String DiagnosticsCollector::getJSON() {
    collect();
    
    String json = "{";
    json += "\"heapFree\":" + String(data.heapFree) + ",";
    json += "\"heapSize\":" + String(data.heapSize) + ",";
    json += "\"heapMinFree\":" + String(data.heapMinFree) + ",";
    json += "\"rssi\":" + String(data.rssi) + ",";
    json += "\"wifiReconnects\":" + String(data.wifiReconnects) + ",";
    json += "\"wifiConnectedTime\":" + String(data.wifiConnectedTime) + ",";
    json += "\"uptime\":" + String(data.uptime) + ",";
    json += "\"cpuFreq\":" + String(data.cpuFreq) + ",";
    json += "\"cpuLoad\":" + String(data.cpuLoad, 2) + ",";
    json += "\"apiRequests\":" + String(data.apiRequests) + ",";
    json += "\"apiErrors\":" + String(data.apiErrors) + ",";
    json += "\"portalSessions\":" + String(data.portalSessions) + ",";
    json += "\"totalErrors\":" + String(data.totalErrors) + ",";
    json += "\"lastError\":\"" + data.lastError + "\"";
    json += "}";
    
    return json;
}

void DiagnosticsCollector::incrementCounter(const char* counter) {
    if (strcmp(counter, "api_requests") == 0) {
        data.apiRequests++;
    } else if (strcmp(counter, "api_errors") == 0) {
        data.apiErrors++;
    } else if (strcmp(counter, "portal_sessions") == 0) {
        data.portalSessions++;
    } else if (strcmp(counter, "wifi_reconnects") == 0) {
        data.wifiReconnects++;
    }
}

void DiagnosticsCollector::recordError(const String& error) {
    data.totalErrors++;
    data.lastError = error;
    ION_LOG_E("Error recorded: %s", error.c_str());
}

void DiagnosticsCollector::collectMemoryStats() {
    #if ION_PLATFORM_ESP32
        data.heapFree = ESP.getFreeHeap();
        data.heapSize = ESP.getHeapSize();
        data.heapMinFree = ESP.getMinFreeHeap();
    #elif ION_PLATFORM_ESP8266
        data.heapFree = ESP.getFreeHeap();
        data.heapSize = 81920; // ESP8266 typical heap size
        data.heapMinFree = 0; // Not available on ESP8266
    #endif
}

void DiagnosticsCollector::collectWiFiStats() {
    if (WiFi.status() == WL_CONNECTED) {
        data.rssi = WiFi.RSSI();
    } else {
        data.rssi = 0;
    }
}

void DiagnosticsCollector::collectSystemStats() {
    data.uptime = millis() / 1000;
    
    #if ION_PLATFORM_ESP32
        data.cpuFreq = ESP.getCpuFreqMHz();
    #elif ION_PLATFORM_ESP8266
        data.cpuFreq = ESP.getCpuFreqMHz();
    #endif
    
    // Simple CPU load calculation (not very accurate)
    data.cpuLoad = 0.0f; // Would need more complex implementation
}

} // namespace IonConnect

#endif // ION_ENABLE_DIAGNOSTICS

