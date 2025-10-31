# IonConnect Hardware Testing Guide

This guide provides comprehensive testing procedures for validating IonConnect on real ESP32 and ESP8266 hardware.

## 🎯 Testing Overview

### Test Categories
1. **Basic Functionality** - Core WiFi provisioning
2. **Network Management** - Multi-network, reconnection
3. **Security** - Authentication, encryption
4. **Web Interface** - Portal UI, API endpoints
5. **Performance** - Memory, stability, speed
6. **Platform-Specific** - ESP32 BLE, ESP8266 limits
7. **Edge Cases** - Error handling, timeouts
8. **Integration** - Real-world scenarios

---

## 🛠️ Test Setup

### Required Hardware

#### ESP32 Testing
- ESP32 DevKit C
- ESP32-S3 DevKit (optional)
- ESP32-C3 DevKit (optional)
- USB cable
- Serial monitor access

#### ESP8266 Testing
- NodeMCU ESP8266
- Wemos D1 Mini
- ESP-12F module (advanced)
- USB cable
- Serial monitor access

### Required Software
- Arduino IDE 2.x or PlatformIO
- Serial monitor tool
- Web browser (Chrome/Firefox)
- Mobile device with WiFi
- Optional: BLE scanner app (ESP32)

### Test Networks
Prepare multiple WiFi networks:
1. **Open Network** - No password
2. **WPA2 Network** - Standard security
3. **WPA3 Network** - Modern security (if available)
4. **Weak Signal** - Far from device
5. **5GHz Network** - For 5GHz capable ESP32 variants

---

## ✅ Test Procedures

### Test 1: First Boot & Portal

**Objective**: Verify captive portal starts on first boot

**Steps**:
1. Flash BasicSetup example
2. Power on device
3. Observe serial output
4. Scan for WiFi networks
5. Connect to `IonConnect-XXXX`
6. Verify browser auto-opens portal

**Expected Results**:
```
✓ Portal started
✓ AP SSID visible
✓ Captive portal redirects
✓ UI loads correctly
✓ No serial errors
```

**Pass Criteria**:
- Portal SSID visible within 10 seconds
- Browser opens portal automatically
- UI displays correctly on mobile and desktop

---

### Test 2: WiFi Configuration

**Objective**: Configure WiFi and connect

**Steps**:
1. In portal, click "Scan Networks"
2. Wait for scan to complete
3. Verify networks list appears
4. Click a network
5. SSID should populate
6. Enter password
7. Click "Save & Connect"
8. Watch live status updates

**Expected Results**:
```
✓ Scan completes in <5 seconds
✓ Networks sorted by signal strength
✓ SSID auto-fills on click
✓ Status shows "Connecting..."
✓ Status shows "Connected!" with IP
✓ Device accessible at new IP
```

**Pass Criteria**:
- Successful connection within 15 seconds
- Live status updates work
- No page refresh required
- IP address displayed correctly

---

### Test 3: Multi-Network Management

**Objective**: Test multiple saved networks

**Steps**:
1. Flash MultiNetwork example
2. Pre-configure 3 networks
3. Move device between coverage areas
4. Observe auto-switching behavior
5. Check reconnection logic

**Expected Results**:
```
✓ Connects to highest priority network
✓ Auto-switches when signal changes
✓ Reconnects after temporary disconnect
✓ Portal fallback after max retries
```

**Pass Criteria**:
- Connects to strongest known network
- Exponential backoff works
- Max 30 second reconnection time
- No crashes during switching

---

### Test 4: Configuration Persistence

**Objective**: Verify settings survive reboot

**Steps**:
1. Configure WiFi via portal
2. Verify connection
3. Power cycle device
4. Wait for auto-connect
5. Verify same network connected
6. Check custom config fields preserved

**Expected Results**:
```
✓ Auto-connects on reboot
✓ Custom fields retained
✓ Network priorities preserved
✓ Timestamps updated correctly
```

**Pass Criteria**:
- Auto-connect within 10 seconds
- All config values retained
- No portal unless configured

---

### Test 5: Security Features

**Objective**: Test authentication and encryption

