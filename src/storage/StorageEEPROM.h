#ifndef STORAGE_EEPROM_H
#define STORAGE_EEPROM_H

#include "StorageProvider.h"

#if ION_PLATFORM_ESP8266

#include <EEPROM.h>
#include <map>

namespace IonConnect {

/**
 * @brief EEPROM-based storage implementation for ESP8266
 * 
 * Emulates NVS-like key-value storage using EEPROM
 * Data format: [magic][version][key1_len][key1][type][data_len][data][key2_len][key2]...
 */
class StorageEEPROM : public StorageProvider {
public:
    StorageEEPROM();
    virtual ~StorageEEPROM();
    
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
    static const uint16_t EEPROM_SIZE = 4096;
    static const uint32_t MAGIC = 0x494F4E43; // "IONC"
    static const uint8_t VERSION = 1;
    
    enum DataType : uint8_t {
        TYPE_STRING = 1,
        TYPE_INT = 2,
        TYPE_UINT = 3,
        TYPE_BOOL = 4
    };
    
    struct Entry {
        DataType type;
        String value;
    };
    
    bool initialized;
    bool dirty;
    String currentNamespace;
    std::map<String, Entry> cache;
    
    bool loadFromEEPROM();
    bool saveToEEPROM();
    void buildCache();
};

} // namespace IonConnect

#endif // ION_PLATFORM_ESP8266
#endif // STORAGE_EEPROM_H

