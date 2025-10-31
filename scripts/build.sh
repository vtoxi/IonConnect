#!/bin/bash
#
# IonConnect Build Script
# Builds all examples for both ESP32 and ESP8266
#

set -e  # Exit on error

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
EXAMPLES=(
    "01_BasicSetup"
    "02_CustomSchema"
    "03_MultiNetwork"
    "04_BLEProvisioning"  # ESP32 only
    "05_SecurePortal"
    "06_WithPlugin"
    "07_BackupRestore"
    "08_Diagnostics"
    "09_OTA_Update"
)

PLATFORMS=("esp32" "esp8266")

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  IonConnect Build Script${NC}"
echo -e "${BLUE}========================================${NC}\n"

# Check if PlatformIO is installed
if ! command -v pio &> /dev/null; then
    echo -e "${RED}✗ PlatformIO not found!${NC}"
    echo "Install with: pip install platformio"
    exit 1
fi

echo -e "${GREEN}✓ PlatformIO found${NC}\n"

# Build web assets
echo -e "${YELLOW}Building web assets...${NC}"
cd tools/web_builder
pip install -r requirements.txt -q
python build.py
cd ../..
echo -e "${GREEN}✓ Web assets built${NC}\n"

# Track build results
TOTAL_BUILDS=0
SUCCESSFUL_BUILDS=0
FAILED_BUILDS=0

# Build each example
for EXAMPLE in "${EXAMPLES[@]}"; do
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BLUE}Building: ${EXAMPLE}${NC}"
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}\n"
    
    cd "examples/${EXAMPLE}"
    
    # Create platformio.ini if it doesn't exist
    if [ ! -f "platformio.ini" ]; then
        cat > platformio.ini << 'EOF'
[platformio]
default_envs = esp32

[env:esp32]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps =
    https://github.com/me-no-dev/ESPAsyncWebServer.git
    bblanchon/ArduinoJson@^6.19.4
build_flags = -DCORE_DEBUG_LEVEL=0
monitor_speed = 115200

[env:esp8266]
platform = espressif8266
board = nodemcuv2
framework = arduino
lib_deps =
    https://github.com/me-no-dev/ESPAsyncWebServer.git
    bblanchon/ArduinoJson@^6.19.4
build_flags = -DPIO_FRAMEWORK_ARDUINO_LWIP2_LOW_MEMORY
monitor_speed = 115200
EOF
    fi
    
    # Build for each platform
    for PLATFORM in "${PLATFORMS[@]}"; do
        # Skip BLE example on ESP8266
        if [ "$EXAMPLE" = "04_BLEProvisioning" ] && [ "$PLATFORM" = "esp8266" ]; then
            echo -e "${YELLOW}⊘ Skipping BLE example on ESP8266${NC}"
            continue
        fi
        
        TOTAL_BUILDS=$((TOTAL_BUILDS + 1))
        
        echo -e "${YELLOW}Building for ${PLATFORM}...${NC}"
        
        if pio run -e "$PLATFORM" 2>&1 | tee build.log; then
            echo -e "${GREEN}✓ ${EXAMPLE} - ${PLATFORM} build successful${NC}"
            SUCCESSFUL_BUILDS=$((SUCCESSFUL_BUILDS + 1))
            
            # Show memory usage
            echo -e "${BLUE}Memory Usage:${NC}"
            pio run -e "$PLATFORM" -t size | grep -E "RAM:|Flash:" || true
            echo ""
        else
            echo -e "${RED}✗ ${EXAMPLE} - ${PLATFORM} build failed${NC}"
            FAILED_BUILDS=$((FAILED_BUILDS + 1))
            
            # Show last 10 lines of build output
            echo -e "${RED}Last 10 lines of build output:${NC}"
            tail -n 10 build.log
            echo ""
        fi
    done
    
    cd ../..
    echo ""
done

# Summary
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Build Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Total Builds:      ${TOTAL_BUILDS}"
echo -e "${GREEN}Successful:        ${SUCCESSFUL_BUILDS}${NC}"
if [ $FAILED_BUILDS -gt 0 ]; then
    echo -e "${RED}Failed:            ${FAILED_BUILDS}${NC}"
else
    echo -e "Failed:            ${FAILED_BUILDS}"
fi
echo -e "${BLUE}========================================${NC}\n"

if [ $FAILED_BUILDS -eq 0 ]; then
    echo -e "${GREEN}✓ All builds completed successfully!${NC}"
    exit 0
else
    echo -e "${RED}✗ Some builds failed. Check the output above.${NC}"
    exit 1
fi