**Steps**:
1. Flash SecurePortal example
2. Start portal
3. Try accessing without password
4. Enter correct password
5. Try API without token
6. Access API with valid token
7. Verify credential encryption

**Expected Results**:
```
✓ Portal requires password
✓ Invalid password rejected
✓ API requires token
✓ Invalid token → 401 error
✓ WiFi password encrypted in storage
```

**Pass Criteria**:
- Authentication works
- Unauthorized access blocked
- Passwords not stored in plaintext

---

### Test 6: Web Interface

**Objective**: Test all portal features

**Steps**:
1. Test on multiple browsers
2. Test on mobile device
3. Verify all API endpoints
4. Test configuration backup/restore
5. Test diagnostics panel
6. Verify SSE updates

**API Endpoints to Test**:
```
GET  /api/schema          → Returns schema
GET  /api/config          → Returns config (no passwords)
POST /api/config          → Saves config
POST /api/scan            → Starts scan
GET  /api/networks        → Returns scan results
GET  /api/status          → Returns WiFi status
GET  /api/diagnostics     → Returns health metrics
POST /api/clear           → Clears config
GET  /api/export          → Exports config JSON
POST /api/import          → Imports config JSON
POST /api/reboot          → Restarts device
GET  /api/events          → SSE stream
```

**Pass Criteria**:
- All endpoints return correct data
- UI responsive on mobile
- SSE updates work
- No JavaScript errors

---

### Test 7: Memory & Stability

**Objective**: Validate memory usage and stability

**Steps**:
1. Flash Diagnostics example
2. Monitor heap usage
3. Run portal for 1 hour
4. Perform 100 API requests
5. Scan networks 50 times
6. Monitor for memory leaks
7. Check for crashes

**Expected Results**:
```
✓ Heap stable over time
✓ No memory leaks detected
✓ Min free heap >10KB (ESP8266) or >20KB (ESP32)
✓ No crashes or reboots
✓ API responsive throughout
```

**Pass Criteria**:
- ESP32: Min 20KB free heap
- ESP8266: Min 5KB free heap
- No unexpected reboots
- 24-hour stability test passes

---

### Test 8: BLE Provisioning (ESP32 Only)

**Objective**: Verify BLE provisioning works

**Steps**:
1. Flash BLEProvisioning example
2. Scan for BLE device on mobile
3. Connect to `BLE-IoT-Device`
4. Read schema characteristic
5. Write config via BLE
6. Monitor status notifications
7. Verify WiFi connection

**BLE Service UUIDs**:
```
Service: 0000ff01-0000-1000-8000-00805f9b34fb
Schema:  0000ff02-0000-1000-8000-00805f9b34fb (Read)
Config:  0000ff03-0000-1000-8000-00805f9b34fb (Write)
Status:  0000ff04-0000-1000-8000-00805f9b34fb (Notify)
Control: 0000ff05-0000-1000-8000-00805f9b34fb (Write)
```

**Expected Results**:
```
✓ BLE device visible
✓ Schema readable
✓ Config write successful
✓ Status notifications work
✓ WiFi connects via BLE config
```

**Pass Criteria**:
- BLE advertising starts
- Characteristics accessible
- Configuration via BLE works
- Both BLE and WiFi can coexist

---

### Test 9: OTA Updates

**Objective**: Test firmware updates

**Steps**:
1. Flash OTA_Update example (v1.0)
2. Modify firmware (add Serial.println)
3. Compile new binary (v1.1)
4. Navigate to /update
5. Upload new firmware
6. Monitor progress
7. Verify reboot and new version

**Expected Results**:
```
✓ Update page loads
✓ Upload starts
✓ Progress bar updates
✓ Update completes
✓ Device reboots
✓ New code running
✓ Settings preserved
```

**Pass Criteria**:
- Upload completes successfully
- No corruption
- Settings survive update
- Rollback works on failure

---

### Test 10: Edge Cases

**Objective**: Test error handling

**Test Cases**:

#### 10.1: Wrong Password
- Enter incorrect WiFi password
- Verify failure message
- Verify can retry

