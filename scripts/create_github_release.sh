#!/bin/bash
#
# Create GitHub Release using GitHub API
#

set -e

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Create GitHub Release via API${NC}"
echo -e "${BLUE}========================================${NC}\n"

# Configuration
GITHUB_TOKEN="${GITHUB_TOKEN:-}"
REPO_OWNER="vtoxi"
REPO_NAME="IonConnect"
TAG_NAME="v1.0.0"
RELEASE_NAME="IonConnect v1.0.0 - Initial Release"

echo -e "${YELLOW}Creating release for: ${TAG_NAME}${NC}\n"

# Create release JSON
cat > release_payload.json << 'EOF'
{
  "tag_name": "v1.0.0",
  "target_commitish": "main",
  "name": "IonConnect v1.0.0 - Initial Release",
  "body": "# IonConnect v1.0.0 - Initial Release 🎉\n\nModern, async WiFi provisioning library for ESP32 and ESP8266.\n\n## 🌟 Features\n\n- **Async Architecture** - Non-blocking captive portal with DNS redirection\n- **Beautiful Web UI** - TailwindCSS responsive design with live updates\n- **REST API** - 13 comprehensive endpoints + SSE event streaming\n- **BLE Provisioning** - ESP32 Bluetooth setup for mobile apps\n- **OTA Updates** - Web-based firmware upload with progress tracking\n- **Plugin System** - Extensible architecture for custom features\n- **Security** - Portal authentication, API tokens, AES-256 encryption\n- **Multi-Network** - Priority-based WiFi management with auto-switching\n- **Diagnostics** - Real-time system health monitoring\n- **9 Complete Examples** - Working code for all major features\n\n## 📦 Installation\n\n### PlatformIO\n```ini\n[env:mydevice]\nlib_deps = \n    vtoxi/IonConnect@^1.0.0\n```\n\n### Arduino IDE\nDownload `IonConnect-1.0.0.zip` from Assets below and install via Library Manager.\n\n## 🚀 Quick Start\n\n```cpp\n#include <IonConnect.h>\n\nIonConnect ion;\n\nvoid setup() {\n    ion.init(\"MyIoTDevice\");\n    ion.begin();\n}\n\nvoid loop() {\n    ion.handle();\n}\n```\n\n## 📚 Documentation\n\n- [README](https://github.com/vtoxi/IonConnect/blob/main/README.md)\n- [Examples](https://github.com/vtoxi/IonConnect/tree/main/examples)\n- [Hardware Testing](https://github.com/vtoxi/IonConnect/blob/main/docs/HARDWARE_TESTING.md)\n- [Deployment Guide](https://github.com/vtoxi/IonConnect/blob/main/docs/DEPLOYMENT.md)\n\n## 🙏 Credits\n\nInspired by [WiFiManager by tzapu](https://github.com/tzapu/WiFiManager)\n\n## 📄 License\n\nMIT License - Free for personal and commercial use",
  "draft": false,
  "prerelease": false
}
EOF

echo -e "${YELLOW}Sending request to GitHub API...${NC}"

# Create the release
RESPONSE=$(curl -s -w "\n%{http_code}" \
  -X POST \
  -H "Accept: application/vnd.github+json" \
  -H "Authorization: Bearer ${GITHUB_TOKEN}" \
  -H "X-GitHub-Api-Version: 2022-11-28" \
  https://api.github.com/repos/${REPO_OWNER}/${REPO_NAME}/releases \
  -d @release_payload.json)

HTTP_CODE=$(echo "$RESPONSE" | tail -n1)
BODY=$(echo "$RESPONSE" | sed '$d')

if [ "$HTTP_CODE" = "201" ]; then
    echo -e "${GREEN}✓ Release created successfully!${NC}\n"
    
    # Extract upload URL and release ID
    UPLOAD_URL=$(echo "$BODY" | grep -o '"upload_url": "[^"]*' | cut -d'"' -f4 | sed 's/{?name,label}//')
    RELEASE_ID=$(echo "$BODY" | grep -o '"id": [0-9]*' | head -1 | cut -d' ' -f2)
    
    echo -e "${BLUE}Release Details:${NC}"
    echo -e "  ID: ${RELEASE_ID}"
    echo -e "  URL: https://github.com/${REPO_OWNER}/${REPO_NAME}/releases/tag/${TAG_NAME}"
    
    # Upload asset if ZIP exists
    if [ -f "IonConnect-1.0.0.zip" ]; then
        echo -e "\n${YELLOW}Uploading release asset...${NC}"
        
        ASSET_RESPONSE=$(curl -s -w "\n%{http_code}" \
          -X POST \
          -H "Accept: application/vnd.github+json" \
          -H "Authorization: Bearer ${GITHUB_TOKEN}" \
          -H "X-GitHub-Api-Version: 2022-11-28" \
          -H "Content-Type: application/zip" \
          --data-binary @IonConnect-1.0.0.zip \
          "${UPLOAD_URL}?name=IonConnect-1.0.0.zip")
        
        ASSET_HTTP_CODE=$(echo "$ASSET_RESPONSE" | tail -n1)
        
        if [ "$ASSET_HTTP_CODE" = "201" ]; then
            echo -e "${GREEN}✓ Asset uploaded successfully!${NC}"
        else
            echo -e "${RED}✗ Asset upload failed (HTTP ${ASSET_HTTP_CODE})${NC}"
        fi
    fi
    
    echo -e "\n${BLUE}========================================${NC}"
    echo -e "${GREEN}✓ GitHub Release Complete!${NC}"
    echo -e "${BLUE}========================================${NC}\n"
    
    echo -e "View release at:"
    echo -e "${GREEN}https://github.com/${REPO_OWNER}/${REPO_NAME}/releases/tag/${TAG_NAME}${NC}\n"
    
elif [ "$HTTP_CODE" = "422" ]; then
    echo -e "${YELLOW}⚠ Release already exists${NC}"
    echo -e "View existing release at:"
    echo -e "${GREEN}https://github.com/${REPO_OWNER}/${REPO_NAME}/releases/tag/${TAG_NAME}${NC}\n"
    
else
    echo -e "${RED}✗ Release creation failed (HTTP ${HTTP_CODE})${NC}"
    echo -e "Response: ${BODY}\n"
    exit 1
fi

# Cleanup
rm -f release_payload.json

