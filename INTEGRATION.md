# IonConnect - Integration Guide

Complete guide for integrating IonConnect WiFi provisioning library into your ESP32/ESP8266 projects.

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Installation](#installation)
3. [Basic Integration](#basic-integration)
4. [API Reference](#api-reference)
5. [Configuration](#configuration)
6. [Examples](#examples)
7. [Platform-Specific Notes](#platform-specific-notes)
8. [Publishing the Library](#publishing-the-library)
9. [Troubleshooting](#troubleshooting)

---

## Quick Start

### 1. Add to platformio.ini

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    vtoxi/IonConnect@^1.0.2
    me-no-dev/AsyncTCP@^1.1.1
```

### 2. Basic Usage

```cpp
#include <IonConnect.h>

IonConnectDevice ion;

void setup() {
    Serial.begin(115200);
    
    // Initialize with device name
    ion.init("MyDevice");
    
    // Start (auto-connects or opens portal)
    ion.begin();
}

void loop() {
    // Handle WiFi and portal
    ion.handle();
}
```

### 3. First Run

- Device starts captive portal
- Connect to "IonConnect-XXXX" WiFi
- Browser opens to http://192.168.4.1
- Configure WiFi credentials
- Device connects automatically

---

## Installation

### PlatformIO (Recommended)

**Published Library:**
```ini
lib_deps = vtoxi/IonConnect@^1.0.2
```

**Local Development:**
```ini
lib_extra_dirs = ../IonConnect
```

### Arduino IDE

1. Open Library Manager
2. Search "IonConnect"
3. Install latest version

### Manual Installation

```bash
cd ~/Arduino/libraries
git clone https://github.com/vtoxi/IonConnect.git
```

---

## Basic Integration

### Minimal Example

```cpp
#include <IonConnect.h>

IonConnectDevice ion;

void setup() {
    Serial.begin(115200);
    ion.init("MyDevice");
    ion.begin();
}

void loop() {
    ion.handle();
}
```

### With Callbacks

```cpp
#include <IonConnect.h>

IonConnectDevice ion;

void setup() {
    Serial.begin(115200);
    
    // Register callbacks
    ion.onConnect([]() {
        Serial.println("WiFi Connected!");
        Serial.println(WiFi.localIP());
    });
    
    ion.onDisconnect([]() {
        Serial.println("WiFi Disconnected");
    });
    
    ion.onPortalStart([]() {
        Serial.println("Captive Portal Started");
        Serial.println("Connect to IonConnect-XXXX");
    });
    
    // Initialize
    ion.init("MyDevice");
    ion.begin();
}

void loop() {
    ion.handle();
}
```

### With Existing Credentials

```cpp
#include <IonConnect.h>

IonConnectDevice ion;

void setup() {
    Serial.begin(115200);
    
    ion.init("MyDevice");
    
    // Add known networks
    ion.addNetwork("HomeWiFi", "password123", 1);     // Priority 1 (highest)
    ion.addNetwork("WorkWiFi", "work_pass", 2);       // Priority 2
    
    ion.begin();  // Will try to connect to known networks
}

void loop() {
    ion.handle();
}
```

---

## API Reference

### Initialization

#### `bool init(const char* deviceName, const IonConfig& config = IonConfig())`

Initialize IonConnect with device name and optional configuration.

**Parameters:**
- `deviceName` - Unique device identifier (used for AP name, mDNS)
- `config` - Optional configuration struct (see Configuration section)

**Returns:** `true` if successful

**Example:**
```cpp
ion.init("MyDevice");

// With custom config
IonConfig config;
config.portalTimeoutSeconds = 300;
config.enableBLE = true;
ion.init("MyDevice", config);
```

---

#### `bool begin()`

Start IonConnect. Will attempt to connect to saved networks or start captive portal.

**Returns:** `true` if initialization successful

---

#### `void handle()`

Main loop handler. Call this in your `loop()` function.

**Must be called regularly** for WiFi management, captive portal, and callbacks.

```cpp
void loop() {
    ion.handle();  // Required!
}
```

---

### WiFi Management

#### `bool connect()`

Manually trigger WiFi connection attempt.

**Returns:** `true` if connection initiated

---

#### `bool disconnect()`

Disconnect from current WiFi network.

**Returns:** `true` if disconnection successful

---

#### `bool isConnected()`

Check if currently connected to WiFi.

**Returns:** `true` if connected

**Example:**
```cpp
if (ion.isConnected()) {
    Serial.println("IP: " + WiFi.localIP().toString());
}
```

---

#### `WiFiState getStatus()`

Get current WiFi connection state.

**Returns:** `WiFiState` enum value

**States:**
- `WiFiState::DISCONNECTED`
- `WiFiState::CONNECTING`
- `WiFiState::CONNECTED`
- `WiFiState::PORTAL_ACTIVE`
- `WiFiState::ERROR`

---

#### `bool addNetwork(const String& ssid, const String& pass, int priority = 0)`

Add a network to the known networks list.

**Parameters:**
- `ssid` - Network SSID
- `pass` - Network password
- `priority` - Connection priority (higher = tried first)

**Returns:** `true` if added successfully

**Example:**
```cpp
ion.addNetwork("HomeWiFi", "password123", 1);
ion.addNetwork("WorkWiFi", "work_pass", 2);
```

---

### Captive Portal

#### `bool startPortal(uint32_t timeoutSeconds = 0)`

Manually start the captive portal.

**Parameters:**
- `timeoutSeconds` - Portal timeout (0 = no timeout)

**Returns:** `true` if portal started

---

#### `bool stopPortal()`

Stop the captive portal.

**Returns:** `true` if portal stopped

---

#### `bool isPortalActive()`

Check if captive portal is currently active.

**Returns:** `true` if portal is running

---

### Configuration

#### `String getConfig(const String& key)`

Get a configuration value.

**Parameters:**
- `key` - Configuration key

**Returns:** Configuration value as String

---

#### `bool setConfig(const String& key, const String& value)`

Set a configuration value.

**Parameters:**
- `key` - Configuration key
- `value` - Configuration value

**Returns:** `true` if set successfully

---

#### `String exportConfig()`

Export all configuration as JSON string.

**Returns:** JSON string with all configuration

**Example:**
```cpp
String config = ion.exportConfig();
Serial.println(config);
// Save to file, send to server, etc.
```

---

#### `bool importConfig(const String& json)`

Import configuration from JSON string.

**Parameters:**
- `json` - JSON configuration string

**Returns:** `true` if imported successfully

---

#### `bool clearConfig()`

Clear all stored configuration.

**Returns:** `true` if cleared successfully

---

### Event Callbacks

#### `void onConnect(std::function<void()> callback)`

Register callback for WiFi connection event.

**Example:**
```cpp
ion.onConnect([]() {
    Serial.println("Connected!");
    // Start MQTT, HTTP, etc.
});
```

---

#### `void onDisconnect(std::function<void()> callback)`

Register callback for WiFi disconnection event.

**Example:**
```cpp
ion.onDisconnect([]() {
    Serial.println("Disconnected!");
    // Stop services, save state, etc.
});
```

---

#### `void onConfigSaved(std::function<void()> callback)`

Register callback for when configuration is saved (via portal or API).

---

#### `void onPortalStart(std::function<void()> callback)`

Register callback for when captive portal starts.

---

#### `void onPortalTimeout(std::function<void()> callback)`

Register callback for when captive portal times out.

---

#### `void onError(std::function<void(IonError, const char*)> callback)`

Register callback for error events.

**Parameters:**
- `error` - IonError enum
- `message` - Error description

**Example:**
```cpp
ion.onError([](IonError error, const char* msg) {
    Serial.printf("Error %d: %s\n", error, msg);
});
```

---

### Security

#### `void setPortalPassword(const String& password)`

Set password for captive portal access.

**Parameters:**
- `password` - Portal password

---

#### `void setAccessToken(const String& token)`

Set API access token for REST endpoints.

**Parameters:**
- `token` - API access token

---

### Diagnostics

#### `DiagnosticsData getDiagnostics()`

Get system diagnostics (memory, WiFi stats, uptime).

**Returns:** `DiagnosticsData` struct

**Example:**
```cpp
auto diag = ion.getDiagnostics();
Serial.printf("Free Heap: %d bytes\n", diag.freeHeap);
Serial.printf("RSSI: %d dBm\n", diag.rssi);
```

---

## Configuration

### IonConfig Structure

```cpp
struct IonConfig {
    // Portal settings
    uint32_t portalTimeoutSeconds = 300;  // 5 minutes
    bool autoReconnect = true;
    uint32_t connectionTimeoutMs = 10000; // 10 seconds
    
    // Features
    bool enableBLE = true;        // BLE provisioning (ESP32 only)
    bool enableOTA = true;        // OTA updates
    bool enableMDNS = true;       // mDNS discovery
    bool enableDiagnostics = true;
    
    // Security
    String portalPassword = "";   // Optional portal password
    String accessToken = "";      // Optional API token
    
    // Network
    bool useStaticIP = false;
    IPAddress staticIP;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress dns1;
    IPAddress dns2;
};
```

### Example with Custom Config

```cpp
IonConfig config;
config.portalTimeoutSeconds = 600;      // 10 minute timeout
config.autoReconnect = true;
config.enableBLE = true;
config.enableOTA = true;
config.portalPassword = "admin123";     // Protect portal
config.accessToken = "secret_token";    // Protect API

ion.init("MyDevice", config);
```

---

## Examples

### Example 1: Basic Water Monitor

```cpp
#include <IonConnect.h>

IonConnectDevice ion;
bool isConfigured = false;

void setup() {
    Serial.begin(115200);
    
    ion.onConnect([]() {
        Serial.println("WiFi Connected!");
        isConfigured = true;
        // Start your application
    });
    
    ion.init("WaterMonitor");
    ion.begin();
}

void loop() {
    ion.handle();
    
    if (isConfigured) {
        // Your application logic
        readSensor();
        publishData();
    }
    
    delay(100);
}
```

---

### Example 2: With Configuration Management

```cpp
#include <IonConnect.h>
#include <Preferences.h>

IonConnectDevice ion;
Preferences prefs;

struct AppConfig {
    String mqttBroker;
    int mqttPort;
    String deviceId;
};

AppConfig appConfig;

void setup() {
    Serial.begin(115200);
    
    // Load app configuration
    prefs.begin("myapp", false);
    appConfig.mqttBroker = prefs.getString("mqtt_broker", "mqtt.example.com");
    appConfig.mqttPort = prefs.getInt("mqtt_port", 1883);
    appConfig.deviceId = prefs.getString("device_id", "device_001");
    
    // Setup IonConnect
    ion.onConnect([]() {
        Serial.println("WiFi Connected!");
        connectMQTT();
    });
    
    ion.init(appConfig.deviceId.c_str());
    ion.begin();
}

void loop() {
    ion.handle();
    // Your logic
}

void connectMQTT() {
    // Connect to MQTT using appConfig.mqttBroker, etc.
}
```

---

### Example 3: Multi-Network Priority

```cpp
#include <IonConnect.h>

IonConnectDevice ion;

void setup() {
    Serial.begin(115200);
    
    ion.init("MyDevice");
    
    // Add networks with priorities
    ion.addNetwork("HomeWiFi", "home_pass", 1);      // Try first
    ion.addNetwork("WorkWiFi", "work_pass", 2);      // Try second
    ion.addNetwork("GuestWiFi", "guest_pass", 3);    // Try third
    
    ion.begin();
}

void loop() {
    ion.handle();
}
```

---

### Example 4: With Custom Web Portal

```cpp
#include <IonConnect.h>

IonConnectDevice ion;

void setup() {
    Serial.begin(115200);
    
    // Custom configuration
    IonConfig config;
    config.portalTimeoutSeconds = 0;        // No timeout
    config.portalPassword = "admin123";     // Protect portal
    config.enableMDNS = true;
    
    ion.init("MyDevice", config);
    
    // Callbacks
    ion.onPortalStart([]() {
        Serial.println("Portal started");
        Serial.println("Connect to: IonConnect-XXXX");
        Serial.println("Access: http://192.168.4.1");
    });
    
    ion.onConfigSaved([]() {
        Serial.println("Config saved, restarting...");
        delay(1000);
        ESP.restart();
    });
    
    ion.begin();
}

void loop() {
    ion.handle();
}
```

---

## Platform-Specific Notes

### ESP32 Classic

**Full Feature Support:**
- ✅ WiFi provisioning
- ✅ BLE provisioning
- ✅ Captive portal
- ✅ REST API
- ✅ OTA updates
- ✅ Diagnostics
- ✅ mDNS

**Memory:**
- Flash: ~1.2 MB firmware typical
- RAM: ~12 KB during operation

**BLE Provisioning:**
```cpp
IonConfig config;
config.enableBLE = true;  // Enable BLE
ion.init("MyDevice", config);
```

---

### ESP32-S2

**Supported Features:**
- ✅ WiFi provisioning
- ❌ BLE provisioning (no BLE hardware)
- ✅ Captive portal
- ✅ REST API
- ✅ OTA updates
- ✅ Diagnostics
- ✅ mDNS

**Differences:**
- Single core (vs dual on ESP32)
- No BLE support
- Native USB CDC

---

### ESP8266

**Supported Features:**
- ✅ WiFi provisioning
- ❌ BLE provisioning
- ✅ Captive portal (optimized for limited RAM)
- ✅ REST API
- ✅ OTA updates
- ✅ Diagnostics
- ✅ mDNS

**Memory Considerations:**
- Limited RAM (~80 KB available)
- Web assets compressed and stored in PROGMEM
- Async patterns minimize memory footprint

**platformio.ini adjustments:**
```ini
[env:d1_mini]
platform = espressif8266
board = d1_mini
lib_deps = 
    vtoxi/IonConnect@^1.0.2
    me-no-dev/ESPAsyncTCP@^1.2.2  # Note: ESPAsyncTCP not AsyncTCP
build_flags = 
    -DESP8266
    -DVTABLES_IN_FLASH           # Save RAM
    -DBEARSSL_SSL_BASIC          # Reduce TLS footprint
```

---

## Publishing the Library

### Prerequisites

1. **PlatformIO Account**
   ```bash
   pio account login
   ```

2. **Verify Library Structure**
   ```bash
   # Required files:
   library.json
   library.properties
   src/IonConnect.h
   README.md
   ```

### Publish Command

```bash
cd /path/to/IonConnect
pio pkg publish --type=library --owner=vtoxi
```

### Update Version

Before republishing, bump version in:

**library.json:**
```json
{
  "version": "1.0.3"
}
```

**library.properties:**
```
version=1.0.3
```

### Verify Publication

https://registry.platformio.org/libraries/vtoxi/IonConnect

---

## Troubleshooting

### Build Errors

**"IonConnect.h: No such file"**
```bash
# Verify library installed
pio pkg list

# Reinstall if needed
pio pkg install

# Or clean and rebuild
pio run -t clean
pio run
```

**"undefined reference to IonConnect..."**
```bash
# Check platformio.ini includes library
# ESP32:
lib_deps = 
    vtoxi/IonConnect@^1.0.2
    me-no-dev/AsyncTCP@^1.1.1

# ESP8266:
lib_deps = 
    vtoxi/IonConnect@^1.0.2
    me-no-dev/ESPAsyncTCP@^1.2.2  # Different library!
```

---

### Runtime Issues

**Captive Portal Not Appearing**

1. **Check credentials cleared:**
   ```cpp
   ion.clearConfig();  // Force portal on next boot
   ```

2. **Verify AP is broadcasting:**
   - Scan for "IonConnect-XXXX" network
   - Try different device/phone

3. **Check Serial output:**
   ```
   Should see: "Captive Portal Started"
   ```

4. **Manual access:**
   - Connect to AP
   - Navigate to http://192.168.4.1

---

**WiFi Won't Connect**

1. **Check credentials:**
   - SSID correct
   - Password correct
   - 2.4 GHz network (not 5 GHz)

2. **Check signal strength:**
   ```cpp
   Serial.println(WiFi.RSSI());  // Should be > -80 dBm
   ```

3. **Check router settings:**
   - MAC filtering disabled
   - DHCP enabled
   - Not at max client limit

---

**Memory Issues (ESP8266)**

1. **Monitor heap:**
   ```cpp
   Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
   ```

2. **Reduce features:**
   ```cpp
   IonConfig config;
   config.enableDiagnostics = false;
   config.enableOTA = false;
   ```

3. **Use build flags:**
   ```ini
   build_flags = 
       -DVTABLES_IN_FLASH
       -DBEARSSL_SSL_BASIC
   ```

---

### Portal Issues

**Portal Timeout**

```cpp
IonConfig config;
config.portalTimeoutSeconds = 0;  // No timeout
ion.init("MyDevice", config);
```

**Portal Password Not Working**

```cpp
// Set password correctly
ion.setPortalPassword("admin123");

// Or in config
IonConfig config;
config.portalPassword = "admin123";
ion.init("MyDevice", config);
```

---

### Debugging

**Enable Debug Output:**

```cpp
#define ION_DEBUG 1
#include <IonConnect.h>
```

**Check Connection State:**

```cpp
void loop() {
    ion.handle();
    
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 5000) {
        WiFiState state = ion.getStatus();
        Serial.printf("WiFi State: %d\n", state);
        Serial.printf("Connected: %d\n", ion.isConnected());
        Serial.printf("Portal Active: %d\n", ion.isPortalActive());
        lastCheck = millis();
    }
}
```

---

## Advanced Topics

### Custom Configuration Schema

Create custom configuration fields in the web portal:

```cpp
const char* customSchema = R"json({
  "fields": [
    {
      "name": "mqtt_broker",
      "label": "MQTT Broker",
      "type": "text",
      "default": "mqtt.example.com"
    },
    {
      "name": "mqtt_port",
      "label": "MQTT Port",
      "type": "number",
      "default": "1883"
    }
  ]
})json";

ion.loadConfigSchema(customSchema);
```

### REST API Endpoints

When portal or WiFi active, REST API available:

**GET /api/status**
- Returns WiFi status, IP, RSSI

**POST /api/config**
- Set configuration values

**GET /api/diagnostics**
- System health metrics

**POST /api/restart**
- Restart device

Example:
```bash
curl http://192.168.4.1/api/status
```

---

## Support

- **Issues:** https://github.com/vtoxi/IonConnect/issues
- **Examples:** See `/examples/` folder
- **Documentation:** This file
- **Library Registry:** https://registry.platformio.org/libraries/vtoxi/IonConnect

---

## License

MIT License - See LICENSE file for details

