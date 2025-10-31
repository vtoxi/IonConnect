#include "../core/IonTypes.h"
#include "StorageNVS.h"

#if ION_PLATFORM_ESP32

namespace IonConnect {

StorageNVS::StorageNVS() : initialized(false) {
}

StorageNVS::~StorageNVS() {
    end();
}

bool StorageNVS::begin(const char* ns) {
    if (initialized && currentNamespace == ns) {
        return true;
    }
    
    if (initialized) {
        end();
    }
    
    currentNamespace = ns;
    initialized = preferences.begin(ns, false); // false = read/write mode
    
    return initialized;
}

void StorageNVS::end() {
    if (initialized) {
        preferences.end();
        initialized = false;
    }
}

bool StorageNVS::clear() {
    if (!initialized) return false;
    return preferences.clear();
}

bool StorageNVS::exists(const char* key) {
    if (!initialized) return false;
    return preferences.isKey(key);
}

String StorageNVS::getString(const char* key, const String& defaultValue) {
    if (!initialized) return defaultValue;
    return preferences.getString(key, defaultValue);
}

bool StorageNVS::putString(const char* key, const String& value) {
    if (!initialized) return false;
    return preferences.putString(key, value) > 0;
}

int StorageNVS::getInt(const char* key, int defaultValue) {
    if (!initialized) return defaultValue;
    return preferences.getInt(key, defaultValue);
}

bool StorageNVS::putInt(const char* key, int value) {
    if (!initialized) return false;
    return preferences.putInt(key, value) > 0;
}

uint32_t StorageNVS::getUInt(const char* key, uint32_t defaultValue) {
    if (!initialized) return defaultValue;
    return preferences.getUInt(key, defaultValue);
}

bool StorageNVS::putUInt(const char* key, uint32_t value) {
    if (!initialized) return false;
    return preferences.putUInt(key, value) > 0;
}

bool StorageNVS::getBool(const char* key, bool defaultValue) {
    if (!initialized) return defaultValue;
    return preferences.getBool(key, defaultValue);
}

bool StorageNVS::putBool(const char* key, bool value) {
    if (!initialized) return false;
    return preferences.putBool(key, value) > 0;
}

bool StorageNVS::remove(const char* key) {
    if (!initialized) return false;
    return preferences.remove(key);
}

bool StorageNVS::commit() {
    // NVS auto-commits, no action needed
    return true;
}

} // namespace IonConnect

#endif // ION_PLATFORM_ESP32

