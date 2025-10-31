#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include "../core/IonTypes.h"
#include "../storage/StorageProvider.h"

namespace IonConnect {

/**
 * @brief Manages configuration schema, validation, and persistence
 * 
 * Handles:
 * - JSON schema loading and parsing
 * - Dynamic field validation
 * - Configuration storage via StorageProvider
 * - Config export/import for backup/restore
 * - Field encryption (passwords, tokens)
 */
class ConfigManager {
public:
    ConfigManager(StorageProvider* storage);
    ~ConfigManager();
    
    // Schema Management
    bool loadSchema(const char* jsonSchema);
    bool loadSchemaFromFile(const char* filepath);
    String getSchemaJSON();
    std::vector<ConfigField*> getFields();
    ConfigField* getField(const String& key);
    
    // Config Operations
    bool load();
    bool save();
    bool clear();
    String get(const String& key, const String& defaultValue = "");
    bool set(const String& key, const String& value);
    bool validate();
    bool isValid();
    
    // Backup/Restore
    String exportJSON();
    bool importJSON(const String& json);
    
    // Encryption
    String encryptValue(const String& value);
    String decryptValue(const String& encrypted);
    
private:
    StorageProvider* storage;
    JsonDocument schemaDoc;
    JsonDocument configDoc;
    std::vector<ConfigField*> fields;
    bool schemaLoaded;
    bool configLoaded;
    
    void parseSchema();
    void clearFields();
    bool validateField(const String& key, const String& value);
    bool shouldEncrypt(const String& key);
    
    // Storage keys
    static const char* KEY_CONFIG_DATA;
    static const char* KEY_SCHEMA_VERSION;
};

} // namespace IonConnect

#endif // CONFIG_MANAGER_H

