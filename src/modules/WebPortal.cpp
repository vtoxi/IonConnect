#include "WebPortal.h"
#include "../utils/Logger.h"
#include <ArduinoJson.h>

namespace IonConnect {

WebPortal::WebPortal(ConfigManager* config, WiFiConnectionCore* wifi, SecurityManager* security)
    : config(config), wifi(wifi), security(security), running(false), port(80) {
    
    dnsHandler = new DNSHandler();
    assetManager = new AssetManager();
    server = new AsyncWebServer(80);
    events = new AsyncEventSource("/api/events");
    
    #if ION_ENABLE_DIAGNOSTICS
    diagnostics = nullptr;
    #endif
}

WebPortal::~WebPortal() {
    stop();
    delete dnsHandler;
    delete assetManager;
    delete events;
    delete server;
}

bool WebPortal::begin(uint16_t p) {
    if (running) {
        ION_LOG_W("WebPortal already running");
        return true;
    }
    
    port = p;
    
    // Initialize asset manager
    assetManager->begin();
    
    // Setup routes
    setupRoutes();
    
    // Start DNS handler for captive portal
    dnsHandler->start(WiFi.softAPIP());
    
    // Start server
    server->begin();
    running = true;
    
    ION_LOG("WebPortal started on port %d", port);
    return true;
}

void WebPortal::handle() {
    if (running) {
        dnsHandler->handle();
    }
}

bool WebPortal::stop() {
    if (!running) return false;
    
    dnsHandler->stop();
    server->end();
    assetManager->end();
    running = false;
    
    ION_LOG("WebPortal stopped");
    return true;
}

bool WebPortal::isRunning() {
    return running;
}

uint16_t WebPortal::getPort() {
    return port;
}

void WebPortal::broadcastStatus(const String& state, const String& ssid, 
                                const String& ip, const String& message) {
    if (!running || !events) return;
    
    String json = "{\"state\":\"" + state + "\"";
    if (!ssid.isEmpty()) json += ",\"ssid\":\"" + ssid + "\"";
    if (!ip.isEmpty()) json += ",\"ip\":\"" + ip + "\"";
    if (!message.isEmpty()) json += ",\"message\":\"" + message + "\"";
    json += "}";
    
    events->send(json.c_str(), "status", millis());
    ION_LOG("Broadcast status: %s", state.c_str());
}

#if ION_ENABLE_DIAGNOSTICS
void WebPortal::setDiagnosticsCollector(DiagnosticsCollector* diag) {
    diagnostics = diag;
}
#endif

void WebPortal::setupRoutes() {
    // Add SSE handler
    server->addHandler(events);
    
    setupPortalRoutes();
    setupAPIRoutes();
    setupSSE();
    
    // 404 handler
    server->onNotFound([this](AsyncWebServerRequest* request) {
        // Redirect to root for captive portal
        request->redirect("/");
    });
}

void WebPortal::setupPortalRoutes() {
    server->on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleRoot(request);
    });
    
    server->on("/script.js", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleJS(request);
    });
    
    server->on("/generate_204", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->redirect("/");
    });
    
    server->on("/fwlink", HTTP_GET, [this](AsyncWebServerRequest* request) {
        request->redirect("/");
    });
}

void WebPortal::setupAPIRoutes() {
    server->on("/api/schema", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleSchema(request);
    });
    
    server->on("/api/config", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleConfig(request);
    });
    
    server->on("/api/config", HTTP_POST, [](AsyncWebServerRequest* request) {},
        NULL, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
            if (index + len == total) {
                handleConfigPost(request, data, total);
            }
        });
    
    server->on("/api/scan", HTTP_POST, [this](AsyncWebServerRequest* request) {
        handleScan(request);
    });
    
    server->on("/api/networks", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleNetworks(request);
    });
    
    server->on("/api/status", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleStatus(request);
    });
    
    server->on("/api/clear", HTTP_POST, [this](AsyncWebServerRequest* request) {
        handleClear(request);
    });
    
    server->on("/api/export", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleExport(request);
    });
    
    server->on("/api/import", HTTP_POST, [](AsyncWebServerRequest* request) {},
        NULL, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
            if (index + len == total) {
                handleImport(request, data, total);
            }
        });
    
    server->on("/api/reboot", HTTP_POST, [this](AsyncWebServerRequest* request) {
        handleReboot(request);
    });
    
    server->on("/api/info", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleInfo(request);
    });
    
    #if ION_ENABLE_DIAGNOSTICS
    server->on("/api/diagnostics", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleDiagnostics(request);
    });
    #endif
}

