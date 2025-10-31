#include "ConfigManager.h"
#include "../utils/Logger.h"
#include "../utils/Crypto.h"

namespace IonConnect {

const char* ConfigManager::KEY_CONFIG_DATA = "config_data";
const char* ConfigManager::KEY_SCHEMA_VERSION = "schema_ver";

ConfigManager::ConfigManager(StorageProvider* storage) 
    : storage(storage), schemaLoaded(false), configLoaded(false) {
}

ConfigManager::~ConfigManager() {
    clearFields();
}

bool ConfigManager::loadSchema(const char* jsonSchema) {
    DeserializationError error = deserializeJson(schemaDoc, jsonSchema);
    
    if (error) {
        ION_LOG_E("Failed to parse schema: %s", error.c_str());
        return false;
    }
    
    parseSchema();
    schemaLoaded = true;
    
    ION_LOG("Schema loaded: %d fields", fields.size());
    return true;
}

bool ConfigManager::loadSchemaFromFile(const char* filepath) {
#if ION_USE_LITTLEFS
    #if ION_PLATFORM_ESP32
        #include <LittleFS.h>
    #elif ION_PLATFORM_ESP8266
        #include <FS.h>
    #endif
    
    if (!LittleFS.begin()) {
        ION_LOG_E("LittleFS mount failed");
        return false;
    }
    
    File file = LittleFS.open(filepath, "r");
    if (!file) {
        ION_LOG_E("Failed to open schema file: %s", filepath);
        return false;
    }
    
    DeserializationError error = deserializeJson(schemaDoc, file);
    file.close();
    
    if (error) {
        ION_LOG_E("Failed to parse schema file: %s", error.c_str());
        return false;
    }
    
    parseSchema();
    schemaLoaded = true;
    
    ION_LOG("Schema loaded from file: %d fields", fields.size());
    return true;
#else
    ION_LOG_E("LittleFS not enabled");
    return false;
#endif
}

String ConfigManager::getSchemaJSON() {
    String output;
    serializeJson(schemaDoc, output);
    return output;
}

std::vector<ConfigField*> ConfigManager::getFields() {
    return fields;
}

ConfigField* ConfigManager::getField(const String& key) {
    for (auto field : fields) {
        if (field->id == key) {
            return field;
        }
    }
    return nullptr;
}

bool ConfigManager::load() {
    if (!storage) {
        ION_LOG_E("Storage not initialized");
        return false;
    }
    
    String configJson = storage->getString(KEY_CONFIG_DATA, "{}");
    
    DeserializationError error = deserializeJson(configDoc, configJson);
    if (error) {
        ION_LOG_E("Failed to parse stored config: %s", error.c_str());
        configDoc.clear();
        configDoc.to<JsonObject>();
        return false;
    }
    
    configLoaded = true;
    ION_LOG("Config loaded from storage");
    return true;
}

bool ConfigManager::save() {
    if (!storage) {
        ION_LOG_E("Storage not initialized");
        return false;
    }
    
    String configJson;
    serializeJson(configDoc, configJson);
    
    bool success = storage->putString(KEY_CONFIG_DATA, configJson);
    
    if (success) {
        success = storage->commit();
    }
    
    if (success) {
        ION_LOG("Config saved to storage");
    } else {
        ION_LOG_E("Failed to save config");
    }
    
    return success;
}

bool ConfigManager::clear() {
    if (!storage) {
        return false;
    }
    
    configDoc.clear();
    configDoc.to<JsonObject>();
    
    storage->remove(KEY_CONFIG_DATA);
    storage->commit();
    
    ION_LOG("Config cleared");
    return true;
}

String ConfigManager::get(const String& key, const String& defaultValue) {
    if (!configLoaded && !load()) {
        return defaultValue;
    }
    
    JsonObject root = configDoc.as<JsonObject>();
    if (root.containsKey(key)) {
        String value = root[key].as<String>();
        
        // Check if value is encrypted
        if (value.startsWith("enc:")) {
            value = decryptValue(value.substring(4));
        }
        
        return value;
    }
    
    // Try default from schema
    ConfigField* field = getField(key);
    if (field && !field->defaultValue.isEmpty()) {
        return field->defaultValue;
    }
    
    return defaultValue;
}

bool ConfigManager::set(const String& key, const String& value) {
    // Validate first
    if (!validateField(key, value)) {
        ION_LOG_W("Validation failed for field: %s", key.c_str());
        return false;
    }
    
    JsonObject root = configDoc.as<JsonObject>();
    
    // Encrypt if needed
    if (shouldEncrypt(key)) {
        String encrypted = "enc:" + encryptValue(value);
        root[key] = encrypted;
    } else {
        root[key] = value;
    }
    
    configLoaded = true;
    return true;
}

bool ConfigManager::validate() {
    if (!schemaLoaded) {
        ION_LOG_W("Schema not loaded, skipping validation");
        return true;
    }
    
    JsonObject root = configDoc.as<JsonObject>();
    
    // Check all required fields
    for (auto field : fields) {
        if (field->required) {
            if (!root.containsKey(field->id) || root[field->id].as<String>().isEmpty()) {
                ION_LOG_E("Required field missing: %s", field->id.c_str());
                return false;
            }
        }
    }
    
    return true;
}

bool ConfigManager::isValid() {
    return validate();
}

String ConfigManager::exportJSON() {
    JsonDocument exportDoc;
    JsonObject root = exportDoc.to<JsonObject>();
    
    root["version"] = "1.0";
    root["timestamp"] = millis() / 1000;
    root["config"] = configDoc.as<JsonObject>();
    
    String output;
    serializeJson(exportDoc, output);
    return output;
}

bool ConfigManager::importJSON(const String& json) {
    JsonDocument importDoc;
    
    DeserializationError error = deserializeJson(importDoc, json);
    if (error) {
        ION_LOG_E("Failed to parse import JSON: %s", error.c_str());
        return false;
    }
    
    if (!importDoc.containsKey("config")) {
        ION_LOG_E("Invalid import format: missing 'config' field");
        return false;
    }
    
    configDoc = importDoc["config"];
    configLoaded = true;
    
    return save();
}

String ConfigManager::encryptValue(const String& value) {
    return Crypto::encrypt(value);
}

String ConfigManager::decryptValue(const String& encrypted) {
    return Crypto::decrypt(encrypted);
}

void ConfigManager::parseSchema() {
    clearFields();
    
    if (!schemaDoc.containsKey("fields")) {
        ION_LOG_E("Schema missing 'fields' array");
        return;
    }
    
    JsonArray fieldsArray = schemaDoc["fields"].as<JsonArray>();
    
    for (JsonVariant v : fieldsArray) {
        JsonObject fieldObj = v.as<JsonObject>();
        
        ConfigField* field = new ConfigField();
        field->id = fieldObj["id"].as<String>();
        field->label = fieldObj["label"] | "";
        field->type = fieldObj["type"] | "text";
        field->defaultValue = fieldObj["default"] | "";
        field->placeholder = fieldObj["placeholder"] | "";
        field->required = fieldObj["required"] | false;
        field->encrypted = fieldObj["encrypted"] | false;
        field->minLength = fieldObj["minLength"] | 0;
        field->maxLength = fieldObj["maxLength"] | 0;
        field->min = fieldObj["min"] | 0;
        field->max = fieldObj["max"] | 0;
        field->pattern = fieldObj["pattern"] | "";
        field->visibleIf = fieldObj["visible_if"] | "";
        
        // Parse options for select type
        if (fieldObj.containsKey("options")) {
            JsonArray opts = fieldObj["options"].as<JsonArray>();
            field->optionsCount = opts.size();
            if (field->optionsCount > 0) {
                field->options = new String[field->optionsCount];
                size_t i = 0;
                for (JsonVariant opt : opts) {
                    field->options[i++] = opt.as<String>();
                }
            }
        }
        
        fields.push_back(field);
    }
}

void ConfigManager::clearFields() {
    for (auto field : fields) {
        delete field;
    }
    fields.clear();
}

bool ConfigManager::validateField(const String& key, const String& value) {
    ConfigField* field = getField(key);
    if (!field) {
        return true; // No schema, allow any field
    }
    
    // Required check
    if (field->required && value.isEmpty()) {
        return false;
    }
    
    // Skip further validation if empty and not required
    if (value.isEmpty()) {
        return true;
    }
    
    // Type-specific validation
    if (field->type == "number") {
        int val = value.toInt();
        if (field->min != 0 && val < field->min) return false;
        if (field->max != 0 && val > field->max) return false;
    }
    
    // Length validation
    if (field->minLength > 0 && value.length() < (size_t)field->minLength) {
        return false;
    }
    if (field->maxLength > 0 && value.length() > (size_t)field->maxLength) {
        return false;
    }
    
    // Pattern validation (basic, can be enhanced)
    if (!field->pattern.isEmpty()) {
        // Simple pattern check - for now just check if pattern is substring
        // Full regex would require regex library
        if (value.indexOf(field->pattern) == -1) {
            ION_LOG_W("Pattern validation not fully implemented");
        }
    }
    
    return true;
}

bool ConfigManager::shouldEncrypt(const String& key) {
    ConfigField* field = getField(key);
    if (!field) {
        // Default encryption for known sensitive keys
        return (key == "wifi_pass" || key == "password" || 
                key == "access_token" || key.endsWith("_pass") ||
                key.endsWith("_password") || key.endsWith("_token"));
    }
    
    return field->encrypted;
}

} // namespace IonConnect

