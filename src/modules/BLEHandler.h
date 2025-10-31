#ifndef BLE_HANDLER_H
#define BLE_HANDLER_H

#include "../core/IonTypes.h"

#if ION_PLATFORM_ESP32 && ION_ENABLE_BLE

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "ConfigManager.h"
#include "WiFiConnectionCore.h"

namespace IonConnect {

// IonConnect BLE Service UUID
#define ION_BLE_SERVICE_UUID        "0000ff01-0000-1000-8000-00805f9b34fb"
#define ION_BLE_SCHEMA_UUID         "0000ff02-0000-1000-8000-00805f9b34fb"
#define ION_BLE_CONFIG_UUID         "0000ff03-0000-1000-8000-00805f9b34fb"
#define ION_BLE_STATUS_UUID         "0000ff04-0000-1000-8000-00805f9b34fb"
#define ION_BLE_CONTROL_UUID        "0000ff05-0000-1000-8000-00805f9b34fb"

/**
 * @brief BLE provisioning handler for ESP32
 * 
 * Provides GATT service for mobile app provisioning via Bluetooth
 */
class BLEHandler : public BLEServerCallbacks, public BLECharacteristicCallbacks {
public:
    BLEHandler(ConfigManager* config, WiFiConnectionCore* wifi);
    ~BLEHandler();
    
    bool begin(const String& deviceName);
    void handle();
    bool stop();
    bool isRunning();
    
    void notifyStatus(const String& status);
    
    // Callbacks
    void onConfigReceived(std::function<void()> cb);
    
private:
    ConfigManager* config;
    WiFiConnectionCore* wifi;
    
    BLEServer* bleServer;
    BLEService* bleService;
    BLECharacteristic* schemaChar;
    BLECharacteristic* configChar;
    BLECharacteristic* statusChar;
    BLECharacteristic* controlChar;
    
    bool running;
    bool deviceConnected;
    String deviceName;
    
    std::function<void()> configReceivedCallback;
    
    // BLEServerCallbacks
    void onConnect(BLEServer* pServer) override;
    void onDisconnect(BLEServer* pServer) override;
    
    // BLECharacteristicCallbacks
    void onWrite(BLECharacteristic* pCharacteristic) override;
    
    void handleConfigWrite(const String& json);
    void handleControlCommand(const String& cmd);
    void setupCharacteristics();
};

} // namespace IonConnect

#endif // ION_PLATFORM_ESP32 && ION_ENABLE_BLE
#endif // BLE_HANDLER_H