void WebPortal::setupSSE() {
    events->onConnect([](AsyncEventSourceClient* client) {
        ION_LOG("SSE client connected");
        client->send("connected", "status", millis());
    });
}

void WebPortal::handleRoot(AsyncWebServerRequest* request) {
    #if ION_ENABLE_DIAGNOSTICS
    if (diagnostics) diagnostics->incrementCounter("api_requests");
    #endif
    
    String html = assetManager->loadHTML();
    request->send(200, "text/html", html);
}

void WebPortal::handleJS(AsyncWebServerRequest* request) {
    String js = assetManager->loadJS();
    request->send(200, "application/javascript", js);
}

void WebPortal::handleSchema(AsyncWebServerRequest* request) {
    #if ION_ENABLE_DIAGNOSTICS
    if (diagnostics) diagnostics->incrementCounter("api_requests");
    #endif
    
    String schema = config->getSchemaJSON();
    sendJSON(request, schema);
}

void WebPortal::handleConfig(AsyncWebServerRequest* request) {
    #if ION_ENABLE_DIAGNOSTICS
    if (diagnostics) diagnostics->incrementCounter("api_requests");
    #endif
    
    // Don't send passwords
    DynamicJsonDocument doc(2048);
    JsonObject root = doc.to<JsonObject>();
    
    auto fields = config->getFields();
    for (auto field : fields) {
        if (field->type != "password" && !field->encrypted) {
            String value = config->get(field->id);
            if (!value.isEmpty()) {
                root[field->id] = value;
            }
        }
    }
    
    String json;
    serializeJson(doc, json);
    sendJSON(request, json);
}

void WebPortal::handleConfigPost(AsyncWebServerRequest* request, uint8_t* data, size_t len) {
    #if ION_ENABLE_DIAGNOSTICS
    if (diagnostics) diagnostics->incrementCounter("api_requests");
    #endif
    
    if (!checkAuth(request)) {
        sendError(request, "Unauthorized", 401);
        return;
    }
    
    String body = "";
    for (size_t i = 0; i < len; i++) {
        body += (char)data[i];
    }
    
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, body);
    
    if (error) {
        sendError(request, "Invalid JSON");
        return;
    }
    
    JsonObject root = doc.as<JsonObject>();
    
    // Save each field
    for (JsonPair kv : root) {
        config->set(kv.key().c_str(), kv.value().as<String>());
    }
    
    // Validate
    if (!config->validate()) {
        sendError(request, "Validation failed");
        return;
    }
    
    // Save to storage
    if (!config->save()) {
        sendError(request, "Failed to save");
        return;
    }
    
    sendJSON(request, "{\"success\":true}");
    
    // Attempt connection
    String ssid = config->get("wifi_ssid");
    String pass = config->get("wifi_pass");
    
    if (!ssid.isEmpty()) {
        broadcastStatus("connecting", ssid);
        wifi->connect(ssid, pass);
    }
}

void WebPortal::handleScan(AsyncWebServerRequest* request) {
    #if ION_ENABLE_DIAGNOSTICS
    if (diagnostics) diagnostics->incrementCounter("api_requests");
    #endif
    
    wifi->startScan();
    sendJSON(request, "{\"scanning\":true}");
}

void WebPortal::handleNetworks(AsyncWebServerRequest* request) {
    #if ION_ENABLE_DIAGNOSTICS
    if (diagnostics) diagnostics->incrementCounter("api_requests");
    #endif
    
    auto networks = wifi->getScanResults();
    
    DynamicJsonDocument doc(2048);
    JsonArray arr = doc["networks"].to<JsonArray>();
    
    for (const auto& net : networks) {
        JsonObject obj = arr.createNestedObject();
        obj["ssid"] = net.ssid;
        obj["rssi"] = net.rssi;
        obj["encryption"] = net.encryption;
        obj["channel"] = net.channel;
    }
    
    doc["timestamp"] = millis() / 1000;
    
    String json;
    serializeJson(doc, json);
    sendJSON(request, json);
}

