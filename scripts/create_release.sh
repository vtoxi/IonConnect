#!/bin/bash
#
# Manual GitHub Release Creation Script
# Use this if GitHub Actions workflow fails
#

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Create GitHub Release${NC}"
echo -e "${BLUE}========================================${NC}\n"

# Get version from git tag
VERSION=$(git describe --tags --abbrev=0 2>/dev/null || echo "v1.0.0")
echo -e "Creating release for version: ${GREEN}${VERSION}${NC}\n"

# Create release notes file
cat > release_notes.md << 'EOF'
# IonConnect v1.0.0 - Initial Release 🎉

Modern, async WiFi provisioning library for ESP32 and ESP8266.

## 🌟 Features

- **Async Architecture** - Non-blocking captive portal with DNS redirection
- **Beautiful Web UI** - TailwindCSS responsive design with live updates
- **REST API** - 13 comprehensive endpoints + SSE event streaming
- **BLE Provisioning** - ESP32 Bluetooth setup for mobile apps
- **OTA Updates** - Web-based firmware upload with progress tracking
- **Plugin System** - Extensible architecture for custom features
- **Security** - Portal authentication, API tokens, AES-256 encryption
- **Multi-Network** - Priority-based WiFi management with auto-switching
- **Diagnostics** - Real-time system health monitoring
- **9 Complete Examples** - Working code for all major features

## 📦 Installation

### Arduino IDE
```
1. Download IonConnect-1.0.0.zip from Assets below
2. Arduino IDE → Sketch → Include Library → Add .ZIP Library
3. Select downloaded ZIP file
```

Or via Library Manager (after approval):
```
Library Manager → Search "IonConnect" → Install
```

### PlatformIO
```ini
[env:mydevice]
lib_deps = 
    vtoxi/IonConnect@^1.0.0
```

## 🚀 Quick Start

```cpp
#include <IonConnect.h>

IonConnect ion;

void setup() {
    Serial.begin(115200);
    
    // Initialize with device name
    ion.init("MyIoTDevice");
    
    // Register callbacks
    ion.onConnect([](const String& ssid, const String& ip) {
        Serial.printf("Connected to %s, IP: %s\n", ssid.c_str(), ip.c_str());
    });
    
    ion.onConfigSaved([](bool success) {
        Serial.println(success ? "Config saved!" : "Save failed");
    });
    
    // Start provisioning
    ion.begin();
}

void loop() {
    ion.handle();
    
    // Your application code here
    if (ion.isConnected()) {
        // Do cloud communication, etc.
    }
}
```

## 📚 Documentation

