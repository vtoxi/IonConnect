# IonConnect for ESP-01 and Low-Memory Devices

This guide explains how to use IonConnect on low-memory devices like ESP-01 (ESP8266 with ~80KB RAM, ~1MB flash).

## Overview

IonConnect provides a **minimal mode** specifically optimized for constrained devices:

### Memory Optimizations

| Feature | Standard Mode | Minimal Mode | Savings |
|---------|--------------|--------------|---------|
| JSON Schema Buffer | 4096 bytes | 1024 bytes | **75%** |
| JSON Config Buffer | 2048 bytes | 512 bytes | **75%** |
| JSON Operation Buffer | 2048 bytes | 512 bytes | **75%** |
| Embedded HTML/JS | ~15 KB | ~6 KB | **60%** |
| Max Networks Stored | 10 | 3 | **70%** |
| Max Config Fields | 32 | 8 | **75%** |

### Features Disabled in Minimal Mode

- ❌ BLE Provisioning (ESP32 only anyway)
- ❌ OTA Updates (requires ~100KB+ free heap)
- ❌ mDNS Discovery
- ❌ Diagnostics Panel
- ❌ Plugin System
- ❌ LittleFS Storage (uses EEPROM instead)
- ❌ Server-Sent Events (SSE)
- ❌ Backup/Restore UI
- ✅ Core WiFi provisioning via captive portal

## Quick Start

### Option 1: PlatformIO (Recommended)

Create or update your `platformio.ini`:

```ini
[env:esp01]
platform = espressif8266
board = esp01_1m
framework = arduino

; Core libraries
lib_deps = 
    https://github.com/vtoxi/IonConnect.git#main
    ESPAsyncTCP
    ESPAsyncWebServer
    ArduinoJson@^6.21.0

; Enable minimal mode for low-memory devices
build_flags = 
    -DION_MINIMAL_MODE=1
    -DPIO_FRAMEWORK_ARDUINO_LWIP2_LOW_MEMORY
    -D PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48
    ; Optional: Further reduce memory
    -D DEBUG_ESP_PORT=
    -D NDEBUG

; Optimize for size
build_unflags = 
    -Os
build_flags = 
    ${env.build_flags}
    -Os
    -ffunction-sections
    -fdata-sections
    -Wl,--gc-sections

monitor_speed = 115200
upload_speed = 115200
```

### Option 2: Arduino IDE

Add to your sketch **before** including IonConnect:

```cpp
// Must be defined BEFORE including IonConnect.h
#define ION_MINIMAL_MODE 1

#include <IonConnect.h>
```

## Example Sketch for ESP-01

```cpp
/*
 * IonConnect Minimal Example for ESP-01
 * 
 * This example demonstrates IonConnect running on ESP-01
 * with minimal memory footprint.
 */

// Enable minimal mode BEFORE including the library
#define ION_MINIMAL_MODE 1

#include <ESP8266WiFi.h>
#include <IonConnect.h>

IonConnectDevice ion;

void setup() {
    Serial.begin(115200);
    delay(100);
    
    Serial.println("\n\n=== IonConnect ESP-01 Minimal Mode ===");
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    
    // Initialize with minimal configuration
    if (!ion.init("ESP01-Device")) {
        Serial.println("Init failed!");
        return;
    }
    
    // Register callbacks
    ion.onConnect([]() {
        Serial.println("\n✓ WiFi Connected!");
        Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    });
    
    ion.onDisconnect([]() {
        Serial.println("✗ WiFi Disconnected");
    });
    
    ion.onPortalStart([]() {
        Serial.println("\n→ Portal Started");
        Serial.printf("Connect to: IonConnect-XXXX\n");
        Serial.printf("Go to: http://192.168.4.1\n");
        Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    });
    
    // Start IonConnect
    if (!ion.begin()) {
        Serial.println("Begin failed!");
        return;
    }
    
    Serial.println("✓ IonConnect started");
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
}

void loop() {
    ion.handle();  // Non-blocking
    
    // Your application code here
    // Keep it lightweight!
}
```

## Advanced Configuration

### Using Minimal Preset

```cpp
#include <IonConnect.h>

IonConnectDevice ion;

void setup() {
    // Use the minimal configuration preset
    IonConfig config = IonConfig::minimal();
    
    // Optionally customize
    config.portalTimeoutSeconds = 120;  // 2 minutes
    config.deviceName = "My ESP-01";
    
    ion.init("ESP01-Device", config);
    ion.begin();
}
```

### Custom Build Flags

Fine-tune memory usage with additional flags:

