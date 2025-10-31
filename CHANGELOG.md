# Changelog

All notable changes to IonConnect will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-10-31

### 🎉 Initial Release

#### Added
- **Core WiFi Provisioning**
  - Captive portal with DNS redirection
  - WiFi network scanning and selection
  - Multi-network support with priority
  - Smart reconnection with exponential backoff
  - Persistent credential storage (NVS/EEPROM)

- **Web Interface**
  - Responsive TailwindCSS-based UI
  - Dynamic form generation from JSON schema
  - Live status updates via Server-Sent Events (SSE)
  - Network scanner with signal strength
  - Configuration backup/restore
  - System diagnostics panel

- **REST API** (13 endpoints)
  - `/api/schema` - Configuration schema
  - `/api/config` - Get/Post configuration
  - `/api/scan` - WiFi network scan
  - `/api/networks` - Scan results
  - `/api/status` - Connection status
  - `/api/clear` - Clear credentials
  - `/api/export` - Backup configuration
  - `/api/import` - Restore configuration
  - `/api/reboot` - Device restart
  - `/api/info` - Device information
  - `/api/diagnostics` - Health metrics
  - `/api/events` - SSE event stream
  - `/update` - OTA firmware upload

- **BLE Provisioning** (ESP32)
  - GATT service for mobile provisioning
  - Schema distribution via BLE
  - Configuration via BLE
  - Status notifications
  - Control commands (scan, connect, reboot)

- **Security Features**
  - Portal password protection
  - API token authentication
  - Credential encryption (AES-256)
  - Session management
  - Portal timeout

- **OTA Updates**
  - Web-based firmware upload
  - Progress tracking
  - Authentication
  - Update validation
  - Error handling

- **Plugin System**
  - Extensible architecture
  - Custom API endpoints
  - UI panel injection
  - Event notifications
  - Plugin lifecycle management

- **System Diagnostics**
  - Memory monitoring
  - WiFi statistics
  - API metrics
  - Error logging
  - Health endpoints

- **Platform Support**
  - ESP32 (full features including BLE)
  - ESP8266 (WiFi provisioning)
  - Platform abstraction layer
  - Shared interfaces

- **Developer Tools**
  - Web asset builder (HTML/JS minification)
  - 9 comprehensive examples
  - Hardware testing guide
  - Complete documentation

#### Examples
1. `BasicSetup` - Minimal configuration
2. `CustomSchema` - Custom config fields
3. `MultiNetwork` - Multiple WiFi networks
4. `BLEProvisioning` - Bluetooth setup (ESP32)
5. `SecurePortal` - Password & token auth
6. `WithPlugin` - Plugin development
7. `BackupRestore` - Config management
8. `Diagnostics` - System monitoring
9. `OTA_Update` - Firmware updates

#### Documentation
- Complete README with quick start
- Hardware testing procedures
- API reference
- Build tool documentation
- Implementation status tracking

### Technical Details
- **Architecture**: Fully async, non-blocking
- **Memory**: Optimized for ESP8266 constraints
- **Code Quality**: SOLID principles, clean code
- **Dependencies**: ESPAsyncWebServer, ArduinoJson
- **License**: MIT

### Inspiration
This library is inspired by and improves upon WiFiManager by tzapu, bringing modern async patterns, extensive features, and beautiful UI to ESP32/ESP8266 WiFi provisioning.

---

## [Unreleased]

### Planned for Future Releases
- mDNS support with `.local` DNS
- QR code generation for quick access
- WPA-Enterprise support
- Ethernet support (ESP32)
- Mobile companion app
- Multi-language UI (i18n)
- Cloud integration
- Fleet management features

---

[1.0.0]: https://github.com/vtoxi/IonConnect/releases/tag/v1.0.0