void WebPortal::handleStatus(AsyncWebServerRequest* request) {
    DynamicJsonDocument doc(1024);
    
    doc["connected"] = wifi->isConnected();
    doc["ssid"] = wifi->getSSID();
    doc["ip"] = wifi->getIP().toString();
    doc["rssi"] = wifi->getRSSI();
    doc["uptime"] = wifi->getUptime();
    doc["portal_active"] = running;
    
    String json;
    serializeJson(doc, json);
    sendJSON(request, json);
}

void WebPortal::handleClear(AsyncWebServerRequest* request) {
    #if ION_ENABLE_DIAGNOSTICS
    if (diagnostics) diagnostics->incrementCounter("api_requests");
    #endif
    
    if (!checkAuth(request)) {
        sendError(request, "Unauthorized", 401);
        return;
    }
    
    config->clear();
    sendJSON(request, "{\"success\":true}");
}

void WebPortal::handleExport(AsyncWebServerRequest* request) {
    if (!checkAuth(request)) {
        sendError(request, "Unauthorized", 401);
        return;
    }
    
    String json = config->exportJSON();
    
    AsyncWebServerResponse* response = request->beginResponse(200, "application/json", json);
    response->addHeader("Content-Disposition", "attachment; filename=ionconnect-config.json");
    request->send(response);
}

void WebPortal::handleImport(AsyncWebServerRequest* request, uint8_t* data, size_t len) {
    if (!checkAuth(request)) {
        sendError(request, "Unauthorized", 401);
        return;
    }
    
    String body = "";
    for (size_t i = 0; i < len; i++) {
        body += (char)data[i];
    }
    
    if (config->importJSON(body)) {
        sendJSON(request, "{\"success\":true}");
    } else {
        sendError(request, "Import failed");
    }
}

void WebPortal::handleReboot(AsyncWebServerRequest* request) {
    if (!checkAuth(request)) {
        sendError(request, "Unauthorized", 401);
        return;
    }
    
    sendJSON(request, "{\"rebooting\":true}");
    delay(1000);
    ESP.restart();
}

void WebPortal::handleInfo(AsyncWebServerRequest* request) {
    DynamicJsonDocument doc(1024);
    
    #if ION_PLATFORM_ESP32
        doc["platform"] = "ESP32";
        doc["chip_model"] = ESP.getChipModel();
        doc["chip_revision"] = ESP.getChipRevision();
        doc["cpu_freq"] = ESP.getCpuFreqMHz();
        doc["flash_size"] = ESP.getFlashChipSize();
        doc["heap_size"] = ESP.getHeapSize();
    #elif ION_PLATFORM_ESP8266
        doc["platform"] = "ESP8266";
        doc["chip_id"] = ESP.getChipId();
        doc["cpu_freq"] = ESP.getCpuFreqMHz();
        doc["flash_size"] = ESP.getFlashChipSize();
    #endif
    
    doc["version"] = ION_VERSION_STRING;
    doc["heap_free"] = ESP.getFreeHeap();
    doc["uptime"] = millis() / 1000;
    
    String json;
    serializeJson(doc, json);
    sendJSON(request, json);
}

#if ION_ENABLE_DIAGNOSTICS
void WebPortal::handleDiagnostics(AsyncWebServerRequest* request) {
    if (diagnostics) {
        diagnostics->incrementCounter("api_requests");
        String json = diagnostics->getJSON();
        sendJSON(request, json);
    } else {
        sendError(request, "Diagnostics not available");
    }
}
#endif

bool WebPortal::checkAuth(AsyncWebServerRequest* request) {
    if (!security) return true;
    return security->authorizeRequest(request);
}

void WebPortal::sendJSON(AsyncWebServerRequest* request, const String& json, int code) {
    request->send(code, "application/json", json);
}

void WebPortal::sendError(AsyncWebServerRequest* request, const String& error, int code) {
    String json = "{\"error\":\"" + error + "\"}";
    request->send(code, "application/json", json);
}

} // namespace IonConnect

