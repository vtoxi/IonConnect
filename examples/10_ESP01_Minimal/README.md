# ESP-01 Minimal Memory Example

This example demonstrates IonConnect running on ESP-01 with minimal memory footprint.

## Hardware Requirements

- ESP-01 or ESP-01S module
- 1MB flash minimum
- USB to Serial adapter (for programming)
- 3.3V power supply (stable, 500mA+)

## Features

- ✅ WiFi provisioning via captive portal
- ✅ Automatic reconnection
- ✅ Minimal HTML UI (~6KB)
- ✅ Low memory usage (~12KB)
- ❌ No OTA updates
- ❌ No mDNS
- ❌ No diagnostics panel
- ❌ No BLE provisioning

## Memory Usage

```
Total RAM: ~80KB

Allocations:
- ESP8266 SDK:      ~25KB
- IonConnect:       ~12KB (minimal mode)
- AsyncWebServer:   ~15KB
- Stack + Working:  ~18KB
- Available:        ~10KB for your app
```

## Setup Instructions

### Option 1: PlatformIO (Recommended)

1. Create `platformio.ini`:
```ini
[env:esp01_1m]
platform = espressif8266
board = esp01_1m
framework = arduino

lib_deps = 
    IonConnect
    ESPAsyncTCP
    ESPAsyncWebServer
    ArduinoJson@^6.21.0

build_flags = 
    -DION_MINIMAL_MODE=1
    -DPIO_FRAMEWORK_ARDUINO_LWIP2_LOW_MEMORY
    -D PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48

monitor_speed = 115200
upload_speed = 115200
```

2. Upload:
```bash
pio run -t upload -t monitor
```

### Option 2: Arduino IDE

1. Add to top of sketch:
```cpp
#define ION_MINIMAL_MODE 1
```

2. Board settings:
   - Board: "Generic ESP8266 Module"
   - Flash Size: "1MB (FS:64KB OTA:~470KB)"
   - Flash Mode: "DIO"
   - Flash Frequency: "40MHz"
   - CPU Frequency: "80MHz"
   - Upload Speed: "115200"

3. Install libraries:
   - IonConnect
   - ESPAsyncTCP
   - ESPAsyncWebServer
   - ArduinoJson

## Usage

1. **Upload sketch** to ESP-01
2. **Power cycle** the module
3. **Connect** to WiFi: "IonConnect-XXXX"
4. **Open browser** to: http://192.168.4.1
5. **Enter credentials** and click "Save & Connect"
6. **Monitor** serial output for connection status

## Serial Output Example

```
==============================================
  IonConnect - ESP-01 Minimal Mode Example
==============================================
----------------------------------------
Memory [Startup]:
  Free:  54123 bytes (66.1%)
  Used:  27797 bytes (33.9%)
----------------------------------------

[1/3] Initializing IonConnect...
✓ Initialized successfully
----------------------------------------
Memory [After Init]:
  Free:  42156 bytes (51.5%)
  Used:  39764 bytes (48.5%)
----------------------------------------

[2/3] Setting up callbacks...

[3/3] Starting IonConnect...
✓ IonConnect started successfully
----------------------------------------
Memory [After Begin]:
  Free:  28432 bytes (34.7%)
  Used:  53488 bytes (65.3%)
----------------------------------------

Setup complete! Entering main loop...

========================================
→ Captive Portal Started
========================================
1. Connect to WiFi: IonConnect-A1B2
2. Open browser to: http://192.168.4.1
3. Enter your WiFi credentials
4. Click 'Save & Connect'
----------------------------------------
Memory [Portal Active]:
  Free:  26234 bytes (32.0%)
  Used:  55686 bytes (68.0%)
----------------------------------------
========================================
```

## Troubleshooting

### Out of Memory

If experiencing crashes:

1. **Check free heap** - Should be > 5KB during operation
2. **Reduce buffers** in `platformio.ini`:
   ```ini
   build_flags = 
       -DION_JSON_SCHEMA_SIZE=768
       -DION_JSON_CONFIG_SIZE=384
   ```
3. **Disable Serial debug**:
   ```ini
   build_flags = -DNDEBUG
   ```

### Cannot Access Portal

1. Ensure connected to "IonConnect-XXXX" WiFi
2. Try http://192.168.4.1 explicitly
3. Disable VPN on phone/laptop
4. Check if browser has cached old page (clear cache)

### Upload Fails

1. Hold GPIO0 LOW during upload (connect to GND)
2. Power cycle ESP-01 before upload
3. Try lower upload speed: `upload_speed = 9600`
4. Check TX/RX connections (must be crossed)

### Watchdog Resets

If seeing WDT resets:
```cpp
void loop() {
    ion.handle();
    delay(10);  // Allow background tasks
    yield();    // Explicit yield for stability
}
```

## Customization

### Change Device Name

```cpp
const char* DEVICE_NAME = "MyESP01";
ion.init(DEVICE_NAME, config);
```

### Adjust Timeouts

```cpp
IonConfig config = IonConfig::minimal();
config.portalTimeoutSeconds = 120;      // 2 minutes
config.connectionTimeoutMs = 20000;     // 20 seconds
config.maxReconnectAttempts = 5;
```

### Custom Schema

Keep it minimal (max 8 fields in minimal mode):

```cpp
const char* customSchema = R"({
  "fields": [
    {"id": "wifi_ssid", "label": "WiFi SSID", "type": "text", "required": true},
    {"id": "wifi_pass", "label": "Password", "type": "password", "required": true},
    {"id": "device_name", "label": "Device Name", "type": "text", "default": "ESP01"}
  ]
})";

ion.loadConfigSchema(customSchema);
```

## Performance

- **Startup time**: ~2-3 seconds
- **Portal response**: ~200-300ms per page
- **Reconnect time**: ~5-10 seconds
- **Memory peak**: ~20KB during portal operation

## Known Limitations

1. **No OTA** - Must upload via serial
2. **3 networks max** - Can store only 3 WiFi credentials
3. **Simple UI** - No live updates or fancy animations
4. **IP access only** - No mDNS (must use IP address)
5. **No diagnostics** - Cannot view system stats in UI

## Further Reading

- [ESP01_LOW_MEMORY.md](../../docs/ESP01_LOW_MEMORY.md) - Complete optimization guide
- [IonConnect Documentation](../../README.md) - Main library docs

## Support

For issues specific to ESP-01 or low-memory devices:
- Tag issues with: `esp01` or `low-memory`
- See: [Troubleshooting Guide](../../docs/TROUBLESHOOTING.md)

