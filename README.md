# 🔌 IonConnect

**Next-generation WiFi provisioning library for ESP32 & ESP8266**

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-ESP32%20%7C%20ESP8266-blue)](https://github.com/vtoxi/IonConnect)
[![Version](https://img.shields.io/badge/version-1.0.0-green)](https://github.com/vtoxi/IonConnect/releases)

IonConnect is a modern, async WiFi provisioning library inspired by [tzapu/WiFiManager](https://github.com/tzapu/WiFiManager), reimagined with contemporary async patterns, enhanced security, plugin architecture, and a beautiful live-updating UI.

## ✨ Features

### Core Capabilities
- ⚡ **Async Architecture** - Non-blocking operation with ESPAsyncWebServer
- 🌐 **Captive Portal** - Auto-redirect for seamless configuration
- 📱 **Live Dashboard** - Real-time status updates via Server-Sent Events
- 🎨 **Modern UI** - Responsive design with TailwindCSS
- 🔒 **Secure** - Token/PIN authentication, encrypted credentials

### Advanced Features
- 📡 **Multi-Network Memory** - Auto-connect to strongest known network
- 🔄 **Smart Reconnect** - Exponential backoff with portal fallback
- 🛠️ **Dynamic Config** - JSON schema-driven UI generation
- 📊 **Diagnostics Panel** - Real-time system health metrics
- 🔌 **Plugin System** - Extensible architecture for custom features
- 📦 **Config Backup/Restore** - Export/import configuration
- 🔧 **OTA Updates** - Built-in firmware update support
- 🏷️ **mDNS + QR Code** - Quick access to device dashboard
- 📶 **BLE Provisioning** - Bluetooth fallback for ESP32 (optional)

## 🚀 Quick Start

### Installation

**Arduino IDE:**
1. Download the latest release
2. Sketch → Include Library → Add .ZIP Library

**PlatformIO:**
```ini
[env:esp32dev]
platform = espressif32
framework = arduino
lib_deps = 
    IonConnect
```

### Minimal Example

```cpp
#include <IonConnect.h>

IonConnect ion;

void setup() {
    Serial.begin(115200);
    
    // Initialize with device name
    ion.init("MyDevice");
    
    // Register connection callback
    ion.onConnect([]() {
        Serial.println("Connected!");
        Serial.println(WiFi.localIP());
    });
    
    // Start (connects or starts portal)
    ion.begin();
}

void loop() {
    ion.handle(); // Non-blocking
}
```

### What Happens

1. **First Boot**: No WiFi credentials → Captive portal starts
2. **User Connects**: To `IonConnect-XXXX` AP
3. **Browser Opens**: Auto-redirect to configuration page
4. **User Configures**: Selects network, enters password
5. **Device Connects**: Saves credentials and connects to WiFi
6. **Subsequent Boots**: Auto-connects using saved credentials

## 📋 Configuration

### Custom Configuration Schema

Define your own configuration fields with JSON schema:

```cpp
const char* customSchema = R"json({
  "fields": [
    {"id": "wifi_ssid", "label": "WiFi Network", "type": "text", "required": true},
    {"id": "wifi_pass", "label": "Password", "type": "password", "encrypted": true},
    {"id": "mqtt_host", "label": "MQTT Server", "type": "text", "default": "mqtt.local"},
    {"id": "mqtt_port", "label": "MQTT Port", "type": "number", "default": "1883"}
  ]
})json";

ion.loadConfigSchema(customSchema);

// Access custom values
String mqttHost = ion.getConfig("mqtt_host");
```

### Advanced Configuration

```cpp
IonConfig config;
config.deviceName = "Smart Sensor";
config.portalTimeoutSeconds = 300; // 5 minutes
config.maxReconnectAttempts = 5;
config.enableOTA = true;
config.enableDiagnostics = true;
config.portalPassword = "admin123"; // Optional security

ion.init("SmartSensor", config);
```

## 🔐 Security

### Portal Password Protection

```cpp
ion.setPortalPassword("admin123");
```

### API Token Authentication

```cpp
ion.setAccessToken("your-secret-token");
// API calls require: Authorization: Bearer your-secret-token
```

### Encrypted Credentials

WiFi passwords and sensitive fields are automatically encrypted using device-unique keys.

## 🌐 REST API

IonConnect exposes a comprehensive REST API:

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/status` | GET | WiFi status, IP, RSSI |
| `/api/config` | GET/POST | Configuration |
| `/api/networks` | GET | Scanned networks |
| `/api/scan` | POST | Trigger WiFi scan |
| `/api/export` | GET | Backup configuration |
| `/api/import` | POST | Restore configuration |
| `/api/diagnostics` | GET | System health metrics |
| `/api/reboot` | POST | Restart device |
| `/update` | GET/POST | OTA firmware update |

## 📱 BLE Provisioning (ESP32)

Enable Bluetooth provisioning for mobile apps:

```cpp
IonConfig config;
config.enableBLE = true;
ion.init("BLEDevice", config);
```

Mobile apps can connect via BLE and configure WiFi without joining the AP.

## 🔧 Plugin System

Extend IonConnect with custom plugins:

```cpp
class MQTTPlugin : public IonPlugin {
public:
    const char* getName() { return "MQTT"; }
    
    bool init(IonConnectBase* ion) {
        // Initialize plugin
        return true;
    }
    
    void handle() {
        // Called in loop()
    }
    
    void registerRoutes(AsyncWebServer* server) {
        server->on("/api/mqtt/status", HTTP_GET, [](AsyncWebServerRequest* req) {
            req->send(200, "application/json", "{\"status\":\"connected\"}");
        });
    }
};

MQTTPlugin mqttPlugin;
ion.registerPlugin(&mqttPlugin);
```

## 📊 Diagnostics

Access real-time system metrics:

```cpp
DiagnosticsData diag = ion.getDiagnostics();
Serial.printf("Heap Free: %d bytes\n", diag.heapFree);
Serial.printf("WiFi RSSI: %d dBm\n", diag.rssi);
Serial.printf("Uptime: %d seconds\n", diag.uptime);
```

## 🎯 Examples

- **01_BasicSetup** - Minimal configuration
- **02_CustomSchema** - Custom configuration fields
- **03_MultiNetwork** - Multiple WiFi credentials
- **04_BLEProvisioning** - Bluetooth setup (ESP32)
- **05_SecurePortal** - Password-protected portal
- **06_WithMQTT** - MQTT integration
- **07_CustomPlugin** - Plugin development
- **08_BackupRestore** - Config export/import
- **09_Diagnostics** - System monitoring

## 🛠️ Dependencies

- **ESPAsyncWebServer** - Async web server
- **DNSServer** - Captive portal
- **ArduinoJson** (^6.21.0) - JSON parsing
- **AsyncTCP** (ESP32) or **ESPAsyncTCP** (ESP8266)

## 📖 Documentation

- [API Reference](docs/API.md)
- [Plugin Development Guide](docs/PLUGINS.md)
- [Schema Format Reference](docs/SCHEMA.md)
- [Security Best Practices](docs/SECURITY.md)

## 🤝 Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## 📄 License

MIT License - see [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Inspired by [tzapu/WiFiManager](https://github.com/tzapu/WiFiManager) - The original and excellent WiFi configuration library
- Built with [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- UI powered by [TailwindCSS](https://tailwindcss.com/)

## 🌟 Star History

If you find IonConnect useful, please consider giving it a star! ⭐

---

**Made with ❤️ for the ESP32/ESP8266 community**

