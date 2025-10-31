#!/bin/bash

# Arduino Library Registry Submission Script
# Automates PR creation to add library to Arduino Library Manager

set -e

GITHUB_TOKEN="${GITHUB_TOKEN:-}"
REPO_URL="https://github.com/vtoxi/IonConnect"
LIBRARY_NAME="IonConnect"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}╔═══════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║                                                           ║${NC}"
echo -e "${BLUE}║    Arduino Library Manager Submission Automation         ║${NC}"
echo -e "${BLUE}║                                                           ║${NC}"
echo -e "${BLUE}╚═══════════════════════════════════════════════════════════╝${NC}"
echo ""

if [ -z "$GITHUB_TOKEN" ]; then
    echo -e "${RED}✗ Error: GITHUB_TOKEN not set${NC}"
    echo ""
    echo "Usage:"
    echo "  export GITHUB_TOKEN='your_github_pat'"
    echo "  ./scripts/submit_arduino.sh"
    echo ""
    echo "Or use manual submission:"
    echo "  https://github.com/arduino/library-registry"
    exit 1
fi

echo -e "${YELLOW}→ Step 1: Forking arduino/library-registry...${NC}"
FORK_RESPONSE=$(curl -s -X POST \
  -H "Authorization: token $GITHUB_TOKEN" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/arduino/library-registry/forks)

FORK_OWNER=$(echo "$FORK_RESPONSE" | grep -o '"login"[[:space:]]*:[[:space:]]*"[^"]*"' | head -1 | cut -d'"' -f4)

