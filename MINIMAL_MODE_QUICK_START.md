# IonConnect Minimal Mode - Quick Start

**For ESP-01 and other low-RAM ESP8266 devices**

## 🚀 Quick Setup (3 Steps)

### 1. Add Build Flag

**PlatformIO** (`platformio.ini`):
```ini
build_flags = -DION_MINIMAL_MODE=1
```

**Arduino IDE** (top of sketch):
```cpp
#define ION_MINIMAL_MODE 1
```

### 2. Use Minimal Sketch

```cpp
#define ION_MINIMAL_MODE 1
#include <IonConnect.h>

IonConnectDevice ion;

void setup() {
    Serial.begin(115200);
    
    // Use minimal config
    IonConfig config = IonConfig::minimal();
    config.deviceName = "ESP01";
    
    ion.init("ESP01-Device", config);
    
    // Setup callbacks
    ion.onConnect([]() {
        Serial.println("Connected!");
    });
    
    ion.begin();
}

void loop() {
    ion.handle();
}
```

### 3. Upload & Connect

1. Upload sketch to ESP-01
2. Connect to WiFi: **IonConnect-XXXX**
3. Open: **http://192.168.4.1**
4. Enter WiFi credentials
5. Done! ✓

## 📊 What You Get

| Standard Mode | Minimal Mode |
|---------------|--------------|
| ~40KB RAM | ~20KB RAM (**50% less**) |
| 15KB HTML | 6KB HTML (**60% less**) |
| Full features | Core WiFi only |
| ESP32/ESP8266 | **ESP-01 optimized** |

## ✅ Included Features

- ✅ WiFi provisioning via captive portal
- ✅ Automatic reconnection
- ✅ Network scanning
- ✅ Credentials storage (EEPROM)
- ✅ Simple web UI
- ✅ Event callbacks

## ❌ Removed Features

- ❌ OTA updates
- ❌ mDNS discovery
- ❌ Diagnostics panel
- ❌ BLE provisioning
- ❌ Plugin system
- ❌ Live status updates (SSE)

## 📝 Complete platformio.ini

```ini
[env:esp01]
platform = espressif8266
board = esp01_1m
framework = arduino

lib_deps = 
    IonConnect
    ESPAsyncTCP
    ESPAsyncWebServer
    ArduinoJson

build_flags = 
    -DION_MINIMAL_MODE=1
    -DPIO_FRAMEWORK_ARDUINO_LWIP2_LOW_MEMORY
    -D PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48

monitor_speed = 115200
```

## 🔍 Monitor Memory Usage

```cpp
void loop() {
    ion.handle();
    
    // Print free heap every 10 seconds
    static uint32_t last = 0;
    if (millis() - last > 10000) {
        Serial.printf("Free RAM: %d bytes\n", ESP.getFreeHeap());
        last = millis();
    }
}
```

**Keep free heap above 5KB for stable operation!**

## 📚 Full Documentation

- **Complete Guide**: [docs/ESP01_LOW_MEMORY.md](docs/ESP01_LOW_MEMORY.md)
- **Example Code**: [examples/10_ESP01_Minimal/](examples/10_ESP01_Minimal/)
- **Implementation Details**: [MINIMAL_MODE_SUMMARY.md](MINIMAL_MODE_SUMMARY.md)

## ❓ Troubleshooting

### Out of Memory?
```ini
build_flags = 
    -DION_JSON_SCHEMA_SIZE=768
    -DION_JSON_CONFIG_SIZE=384
```

### Upload Fails?
- Hold GPIO0 to GND during upload
- Lower upload speed: `upload_speed = 9600`

### Portal Not Accessible?
- Ensure connected to "IonConnect-XXXX"
- Try http://192.168.4.1 explicitly
- Disable VPN on phone/laptop

## 💡 Tips

1. **Monitor heap** - Should stay > 5KB
2. **Use PROGMEM** - Store strings in flash
3. **Avoid String** - Use char arrays
4. **Keep it simple** - Limited resources!

## 🎯 Typical Memory Budget

```
ESP-01 (80KB RAM):
├─ ESP8266 SDK:     25KB
├─ IonConnect:      12KB ← Minimal mode
├─ AsyncWebServer:  15KB
├─ Stack/Working:   18KB
└─ Your App:        10KB ✓ Available
```

---

**Ready to deploy? Upload and connect! 🚀**