#### 10.2: Network Disappears
- Connect to network
- Turn off router
- Verify reconnection attempts
- Verify portal fallback

#### 10.3: Portal Timeout
- Start portal with 60s timeout
- Wait without configuring
- Verify portal closes
- Verify callback triggered

#### 10.4: Invalid Configuration
- Submit empty SSID
- Submit invalid JSON
- Verify validation errors
- Verify no crash

#### 10.5: Concurrent Connections
- Multiple devices access portal
- Verify all can configure
- Check for race conditions

#### 10.6: Rapid Reboots
- Power cycle 10 times quickly
- Verify no corruption
- Verify portal still works

**Pass Criteria**:
- All errors handled gracefully
- Clear error messages
- No crashes or corruption
- Recovery possible

---

## 📊 Performance Benchmarks

### Memory Usage

| Platform | Flash Used | RAM Used | Min Free Heap |
|----------|-----------|----------|---------------|
| ESP32    | ~300KB    | ~80KB    | >20KB         |
| ESP8266  | ~260KB    | ~50KB    | >5KB          |

### Timing Benchmarks

| Operation | Target | Acceptable |
|-----------|--------|------------|
| Portal Start | <5s | <10s |
| WiFi Scan | <3s | <5s |
| Connection | <10s | <15s |
| Reconnect | <30s | <60s |
| API Response | <100ms | <500ms |
| SSE Update | <1s | <3s |

### Stability Targets

| Metric | Target |
|--------|--------|
| Uptime | >7 days |
| MTBF | >30 days |
| Memory Leaks | None |
| API Errors | <0.1% |

---

## 🐛 Known Issues & Workarounds

### ESP8266 Limitations
- **Issue**: Limited heap memory
- **Workaround**: Disable diagnostics, use minimal schema
- **Threshold**: Keep free heap >5KB

### BLE Coexistence (ESP32)
- **Issue**: WiFi+BLE can affect performance
- **Workaround**: Disable BLE after provisioning
- **Impact**: ~20% slower WiFi when BLE active

### Captive Portal Detection
- **Issue**: Some Android devices don't auto-open
- **Workaround**: User manually navigates to 192.168.4.1
- **Status**: Device limitation, not library bug

### Large Schemas
- **Issue**: Very large schemas (>2KB) may cause issues on ESP8266
- **Workaround**: Use minimal schema, load additional fields dynamically
- **Limit**: Keep schema <1KB for ESP8266

---

## 📝 Test Report Template

```markdown
# IonConnect Test Report

**Date**: YYYY-MM-DD
**Tester**: [Name]
**Hardware**: ESP32 DevKit / NodeMCU
**Firmware Version**: 1.0.0

## Test Results

| Test # | Test Name | Status | Notes |
|--------|-----------|--------|-------|
| 1 | First Boot | ✅ PASS | Portal started in 3s |
| 2 | WiFi Config | ✅ PASS | Connected in 8s |
| 3 | Multi-Network | ✅ PASS | Switching works |
| ... | ... | ... | ... |

## Performance Metrics

- Flash Used: 285KB
- Min Free Heap: 28KB
- Portal Start: 4.2s
- Connection Time: 7.8s

## Issues Found

1. None

## Recommendations

1. Ready for production
2. Consider enabling diagnostics logging

## Overall Status

✅ **PASS** - All tests passed
```

---

## 🚀 Deployment Checklist

Before deploying to production:

- [ ] All tests pass on target hardware
- [ ] 24-hour stability test completed
- [ ] Memory leaks checked
- [ ] Security features tested
- [ ] OTA updates verified
- [ ] Custom schema validated
- [ ] Edge cases handled
- [ ] Documentation reviewed
- [ ] Default passwords changed
- [ ] Logging configured
- [ ] Monitoring setup
- [ ] Rollback plan ready

---

## 📚 Additional Resources

- [IonConnect API Reference](API.md)
- [Security Best Practices](SECURITY.md)
- [Troubleshooting Guide](TROUBLESHOOTING.md)
- [Example Sketches](../examples/)

---

**Happy Testing! 🎉**

