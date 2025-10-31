/**
 * @file IonConnect.h
 * @brief Next-generation WiFi provisioning library for ESP32/ESP8266
 * @version 1.0.0
 * @author IonConnect Team
 * @license MIT
 * 
 * Inspired by WiFiManager by tzapu, reimagined with modern async patterns,
 * enhanced security, plugin architecture, and a beautiful live-updating UI.
 * 
 * Features:
 * - Async + Smart Connection Core
 * - Live Provisioning Dashboard (SSE)
 * - Dynamic JSON-Driven UI
 * - REST API for Config + Status
 * - BLE Fallback Provisioning (ESP32)
 * - Unified Setup + OTA Web Panel
 * - Secure Token/PIN Access
 * - Modern Responsive UI (Tailwind)
 * - mDNS + QR Quick Access
 * - Config Backup/Restore
 * - Modular Plugin Architecture
 * - Built-in Diagnostics Panel
 */

#ifndef ION_CONNECT_H
#define ION_CONNECT_H

#include "core/IonTypes.h"
#include "core/IonConfig.h"
#include "core/IonConnectBase.h"

// Platform-specific implementations
#if ION_PLATFORM_ESP32
    #include "core/IonConnectESP32.h"
#elif ION_PLATFORM_ESP8266
    #include "core/IonConnectESP8266.h"
#endif

namespace IonConnect {

// Factory function to create platform-specific instance
inline IonConnectBase* createInstance() {
    #if ION_PLATFORM_ESP32
        return new IonConnectESP32();
    #elif ION_PLATFORM_ESP8266
        return new IonConnectESP8266();
    #else
        #error "Unsupported platform"
    #endif
}

} // namespace IonConnect

// Convenience typedef for user code  
#if ION_PLATFORM_ESP32
    using IonConnectDevice = IonConnect::IonConnectESP32;
#elif ION_PLATFORM_ESP8266
    using IonConnectDevice = IonConnect::IonConnectESP8266;
#endif

// Bring commonly used types into global namespace for convenience
using IonConnect::IonConfig;
using IonConnect::WiFiState;
using IonConnect::IonError;

#endif // ION_CONNECT_H

