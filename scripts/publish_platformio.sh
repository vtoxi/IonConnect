#!/bin/bash
#
# PlatformIO Publishing Script
#

set -e

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Publishing to PlatformIO${NC}"
echo -e "${BLUE}========================================${NC}\n"

# Set PlatformIO token
export PLATFORMIO_AUTH_TOKEN="m6SXqnCVpo1hbpefiqO4oY171GCf2mPeaZfVXI2ck2uUprmgi6SglZ2hld6ek6BeYWxmZrzVj5uFpqaRbatprW+Uo4tkc5Nr"

echo -e "${YELLOW}Validating library structure...${NC}"

# Check required files
if [ ! -f "library.json" ]; then
    echo -e "${RED}Error: library.json not found${NC}"
    exit 1
fi

if [ ! -f "library.properties" ]; then
    echo -e "${RED}Error: library.properties not found${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Library structure valid${NC}\n"

echo -e "${YELLOW}Publishing to PlatformIO Registry...${NC}"

# Publish
pio pkg publish --type=library --owner=vtoxi

echo -e "\n${GREEN}✓ Published successfully!${NC}"

echo -e "\n${BLUE}========================================${NC}"
echo -e "${GREEN}Library published to PlatformIO!${NC}"
echo -e "${BLUE}========================================${NC}\n"

echo -e "View your library at:"
echo -e "${GREEN}https://registry.platformio.org/libraries/vtoxi/IonConnect${NC}\n"

echo -e "Users can now install with:"
echo -e "${YELLOW}pio pkg install --library \"vtoxi/IonConnect\"${NC}\n"

