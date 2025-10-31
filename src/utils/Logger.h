#ifndef ION_LOGGER_H
#define ION_LOGGER_H

#include <Arduino.h>
#include "../core/IonTypes.h"

namespace IonConnect {

#if ION_DEBUG
    #define ION_LOG(fmt, ...) Logger::log("[ION] " fmt, ##__VA_ARGS__)
    #define ION_LOG_E(fmt, ...) Logger::error("[ION ERROR] " fmt, ##__VA_ARGS__)
    #define ION_LOG_W(fmt, ...) Logger::warn("[ION WARN] " fmt, ##__VA_ARGS__)
#else
    #define ION_LOG(fmt, ...)
    #define ION_LOG_E(fmt, ...)
    #define ION_LOG_W(fmt, ...)
#endif

class Logger {
public:
    static void log(const char* fmt, ...) {
        char buffer[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        Serial.println(buffer);
    }
    
    static void error(const char* fmt, ...) {
        char buffer[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        Serial.println(buffer);
    }
    
    static void warn(const char* fmt, ...) {
        char buffer[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        Serial.println(buffer);
    }
};

} // namespace IonConnect

#endif // ION_LOGGER_H

