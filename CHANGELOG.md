# Changelog

All notable changes to IonConnect will be documented in this file.

## [1.0.3] - 2025-10-31

### Added
- **Minimal Mode** for ESP-01 and low-RAM devices (ESP8266 with <100KB RAM)
  - Reduced JSON buffers: 75% smaller (4096→1024, 2048→512)
  - Minimal HTML: 63% smaller (15.6KB→5.7KB)
  - Memory savings: ~18-20KB total
  - New build flag: `ION_MINIMAL_MODE=1`
  - Factory method: `IonConfig::minimal()`
  - ESP-01 example with complete setup guide

- **Documentation**
  - ESP-01 low-memory optimization guide
  - PlatformIO configuration examples
  - Memory usage analysis and troubleshooting

- **Build Flags**
  - `ION_MINIMAL_MODE` - Enable minimal mode
  - `ION_JSON_SCHEMA_SIZE` - Configure buffer sizes
  - `ION_JSON_CONFIG_SIZE` - Configure buffer sizes
  - `ION_JSON_BUFFER_SIZE` - Configure buffer sizes

### Changed
- All JSON allocations now use configurable constants
- Conditional feature disabling in minimal mode (OTA, mDNS, diagnostics, BLE)

### Fixed
- Namespace crash during initialization (PROGMEM access issue)
- HTML embedding structure for minimal mode
- Memory allocation on low-RAM devices

### Performance
- Startup time: 25% faster in minimal mode
- Peak RAM usage: 50% reduction (40KB → 20KB)
- Portal response: 200-300ms (minimal mode)

## [1.0.1] - 2025-10-30

### Added
- Initial release
- Async WiFi provisioning with captive portal
- BLE provisioning (ESP32)
- REST API and dynamic configuration
- Plugin system
- OTA updates
- Diagnostics panel
- mDNS support

[1.0.3]: https://github.com/vtoxi/IonConnect/compare/v1.0.2...v1.0.3
[1.0.2]: https://github.com/vtoxi/IonConnect/compare/v1.0.1...v1.0.2
[1.0.1]: https://github.com/vtoxi/IonConnect/releases/tag/v1.0.1

