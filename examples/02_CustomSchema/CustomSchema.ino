/**
 * @file CustomSchema.ino
 * @brief Custom configuration schema example
 * 
 * This example demonstrates how to define custom configuration fields
 * for your IoT device. The schema dynamically generates the web UI.
 * 
 * Hardware: ESP32 or ESP8266
 */

#include <IonConnect.h>

IonConnect ion;

// Define custom configuration schema
const char* customSchema = R"json({
  "version": "1.0",
  "fields": [
    {
      "id": "wifi_ssid",
      "label": "WiFi Network",
      "type": "text",
      "required": true,
      "placeholder": "Enter SSID"
    },
    {
      "id": "wifi_pass",
      "label": "WiFi Password",
      "type": "password",
      "required": true,
      "encrypted": true,
      "minLength": 8
    },
    {
      "id": "device_name",
      "label": "Device Name",
      "type": "text",
      "default": "Smart Sensor",
      "maxLength": 32
    },
    {
      "id": "mqtt_enabled",
      "label": "Enable MQTT",
      "type": "checkbox",
      "default": "true"
    },
    {
      "id": "mqtt_host",
      "label": "MQTT Broker",
      "type": "text",
      "visible_if": "mqtt_enabled==true",
      "placeholder": "mqtt.example.com"
    },
    {
      "id": "mqtt_port",
      "label": "MQTT Port",
      "type": "number",
      "visible_if": "mqtt_enabled==true",
      "default": "1883",
      "min": 1,
      "max": 65535
    },
    {
      "id": "mqtt_user",
      "label": "MQTT Username",
      "type": "text",
      "visible_if": "mqtt_enabled==true"
    },
    {
      "id": "mqtt_pass",
      "label": "MQTT Password",
      "type": "password",
      "visible_if": "mqtt_enabled==true",
      "encrypted": true
    },
    {
      "id": "update_interval",
      "label": "Update Interval (seconds)",
      "type": "number",
      "default": "60",
      "min": 10,
      "max": 3600
    },
    {
      "id": "log_level",
      "label": "Log Level",
      "type": "select",
      "options": ["DEBUG", "INFO", "WARN", "ERROR"],
      "default": "INFO"
    }
  ]
})json";

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  IonConnect - Custom Schema Example");
    Serial.println("========================================\n");
    
    // Initialize IonConnect
    ion.init("SmartSensor");
    
    // Load custom schema
    if (!ion.loadConfigSchema(customSchema)) {
        Serial.println("✗ Failed to load schema!");
        return;
    }
    
    Serial.println("✓ Custom schema loaded");
    
    // Register callbacks
    ion.onConnect([]() {
        Serial.println("\n✓ WiFi Connected!");
        
        // Access custom configuration values
        String deviceName = ion.getConfig("device_name");
        String mqttEnabled = ion.getConfig("mqtt_enabled");
        String updateInterval = ion.getConfig("update_interval");
        String logLevel = ion.getConfig("log_level");
        
        Serial.println("\n📋 Configuration:");
        Serial.println("  Device Name: " + deviceName);
        Serial.println("  MQTT Enabled: " + mqttEnabled);
        Serial.println("  Update Interval: " + updateInterval + "s");
        Serial.println("  Log Level: " + logLevel);
        
        if (mqttEnabled == "true") {
            String mqttHost = ion.getConfig("mqtt_host");
            String mqttPort = ion.getConfig("mqtt_port");
            String mqttUser = ion.getConfig("mqtt_user");
            
            Serial.println("\n📡 MQTT Settings:");
            Serial.println("  Broker: " + mqttHost + ":" + mqttPort);
            Serial.println("  Username: " + mqttUser);
            
            // Here you would initialize your MQTT client
            // connectMQTT(mqttHost, mqttPort.toInt(), mqttUser, mqttPass);
        }
    });
    
    ion.onConfigSaved([]() {
        Serial.println("✓ Configuration saved!");
    });
    
    // Start IonConnect
    if (!ion.begin()) {
        Serial.println("✗ IonConnect initialization failed!");
        return;
    }
    
    Serial.println("✓ IonConnect started");
}

void loop() {
    ion.handle();
    
    // Your application logic here
    static uint32_t lastUpdate = 0;
    uint32_t interval = ion.getConfig("update_interval").toInt() * 1000;
    
    if (ion.isConnected() && millis() - lastUpdate > interval) {
        lastUpdate = millis();
        
        // Perform periodic tasks (e.g., send sensor data)
        Serial.println("📊 Sending sensor data...");
    }
}

