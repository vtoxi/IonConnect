#!/bin/bash
#
# IonConnect Publishing Script
# Prepares and publishes library to Arduino and PlatformIO
#

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  IonConnect Publishing Script${NC}"
echo -e "${BLUE}========================================${NC}\n"

# Get current version
VERSION=$(grep "version=" library.properties | cut -d'=' -f2)
echo -e "Current version: ${GREEN}${VERSION}${NC}\n"

# Ask for confirmation
read -p "Publish version ${VERSION}? (y/N): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Aborted."
    exit 1
fi

echo -e "\n${YELLOW}Step 1: Running tests...${NC}"
./scripts/test.sh || {
    echo -e "${RED}Tests failed! Fix errors before publishing.${NC}"
    exit 1
}

echo -e "\n${YELLOW}Step 2: Building all examples...${NC}"
./scripts/build.sh || {
    echo -e "${RED}Build failed! Fix errors before publishing.${NC}"
    exit 1
}

echo -e "\n${YELLOW}Step 3: Creating release package...${NC}"
mkdir -p release/IonConnect

# Copy library files
cp -r src release/IonConnect/
cp -r examples release/IonConnect/
cp library.properties release/IonConnect/
cp library.json release/IonConnect/
cp keywords.txt release/IonConnect/
cp README.md release/IonConnect/
cp LICENSE release/IonConnect/
cp CHANGELOG.md release/IonConnect/

# Create ZIP for Arduino
cd release
zip -r "../IonConnect-${VERSION}.zip" IonConnect/ > /dev/null
cd ..

echo -e "${GREEN}✓ Release package created: IonConnect-${VERSION}.zip${NC}"

echo -e "\n${YELLOW}Step 4: Git operations...${NC}"

# Check if there are uncommitted changes
if [[ -n $(git status -s) ]]; then
    echo -e "${RED}Warning: Uncommitted changes detected!${NC}"
    git status -s
    read -p "Commit changes? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        git add -A
        git commit -m "Release v${VERSION}"
    fi
fi

# Create git tag
if git rev-parse "v${VERSION}" >/dev/null 2>&1; then
    echo -e "${YELLOW}Tag v${VERSION} already exists${NC}"
else
    git tag -a "v${VERSION}" -m "Release version ${VERSION}"
    echo -e "${GREEN}✓ Created git tag v${VERSION}${NC}"
fi

# Push to GitHub
read -p "Push to GitHub? (y/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    git push origin main
    git push origin "v${VERSION}"
    echo -e "${GREEN}✓ Pushed to GitHub${NC}"
fi

echo -e "\n${YELLOW}Step 5: PlatformIO publishing...${NC}"

# Check if PlatformIO CLI is available
if command -v pio &> /dev/null; then
    read -p "Publish to PlatformIO? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        if [ -z "$PLATFORMIO_AUTH_TOKEN" ]; then
            echo -e "${YELLOW}Note: Set PLATFORMIO_AUTH_TOKEN environment variable${NC}"
            echo "Get token from: https://platformio.org/account/token"
            read -p "Enter token (or press Enter to skip): " TOKEN
            if [ -n "$TOKEN" ]; then
                export PLATFORMIO_AUTH_TOKEN=$TOKEN
            fi
        fi
        
        if [ -n "$PLATFORMIO_AUTH_TOKEN" ]; then
            pio pkg publish --type=library
            echo -e "${GREEN}✓ Published to PlatformIO${NC}"
        else
            echo -e "${YELLOW}Skipped PlatformIO publishing (no token)${NC}"
        fi
    fi
else
    echo -e "${YELLOW}PlatformIO CLI not found, skipping${NC}"
fi

echo -e "\n${YELLOW}Step 6: Arduino Library Manager${NC}\n"

echo "To publish to Arduino Library Manager:"
echo ""
echo "Option 1: Submit via GitHub (Recommended)"
echo "  1. Go to: https://github.com/arduino/library-registry"
echo "  2. Fork the repository"
echo "  3. Edit 'repositories.txt'"
echo "  4. Add: https://github.com/$(git config --get remote.origin.url | sed 's/.*github.com[:/]\(.*\)\.git/\1/')"
echo "  5. Create a Pull Request"
echo ""
echo "Option 2: Submit an Issue"
echo "  1. Go to: https://github.com/arduino/library-registry/issues/new"
echo "  2. Title: 'Add IonConnect library'"
echo "  3. Body: Repository URL: https://github.com/$(git config --get remote.origin.url | sed 's/.*github.com[:/]\(.*\)\.git/\1/')"
echo ""

read -p "Open Arduino Library Registry in browser? (y/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    if command -v xdg-open &> /dev/null; then
        xdg-open "https://github.com/arduino/library-registry"
    elif command -v open &> /dev/null; then
        open "https://github.com/arduino/library-registry"
    fi
fi

echo -e "\n${BLUE}========================================${NC}"
echo -e "${GREEN}✓ Publishing Complete!${NC}"
echo -e "${BLUE}========================================${NC}\n"

echo "Summary:"
echo "  Version:     ${VERSION}"
echo "  ZIP Package: IonConnect-${VERSION}.zip"
echo "  Git Tag:     v${VERSION}"
echo ""
echo "Next steps:"
echo "  1. Create GitHub release with ZIP file"
echo "  2. Submit to Arduino Library Manager"
echo "  3. Announce on forums/social media"
echo "  4. Update documentation website"
echo ""
echo -e "${GREEN}Happy publishing! 🎉${NC}\n"