if [ -z "$FORK_OWNER" ]; then
    echo -e "${YELLOW}  Fork may already exist, checking...${NC}"
    USER_RESPONSE=$(curl -s \
      -H "Authorization: token $GITHUB_TOKEN" \
      -H "Accept: application/vnd.github.v3+json" \
      https://api.github.com/user)
    FORK_OWNER=$(echo "$USER_RESPONSE" | grep -o '"login"[[:space:]]*:[[:space:]]*"[^"]*"' | head -1 | cut -d'"' -f4)
fi

echo -e "${GREEN}✓ Fork: $FORK_OWNER/library-registry${NC}"
echo ""

# Wait for fork to be ready
echo -e "${YELLOW}→ Step 2: Waiting for fork to be ready...${NC}"
sleep 3
echo -e "${GREEN}✓ Fork ready${NC}"
echo ""

echo -e "${YELLOW}→ Step 3: Getting repositories.txt content...${NC}"
CONTENT_RESPONSE=$(curl -s \
  -H "Authorization: token $GITHUB_TOKEN" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/$FORK_OWNER/library-registry/contents/repositories.txt)

FILE_SHA=$(echo "$CONTENT_RESPONSE" | grep -o '"sha"[[:space:]]*:[[:space:]]*"[^"]*"' | head -1 | cut -d'"' -f4)
CONTENT_BASE64=$(echo "$CONTENT_RESPONSE" | grep -o '"content"[[:space:]]*:[[:space:]]*"[^"]*"' | head -1 | cut -d'"' -f4)

if [ -z "$FILE_SHA" ]; then
    echo -e "${RED}✗ Could not get file SHA${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Got current file content${NC}"
echo ""

echo -e "${YELLOW}→ Step 4: Adding $REPO_URL to repositories.txt...${NC}"

# Decode content, add new line, encode back
CURRENT_CONTENT=$(curl -s \
  -H "Authorization: token $GITHUB_TOKEN" \
  https://raw.githubusercontent.com/$FORK_OWNER/library-registry/main/repositories.txt)
NEW_CONTENT="${CURRENT_CONTENT}
${REPO_URL}"
NEW_CONTENT_BASE64=$(echo -n "$NEW_CONTENT" | base64)

# Create new branch and update file
BRANCH_NAME="add-ionconnect-$(date +%s)"

echo -e "${YELLOW}→ Step 5: Creating branch: $BRANCH_NAME...${NC}"

# Get main branch SHA
MAIN_REF=$(curl -s \
  -H "Authorization: token $GITHUB_TOKEN" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/$FORK_OWNER/library-registry/git/refs/heads/main)

MAIN_SHA=$(echo "$MAIN_REF" | grep -o '"sha"[[:space:]]*:[[:space:]]*"[^"]*"' | head -1 | cut -d'"' -f4)

# Create new branch
curl -s -X POST \
  -H "Authorization: token $GITHUB_TOKEN" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/$FORK_OWNER/library-registry/git/refs \
  -d "{\"ref\":\"refs/heads/$BRANCH_NAME\",\"sha\":\"$MAIN_SHA\"}" > /dev/null

echo -e "${GREEN}✓ Branch created${NC}"
echo ""

echo -e "${YELLOW}→ Step 6: Updating repositories.txt...${NC}"

UPDATE_RESPONSE=$(curl -s -X PUT \
  -H "Authorization: token $GITHUB_TOKEN" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/$FORK_OWNER/library-registry/contents/repositories.txt \
  -d "{
    \"message\": \"Add IonConnect library\",
    \"content\": \"$NEW_CONTENT_BASE64\",
    \"sha\": \"$FILE_SHA\",
    \"branch\": \"$BRANCH_NAME\"
  }")

echo -e "${GREEN}✓ File updated in branch${NC}"
echo ""

echo -e "${YELLOW}→ Step 7: Creating pull request...${NC}"

PR_BODY="Adding IonConnect - Modern WiFi provisioning library for ESP32/ESP8266

**Repository**: $REPO_URL
**Category**: Communication
**Architectures**: esp32, esp8266
**License**: MIT
**Version**: 1.0.0

## Features
- ✨ Async captive portal with modern web UI
- ✨ REST API + Server-Sent Events
- ✨ BLE provisioning (ESP32)
- ✨ OTA firmware updates
- ✨ Multiple network support
- ✨ Plugin system
- ✨ Built-in diagnostics

This library provides a modern replacement for legacy WiFiManager with async operations, beautiful UI, and extensive features for production IoT devices.

## Compliance
- [x] Library follows Arduino Library Specification
- [x] Contains library.properties with valid metadata
- [x] Includes working examples
- [x] GPL-compatible license (MIT)
- [x] Release tagged (v1.0.0)
- [x] Examples compile for target architectures"

PR_RESPONSE=$(curl -s -X POST \
  -H "Authorization: token $GITHUB_TOKEN" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/arduino/library-registry/pulls \
  -d "{
    \"title\": \"Add $LIBRARY_NAME library\",
    \"body\": $(echo "$PR_BODY" | jq -Rs .),
    \"head\": \"$FORK_OWNER:$BRANCH_NAME\",
    \"base\": \"main\"
  }")

PR_URL=$(echo "$PR_RESPONSE" | grep -o '"html_url"[[:space:]]*:[[:space:]]*"[^"]*"' | head -1 | cut -d'"' -f4)

if [ -n "$PR_URL" ]; then
    echo -e "${GREEN}✓ Pull request created!${NC}"
    echo ""
    echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}"
    echo -e "${GREEN}✅ SUCCESS!${NC}"
    echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}"
    echo ""
    echo -e "Pull Request: ${BLUE}$PR_URL${NC}"
    echo ""
    echo "What happens next:"
    echo "  1. Arduino team will review your PR (1-3 days)"
    echo "  2. They may request changes or approve directly"
    echo "  3. Once merged, your library appears in Arduino Library Manager"
    echo "  4. Users can install via: Sketch → Include Library → Manage Libraries"
    echo ""
else
    ERROR_MSG=$(echo "$PR_RESPONSE" | grep -o '"message"[[:space:]]*:[[:space:]]*"[^"]*"' | head -1 | cut -d'"' -f4)
    echo -e "${RED}✗ Failed to create PR${NC}"
    echo "Error: $ERROR_MSG"
    echo ""
    echo "Manual submission:"
    echo "  1. Go to: https://github.com/arduino/library-registry"
    echo "  2. Fork and edit repositories.txt"
    echo "  3. Add: $REPO_URL"
    echo "  4. Create pull request"
    exit 1
fi

