#include "BLEHandler.h"

#if ION_PLATFORM_ESP32 && ION_ENABLE_BLE

#include "../utils/Logger.h"
#include <ArduinoJson.h>

namespace IonConnect {

BLEHandler::BLEHandler(ConfigManager* config, WiFiConnectionCore* wifi)
    : config(config), wifi(wifi), running(false), deviceConnected(false) {
}

BLEHandler::~BLEHandler() {
    stop();
}

bool BLEHandler::begin(const String& name) {
    if (running) {
        ION_LOG_W("BLE already running");
        return true;
    }
    
    deviceName = name;
    
    ION_LOG("Starting BLE: %s", deviceName.c_str());
    
    // Initialize BLE
    BLEDevice::init(deviceName.c_str());
    
    // Create BLE Server
    bleServer = BLEDevice::createServer();
    bleServer->setCallbacks(this);
    
    // Create BLE Service
    bleService = bleServer->createService(ION_BLE_SERVICE_UUID);
    
    setupCharacteristics();
    
    // Start the service
    bleService->start();
    
    // Start advertising
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(ION_BLE_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    
    running = true;
    ION_LOG("BLE advertising started");
    
    return true;
}

void BLEHandler::handle() {
    // BLE is event-driven, nothing to do in loop
}

bool BLEHandler::stop() {
    if (!running) return false;
    
    BLEDevice::deinit(true);
    running = false;
    
    ION_LOG("BLE stopped");
    return true;
}

bool BLEHandler::isRunning() {
    return running;
}

void BLEHandler::notifyStatus(const String& status) {
    if (!running || !deviceConnected) return;
    
    statusChar->setValue(status.c_str());
    statusChar->notify();
    
    ION_LOG("BLE status notify: %s", status.c_str());
}

void BLEHandler::onConfigReceived(std::function<void()> cb) {
    configReceivedCallback = cb;
}

void BLEHandler::onConnect(BLEServer* pServer) {
    deviceConnected = true;
    ION_LOG("BLE client connected");
}

void BLEHandler::onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    ION_LOG("BLE client disconnected");
    
    // Restart advertising
    delay(500);
    BLEDevice::startAdvertising();
    ION_LOG("BLE advertising restarted");
}

void BLEHandler::onWrite(BLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    
    if (value.length() > 0) {
        String data = String(value.c_str());
        
        if (pCharacteristic->getUUID().toString() == ION_BLE_CONFIG_UUID) {
            handleConfigWrite(data);
        } else if (pCharacteristic->getUUID().toString() == ION_BLE_CONTROL_UUID) {
            handleControlCommand(data);
        }
    }
}

void BLEHandler::handleConfigWrite(const String& json) {
    ION_LOG("BLE config received");
    
    DynamicJsonDocument doc(ION_JSON_BUFFER_SIZE);
    DeserializationError error = deserializeJson(doc, json);
    
    if (error) {
        ION_LOG_E("Failed to parse BLE config");
        notifyStatus("{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
        return;
    }
    
    JsonObject root = doc.as<JsonObject>();
    
    // Save each field
    for (JsonPair kv : root) {
        config->set(kv.key().c_str(), kv.value().as<String>());
    }
    
    // Validate
    if (!config->validate()) {
        notifyStatus("{\"status\":\"error\",\"message\":\"Validation failed\"}");
        return;
    }
    
    // Save to storage
    if (!config->save()) {
        notifyStatus("{\"status\":\"error\",\"message\":\"Failed to save\"}");
        return;
    }
    
    notifyStatus("{\"status\":\"success\",\"message\":\"Configuration saved\"}");
    
    // Callback
    if (configReceivedCallback) {
        configReceivedCallback();
    }
    
    // Attempt connection
    String ssid = config->get("wifi_ssid");
    String pass = config->get("wifi_pass");
    
    if (!ssid.isEmpty()) {
        notifyStatus("{\"status\":\"connecting\",\"ssid\":\"" + ssid + "\"}");
        wifi->connect(ssid, pass);
    }
}

void BLEHandler::handleControlCommand(const String& cmd) {
    ION_LOG("BLE control: %s", cmd.c_str());
    
    DynamicJsonDocument doc(ION_JSON_BUFFER_SIZE / 2);
    DeserializationError error = deserializeJson(doc, cmd);
    
    if (error) {
        notifyStatus("{\"status\":\"error\",\"message\":\"Invalid command\"}");
        return;
    }
    
    String command = doc["command"] | "";
    
    if (command == "scan") {
        wifi->startScan();
        notifyStatus("{\"status\":\"scanning\"}");
        
        // Wait for scan to complete
        delay(3000);
        
        auto networks = wifi->getScanResults();
        DynamicJsonDocument resultDoc(ION_JSON_BUFFER_SIZE);
        JsonArray arr = resultDoc["networks"].to<JsonArray>();
        
        for (const auto& net : networks) {
            JsonObject obj = arr.createNestedObject();
            obj["ssid"] = net.ssid;
            obj["rssi"] = net.rssi;
            obj["encryption"] = net.encryption;
        }
        
        String result;
        serializeJson(resultDoc, result);
        notifyStatus(result);
        
    } else if (command == "connect") {
        String ssid = config->get("wifi_ssid");
        String pass = config->get("wifi_pass");
        if (!ssid.isEmpty()) {
            wifi->connect(ssid, pass);
            notifyStatus("{\"status\":\"connecting\"}");
        }
        
    } else if (command == "reboot") {
        notifyStatus("{\"status\":\"rebooting\"}");
        delay(1000);
        ESP.restart();
        
    } else if (command == "clear") {
        config->clear();
        notifyStatus("{\"status\":\"cleared\"}");
    }
}

void BLEHandler::setupCharacteristics() {
    // Schema characteristic (Read)
    schemaChar = bleService->createCharacteristic(
        ION_BLE_SCHEMA_UUID,
        BLECharacteristic::PROPERTY_READ
    );
    String schema = config->getSchemaJSON();
    schemaChar->setValue(schema.c_str());
    
    // Config characteristic (Write)
    configChar = bleService->createCharacteristic(
        ION_BLE_CONFIG_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    configChar->setCallbacks(this);
    
    // Status characteristic (Notify)
    statusChar = bleService->createCharacteristic(
        ION_BLE_STATUS_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    statusChar->addDescriptor(new BLE2902());
    statusChar->setValue("{\"status\":\"ready\"}");
    
    // Control characteristic (Write)
    controlChar = bleService->createCharacteristic(
        ION_BLE_CONTROL_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    controlChar->setCallbacks(this);
    
    ION_LOG("BLE characteristics configured");
}

} // namespace IonConnect

#endif // ION_PLATFORM_ESP32 && ION_ENABLE_BLE

