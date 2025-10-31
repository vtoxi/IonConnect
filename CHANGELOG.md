# Changelog

All notable changes to IonConnect-ESP will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.3] - 2025-10-31

### Added
- **Minimal Mode** for low-memory devices (ESP-01, ESP-01S)
  - Reduced JSON buffer sizes (75% reduction: 4096→1024, 2048→512)
  - Minimal embedded HTML/JS (63% reduction: 15.6KB→5.7KB)
  - Configurable memory constants via build flags
  - Factory method `IonConfig::minimal()` for easy minimal configuration
  - Support for devices with ~80KB RAM
  
- **Documentation**
  - Complete ESP-01 optimization guide (`docs/ESP01_LOW_MEMORY.md`)
  - Minimal mode quick start guide (`MINIMAL_MODE_QUICK_START.md`)
  - Implementation details (`MINIMAL_MODE_SUMMARY.md`)
  - Optimization changelog (`OPTIMIZATION_CHANGELOG.md`)
  - ESP-01 example project (`examples/10_ESP01_Minimal/`)

- **Build Flags**
  - `ION_MINIMAL_MODE` - Enable minimal mode (default: 0)
  - `ION_JSON_SCHEMA_SIZE` - Configure schema buffer size
  - `ION_JSON_CONFIG_SIZE` - Configure config buffer size
  - `ION_JSON_BUFFER_SIZE` - Configure operation buffer size
  - `ION_MAX_NETWORKS` - Limit stored network count
  - `ION_MAX_CONFIG_FIELDS` - Limit config field count

### Changed
- All `DynamicJsonDocument` allocations now use configurable constants
- Conditional defaults in `IonConfig` based on minimal mode
- HTML/JS conditionally loaded based on `ION_MINIMAL_MODE`

### Fixed
- Namespace nesting issue in embedded HTML includes
- PROGMEM access crash during initialization

### Performance
- Startup time reduced by ~25% in minimal mode
- Peak RAM usage reduced by 50% (40KB→20KB)
- Total memory savings: ~18-20KB

## [1.0.2] - 2025-10-31

### Changed
- Library renamed from `IonConnect` to `IonConnect-ESP` for PlatformIO registry
- Updated description to highlight minimal mode and low-memory optimization

## [1.0.1] - 2025-10-30

### Added
- Initial release with full feature set
- Async WiFi provisioning with captive portal
- BLE provisioning (ESP32)
- REST API
- Dynamic configuration schema
- Plugin system
- OTA updates
- Diagnostics panel
- mDNS support

[1.0.3]: https://github.com/vtoxi/IonConnect/compare/v1.0.2...v1.0.3
[1.0.2]: https://github.com/vtoxi/IonConnect/compare/v1.0.1...v1.0.2
[1.0.1]: https://github.com/vtoxi/IonConnect/releases/tag/v1.0.1
