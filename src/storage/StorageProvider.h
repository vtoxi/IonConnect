#ifndef STORAGE_PROVIDER_H
#define STORAGE_PROVIDER_H

#include <Arduino.h>

namespace IonConnect {

/**
 * @brief Abstract interface for persistent storage
 * 
 * Provides a unified interface for NVS (ESP32) and EEPROM (ESP8266)
 */
class StorageProvider {
public:
    virtual ~StorageProvider() {}
    
    /**
     * @brief Initialize storage with namespace
     * @param ns Namespace/prefix for storage keys
     * @return true if initialization successful
     */
    virtual bool begin(const char* ns = "ionconnect") = 0;
    
    /**
     * @brief End/close storage
     */
    virtual void end() = 0;
    
    /**
     * @brief Clear all stored data
     * @return true if successful
     */
    virtual bool clear() = 0;
    
    /**
     * @brief Check if a key exists
     * @param key Storage key
     * @return true if key exists
     */
    virtual bool exists(const char* key) = 0;
    
    /**
     * @brief Get string value
     * @param key Storage key
     * @param defaultValue Default if key doesn't exist
     * @return Stored value or default
     */
    virtual String getString(const char* key, const String& defaultValue = "") = 0;
    
    /**
     * @brief Set string value
     * @param key Storage key
     * @param value Value to store
     * @return true if successful
     */
    virtual bool putString(const char* key, const String& value) = 0;
    
    /**
     * @brief Get integer value
     * @param key Storage key
     * @param defaultValue Default if key doesn't exist
     * @return Stored value or default
     */
    virtual int getInt(const char* key, int defaultValue = 0) = 0;
    
    /**
     * @brief Set integer value
     * @param key Storage key
     * @param value Value to store
     * @return true if successful
     */
    virtual bool putInt(const char* key, int value) = 0;
    
    /**
     * @brief Get unsigned integer value
     * @param key Storage key
     * @param defaultValue Default if key doesn't exist
     * @return Stored value or default
     */
    virtual uint32_t getUInt(const char* key, uint32_t defaultValue = 0) = 0;
    
    /**
     * @brief Set unsigned integer value
     * @param key Storage key
     * @param value Value to store
     * @return true if successful
     */
    virtual bool putUInt(const char* key, uint32_t value) = 0;
    
    /**
     * @brief Get boolean value
     * @param key Storage key
     * @param defaultValue Default if key doesn't exist
     * @return Stored value or default
     */
    virtual bool getBool(const char* key, bool defaultValue = false) = 0;
    
    /**
     * @brief Set boolean value
     * @param key Storage key
     * @param value Value to store
     * @return true if successful
     */
    virtual bool putBool(const char* key, bool value) = 0;
    
    /**
     * @brief Remove a key
     * @param key Storage key
     * @return true if successful
     */
    virtual bool remove(const char* key) = 0;
    
    /**
     * @brief Commit changes to storage (for EEPROM)
     * @return true if successful
     */
    virtual bool commit() = 0;
};

} // namespace IonConnect

#endif // STORAGE_PROVIDER_H

