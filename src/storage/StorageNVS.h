#ifndef STORAGE_NVS_H
#define STORAGE_NVS_H

#include "StorageProvider.h"

#if ION_PLATFORM_ESP32

#include <Preferences.h>

namespace IonConnect {

/**
 * @brief NVS-based storage implementation for ESP32
 * 
 * Uses ESP32's Preferences library (NVS wrapper)
 */
class StorageNVS : public StorageProvider {
public:
    StorageNVS();
    virtual ~StorageNVS();
    
    bool begin(const char* ns = "ionconnect") override;
    void end() override;
    bool clear() override;
    bool exists(const char* key) override;
    
    String getString(const char* key, const String& defaultValue = "") override;
    bool putString(const char* key, const String& value) override;
    
    int getInt(const char* key, int defaultValue = 0) override;
    bool putInt(const char* key, int value) override;
    
    uint32_t getUInt(const char* key, uint32_t defaultValue = 0) override;
    bool putUInt(const char* key, uint32_t value) override;
    
    bool getBool(const char* key, bool defaultValue = false) override;
    bool putBool(const char* key, bool value) override;
    
    bool remove(const char* key) override;
    bool commit() override;
    
private:
    Preferences preferences;
    bool initialized;
    String currentNamespace;
};

} // namespace IonConnect

#endif // ION_PLATFORM_ESP32
#endif // STORAGE_NVS_H

