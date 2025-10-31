#include "../core/IonTypes.h"
#include "StorageEEPROM.h"

#if ION_PLATFORM_ESP8266

namespace IonConnect {

StorageEEPROM::StorageEEPROM() : initialized(false), dirty(false) {
}

StorageEEPROM::~StorageEEPROM() {
    end();
}

bool StorageEEPROM::begin(const char* ns) {
    if (initialized && currentNamespace == ns) {
        return true;
    }
    
    currentNamespace = ns;
    EEPROM.begin(EEPROM_SIZE);
    
    initialized = loadFromEEPROM();
    if (!initialized) {
        // Initialize empty storage
        cache.clear();
        initialized = true;
        dirty = true;
        saveToEEPROM();
    }
    
    return initialized;
}

void StorageEEPROM::end() {
    if (initialized && dirty) {
        commit();
    }
    EEPROM.end();
    initialized = false;
}

bool StorageEEPROM::clear() {
    if (!initialized) return false;
    
    cache.clear();
    dirty = true;
    return saveToEEPROM();
}

bool StorageEEPROM::exists(const char* key) {
    if (!initialized) return false;
    return cache.find(key) != cache.end();
}

String StorageEEPROM::getString(const char* key, const String& defaultValue) {
    if (!initialized) return defaultValue;
    
    auto it = cache.find(key);
    if (it == cache.end() || it->second.type != TYPE_STRING) {
        return defaultValue;
    }
    
    return it->second.value;
}

bool StorageEEPROM::putString(const char* key, const String& value) {
    if (!initialized) return false;
    
    Entry entry;
    entry.type = TYPE_STRING;
    entry.value = value;
    cache[key] = entry;
    dirty = true;
    
    return true;
}

int StorageEEPROM::getInt(const char* key, int defaultValue) {
    if (!initialized) return defaultValue;
    
    auto it = cache.find(key);
    if (it == cache.end() || it->second.type != TYPE_INT) {
        return defaultValue;
    }
    
    return it->second.value.toInt();
}

bool StorageEEPROM::putInt(const char* key, int value) {
    if (!initialized) return false;
    
    Entry entry;
    entry.type = TYPE_INT;
    entry.value = String(value);
    cache[key] = entry;
    dirty = true;
    
    return true;
}

uint32_t StorageEEPROM::getUInt(const char* key, uint32_t defaultValue) {
    if (!initialized) return defaultValue;
    
    auto it = cache.find(key);
    if (it == cache.end() || it->second.type != TYPE_UINT) {
        return defaultValue;
    }
    
    return (uint32_t)it->second.value.toInt();
}

bool StorageEEPROM::putUInt(const char* key, uint32_t value) {
    if (!initialized) return false;
    
    Entry entry;
    entry.type = TYPE_UINT;
    entry.value = String(value);
    cache[key] = entry;
    dirty = true;
    
    return true;
}

bool StorageEEPROM::getBool(const char* key, bool defaultValue) {
    if (!initialized) return defaultValue;
    
    auto it = cache.find(key);
    if (it == cache.end() || it->second.type != TYPE_BOOL) {
        return defaultValue;
    }
    
    return it->second.value == "1";
}

bool StorageEEPROM::putBool(const char* key, bool value) {
    if (!initialized) return false;
    
    Entry entry;
    entry.type = TYPE_BOOL;
    entry.value = value ? "1" : "0";
    cache[key] = entry;
    dirty = true;
    
    return true;
}

bool StorageEEPROM::remove(const char* key) {
    if (!initialized) return false;
    
    auto it = cache.find(key);
    if (it != cache.end()) {
        cache.erase(it);
        dirty = true;
        return true;
    }
    
    return false;
}

bool StorageEEPROM::commit() {
    if (!initialized || !dirty) return true;
    
    bool success = saveToEEPROM();
    if (success) {
        dirty = false;
    }
    
    return success;
}

bool StorageEEPROM::loadFromEEPROM() {
    uint16_t addr = 0;
    
    // Check magic
    uint32_t magic = 0;
    for (int i = 0; i < 4; i++) {
        magic |= ((uint32_t)EEPROM.read(addr++) << (i * 8));
    }
    
    if (magic != MAGIC) {
        return false;
    }
    
    // Check version
    uint8_t version = EEPROM.read(addr++);
    if (version != VERSION) {
        return false;
    }
    
    // Read namespace length and namespace
    uint8_t nsLen = EEPROM.read(addr++);
    String ns = "";
    for (uint8_t i = 0; i < nsLen; i++) {
        ns += (char)EEPROM.read(addr++);
    }
    
    if (ns != currentNamespace) {
        return false;
    }
    
    // Read entries
    cache.clear();
    while (addr < EEPROM_SIZE - 3) { // Need at least 3 bytes for entry header
        uint8_t keyLen = EEPROM.read(addr++);
        if (keyLen == 0 || keyLen == 0xFF) break; // End marker or uninitialized
        
        // Read key
        String key = "";
        for (uint8_t i = 0; i < keyLen; i++) {
            key += (char)EEPROM.read(addr++);
        }
        
        // Read type and data
        DataType type = (DataType)EEPROM.read(addr++);
        uint16_t dataLen = EEPROM.read(addr++);
        dataLen |= ((uint16_t)EEPROM.read(addr++) << 8);
        
        String value = "";
        for (uint16_t i = 0; i < dataLen; i++) {
            value += (char)EEPROM.read(addr++);
        }
        
        Entry entry;
        entry.type = type;
        entry.value = value;
        cache[key] = entry;
    }
    
    return true;
}

bool StorageEEPROM::saveToEEPROM() {
    uint16_t addr = 0;
    
    // Write magic
    for (int i = 0; i < 4; i++) {
        EEPROM.write(addr++, (MAGIC >> (i * 8)) & 0xFF);
    }
    
    // Write version
    EEPROM.write(addr++, VERSION);
    
    // Write namespace
    uint8_t nsLen = currentNamespace.length();
    EEPROM.write(addr++, nsLen);
    for (uint8_t i = 0; i < nsLen; i++) {
        EEPROM.write(addr++, currentNamespace[i]);
    }
    
    // Write entries
    for (const auto& pair : cache) {
        const String& key = pair.first;
        const Entry& entry = pair.second;
        
        // Check if we have enough space
        uint16_t needed = 1 + key.length() + 1 + 2 + entry.value.length();
        if (addr + needed >= EEPROM_SIZE) {
            return false; // Not enough space
        }
        
        // Write key length and key
        EEPROM.write(addr++, (uint8_t)key.length());
        for (size_t i = 0; i < key.length(); i++) {
            EEPROM.write(addr++, key[i]);
        }
        
        // Write type
        EEPROM.write(addr++, (uint8_t)entry.type);
        
        // Write data length (2 bytes)
        uint16_t dataLen = entry.value.length();
        EEPROM.write(addr++, dataLen & 0xFF);
        EEPROM.write(addr++, (dataLen >> 8) & 0xFF);
        
        // Write data
        for (uint16_t i = 0; i < dataLen; i++) {
            EEPROM.write(addr++, entry.value[i]);
        }
    }
    
    // Write end marker
    if (addr < EEPROM_SIZE) {
        EEPROM.write(addr, 0);
    }
    
    return EEPROM.commit();
}

} // namespace IonConnect

#endif // ION_PLATFORM_ESP8266