```ini
build_flags = 
    -DION_MINIMAL_MODE=1
    
    ; Customize buffer sizes (default shown)
    -DION_JSON_SCHEMA_SIZE=1024
    -DION_JSON_CONFIG_SIZE=512
    -DION_JSON_BUFFER_SIZE=512
    -DION_MAX_NETWORKS=3
    -DION_MAX_CONFIG_FIELDS=8
    
    ; Selectively disable features
    -DION_ENABLE_OTA=0
    -DION_ENABLE_MDNS=0
    -DION_ENABLE_DIAGNOSTICS=0
    -DION_USE_LITTLEFS=0
```

## Memory Budget Analysis

### ESP-01 Typical Usage

```
ESP-01 Total RAM: ~80KB

Baseline (ESP8266 SDK): ~25KB
IonConnect Core:        ~12KB (minimal mode)
AsyncWebServer:         ~15KB
Stack + Working:        ~18KB
Available for App:      ~10KB
```

### Tips to Maximize Available Memory

1. **Disable Serial Debug in Production**
   ```cpp
   #define NDEBUG
   ```

2. **Use PROGMEM for Strings**
   ```cpp
   const char myString[] PROGMEM = "Store in flash";
   ```

3. **Minimize String Usage**
   ```cpp
   // BAD - creates String objects
   String ip = WiFi.localIP().toString();
   
   // GOOD - use char arrays
   char ip[16];
   sprintf(ip, "%d.%d.%d.%d", 
           WiFi.localIP()[0], WiFi.localIP()[1], 
           WiFi.localIP()[2], WiFi.localIP()[3]);
   ```

4. **Reduce WiFi Buffer Sizes**
   ```cpp
   WiFi.setOutputPower(17);  // Reduce TX power (0-20.5dBm)
   ```

5. **Monitor Heap Usage**
   ```cpp
   void loop() {
       static uint32_t lastCheck = 0;
       if (millis() - lastCheck > 10000) {
           Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
           lastCheck = millis();
       }
       ion.handle();
   }
   ```

## Limitations in Minimal Mode

1. **No OTA Updates** - You must upload firmware via serial/USB
2. **No mDNS** - Access via IP address only (http://192.168.4.1)
3. **Limited Networks** - Can only store 3 WiFi networks
4. **Simple UI** - Basic provisioning interface without live updates
5. **No Diagnostics** - Cannot view system stats in web UI

## Troubleshooting

### Out of Memory Errors

If you see crashes or weird behavior:

1. **Check Free Heap**
   ```cpp
   Serial.printf("Heap: %d\n", ESP.getFreeHeap());
   ```
   Should be > 5KB during operation

2. **Reduce Buffer Sizes Further**
   ```cpp
   #define ION_JSON_SCHEMA_SIZE 768
   #define ION_JSON_CONFIG_SIZE 384
   ```

3. **Simplify Your Schema**
   - Use fewer config fields (max 8 in minimal mode)
   - Shorter field names
   - Avoid large default values

4. **Disable Features You Don't Need**
   ```cpp
   config.enableCaptivePortal = false;  // If not needed
   ```

### Portal Not Accessible

1. Connect to the AP (IonConnect-XXXX)
2. Try both:
   - http://192.168.4.1
   - http://192.168.4.1:80

3. Check firewall settings on your phone/laptop

### Connection Failures

1. **Increase timeouts** for slow networks:
   ```cpp
   config.connectionTimeoutMs = 20000;  // 20 seconds
   ```

2. **Check WiFi signal strength**
   - ESP-01 has weak antenna
   - Keep router close during setup

## Performance Comparison

### Startup Time
- **Standard Mode**: ~3-4 seconds
- **Minimal Mode**: ~2-3 seconds

### Portal Response Time
- **Standard Mode**: ~100-200ms (with SSE)
- **Minimal Mode**: ~200-300ms (polling)

### Memory Peak Usage
- **Standard Mode**: ~40KB peak
- **Minimal Mode**: ~20KB peak

## Example Projects

See the `examples/` directory for complete examples:

- `examples/10_ESP01_Minimal/` - Complete ESP-01 example
- `examples/11_LowPowerSensor/` - Battery-powered sensor with deep sleep

## Further Optimization

For even lower memory usage, consider:

1. **Remove AsyncWebServer** - Use basic ESP8266WebServer
2. **Static HTML** - Serve from PROGMEM instead of dynamic generation
3. **Minimal Schema** - Only SSID + Password fields
4. **Custom Build** - Fork IonConnect and remove unused code

## Support

- Issues: https://github.com/vtoxi/IonConnect/issues
- Discussions: https://github.com/vtoxi/IonConnect/discussions
- Tag: `low-memory` or `esp01`

---

**Note**: ESP-01 is a challenging platform. Test thoroughly and monitor memory usage!