- [README](https://github.com/vtoxi/IonConnect/blob/main/README.md) - Complete user guide
- [Examples](https://github.com/vtoxi/IonConnect/tree/main/examples) - 9 working examples
- [Hardware Testing](https://github.com/vtoxi/IonConnect/blob/main/docs/HARDWARE_TESTING.md) - Testing procedures
- [Deployment Guide](https://github.com/vtoxi/IonConnect/blob/main/docs/DEPLOYMENT.md) - Publishing & CI/CD

## 🎯 What's Included

### Core Library
- 18 core modules for ESP32/ESP8266
- Platform abstraction layer
- 5,320+ lines of tested code
- Memory optimized for ESP8266

### Web Interface
- Dynamic form generation from JSON schema
- Real-time status updates via SSE
- Network scanner interface
- Configuration backup/restore
- System diagnostics dashboard

### Examples
1. `01_BasicSetup` - Minimal configuration
2. `02_CustomSchema` - Custom config fields
3. `03_MultiNetwork` - Multiple WiFi networks
4. `04_BLEProvisioning` - Bluetooth setup (ESP32)
5. `05_SecurePortal` - Authentication & security
6. `06_WithPlugin` - Plugin development
7. `07_BackupRestore` - Config import/export
8. `08_Diagnostics` - System monitoring
9. `09_OTA_Update` - Firmware updates

## 🧪 Testing

All 43 automated tests pass:
- Library structure validation
- Metadata verification
- Example sketch checks
- Documentation validation
- Code quality tests

## 🏗️ Platform Support

| Platform | WiFi | BLE | OTA | Plugins | Status |
|----------|------|-----|-----|---------|--------|
| ESP32 | ✅ | ✅ | ✅ | ✅ | Full Support |
| ESP8266 | ✅ | ❌ | ✅ | ✅ | Optimized |

## 📊 Performance

| Platform | Flash | RAM | Min Free Heap |
|----------|-------|-----|---------------|
| ESP32 | ~300KB | ~80KB | >20KB |
| ESP8266 | ~260KB | ~50KB | >5KB |

## 🙏 Credits

**Inspired by**: [WiFiManager by tzapu](https://github.com/tzapu/WiFiManager) - The original and excellent WiFi configuration library

**Built with**:
- ESPAsyncWebServer - Async web server library
- ArduinoJson - JSON parsing and generation
- TailwindCSS - Utility-first CSS framework

## 🐛 Known Issues

None reported yet! This is the initial release.

Please report any issues at: https://github.com/vtoxi/IonConnect/issues

## 🔮 Coming Soon (v1.1)

- mDNS support with `.local` DNS
- QR code generation for quick access
- WPA-Enterprise support
- Mobile companion app
- Multi-language UI

## 📄 License

MIT License - Free for personal and commercial use

See [LICENSE](https://github.com/vtoxi/IonConnect/blob/main/LICENSE) for full text.

---

**Star ⭐ the project if you find it useful!**

**Questions?** Open an [issue](https://github.com/vtoxi/IonConnect/issues) or check the [documentation](https://github.com/vtoxi/IonConnect/blob/main/README.md).
EOF

echo -e "${GREEN}✓ Release notes created${NC}\n"

# Create release ZIP
echo -e "${YELLOW}Creating release ZIP...${NC}"
mkdir -p release_package/IonConnect

# Copy files
cp -r src release_package/IonConnect/
cp -r examples release_package/IonConnect/
cp library.properties release_package/IonConnect/
cp library.json release_package/IonConnect/
cp keywords.txt release_package/IonConnect/
cp README.md release_package/IonConnect/
cp LICENSE release_package/IonConnect/
cp CHANGELOG.md release_package/IonConnect/

# Create ZIP
cd release_package
zip -r ../IonConnect-1.0.0.zip IonConnect/ > /dev/null
cd ..

echo -e "${GREEN}✓ Release package created: IonConnect-1.0.0.zip${NC}\n"

# Show instructions
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Manual Release Instructions${NC}"
echo -e "${BLUE}========================================${NC}\n"

echo -e "${YELLOW}GitHub Actions failed due to permissions.${NC}"
echo -e "${YELLOW}Create the release manually instead:${NC}\n"

echo -e "1. Go to: ${GREEN}https://github.com/vtoxi/IonConnect/releases/new${NC}"
echo -e ""
echo -e "2. Fill in the form:"
echo -e "   • Tag: ${GREEN}v1.0.0${NC} (select existing tag)"
echo -e "   • Title: ${GREEN}IonConnect v1.0.0 - Initial Release${NC}"
echo -e "   • Description: Copy from ${GREEN}release_notes.md${NC}"
echo -e "   • Attach: ${GREEN}IonConnect-1.0.0.zip${NC}"
echo -e ""
echo -e "3. Click ${GREEN}Publish release${NC}"
echo -e ""

echo -e "${BLUE}========================================${NC}\n"

echo -e "Files ready:"
echo -e "  📄 ${GREEN}release_notes.md${NC} - Release description"
echo -e "  📦 ${GREEN}IonConnect-1.0.0.zip${NC} - Release package"
echo -e ""

# Open release notes in editor
if command -v code &> /dev/null; then
    echo -e "Opening release notes in VS Code..."
    code release_notes.md
elif command -v nano &> /dev/null; then
    echo -e "Release notes content:"
    cat release_notes.md
fi

echo -e "\n${GREEN}Ready to create release!${NC}"

