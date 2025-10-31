#!/bin/bash
#
# IonConnect Test Script
# Runs various tests to validate library functionality
#

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  IonConnect Test Suite${NC}"
echo -e "${BLUE}========================================${NC}\n"

TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

run_test() {
    local test_name=$1
    local test_command=$2
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -e "${YELLOW}Running: ${test_name}${NC}"
    
    if eval "$test_command" > /dev/null 2>&1; then
        echo -e "${GREEN}✓ PASS${NC}\n"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        return 0
    else
        echo -e "${RED}✗ FAIL${NC}\n"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi
}

echo -e "${BLUE}1. Library Structure Tests${NC}\n"

run_test "library.properties exists" "test -f library.properties"
run_test "library.json exists" "test -f library.json"
run_test "keywords.txt exists" "test -f keywords.txt"
run_test "LICENSE exists" "test -f LICENSE"
run_test "README.md exists" "test -f README.md"
run_test "CHANGELOG.md exists" "test -f CHANGELOG.md"
run_test "src/ directory exists" "test -d src"
run_test "examples/ directory exists" "test -d examples"

echo -e "${BLUE}2. Required Files Tests${NC}\n"

run_test "IonConnect.h exists" "test -f src/IonConnect.h"
run_test "IonTypes.h exists" "test -f src/core/IonTypes.h"
run_test "IonConfig.h exists" "test -f src/core/IonConfig.h"
run_test "ConfigManager exists" "test -f src/modules/ConfigManager.h"
run_test "WiFiConnectionCore exists" "test -f src/modules/WiFiConnectionCore.h"
run_test "SecurityManager exists" "test -f src/modules/SecurityManager.h"

echo -e "${BLUE}3. library.properties Validation${NC}\n"

run_test "Has name field" "grep -q 'name=IonConnect' library.properties"
run_test "Has version field" "grep -q 'version=' library.properties"
run_test "Has author field" "grep -q 'author=' library.properties"
run_test "Has architectures field" "grep -q 'architectures=' library.properties"
run_test "Specifies esp32 architecture" "grep -q 'esp32' library.properties"
run_test "Specifies esp8266 architecture" "grep -q 'esp8266' library.properties"

echo -e "${BLUE}4. library.json Validation${NC}\n"

run_test "library.json is valid JSON" "python -m json.tool library.json"
run_test "Has name field" "grep -q '\"name\"' library.json"
run_test "Has version field" "grep -q '\"version\"' library.json"
run_test "Has frameworks field" "grep -q '\"frameworks\"' library.json"

echo -e "${BLUE}5. Example Sketches Tests${NC}\n"

# Examples with their directory prefixes
EXAMPLES=(
    "01_BasicSetup:BasicSetup"
    "02_CustomSchema:CustomSchema"
    "03_MultiNetwork:MultiNetwork"
    "04_BLEProvisioning:BLEProvisioning"
    "05_SecurePortal:SecurePortal"
    "06_WithPlugin:WithPlugin"
    "07_BackupRestore:BackupRestore"
    "08_Diagnostics:Diagnostics"
    "09_OTA_Update:OTA_Update"
)

for EXAMPLE in "${EXAMPLES[@]}"; do
    DIR="${EXAMPLE%%:*}"
    FILE="${EXAMPLE##*:}"
    run_test "${FILE} sketch exists" "test -f examples/${DIR}/${FILE}.ino"
done

echo -e "${BLUE}6. Documentation Tests${NC}\n"

run_test "README has installation" "grep -q 'Installation' README.md"
run_test "README has quick start" "grep -q 'Quick Start' README.md"
run_test "README has examples" "grep -q 'Examples' README.md"
run_test "HARDWARE_TESTING.md exists" "test -f docs/HARDWARE_TESTING.md"

echo -e "${BLUE}7. Build System Tests${NC}\n"

run_test "Web builder script exists" "test -f tools/web_builder/build.py"
run_test "Web builder requirements exists" "test -f tools/web_builder/requirements.txt"
run_test "Build script exists" "test -f scripts/build.sh"
run_test "Build script is executable" "test -x scripts/build.sh"

echo -e "${BLUE}8. Code Quality Tests${NC}\n"

# Check for common issues
run_test "All headers have include guards" "find src -name '*.h' -exec grep -l '#ifndef' {} + | wc -l | grep -q '[0-9]'"
run_test "Main header exists and is clean" "test -f src/IonConnect.h && ! grep -q 'FIXME' src/IonConnect.h"

# Summary
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Test Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Total Tests:       ${TOTAL_TESTS}"
echo -e "${GREEN}Passed:            ${PASSED_TESTS}${NC}"
if [ $FAILED_TESTS -gt 0 ]; then
    echo -e "${RED}Failed:            ${FAILED_TESTS}${NC}"
else
    echo -e "Failed:            ${FAILED_TESTS}"
fi
echo -e "${BLUE}========================================${NC}\n"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}✓ All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}✗ Some tests failed.${NC}"
    exit 1
fi

