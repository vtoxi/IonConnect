#include "AssetManager.h"
#include "../utils/Logger.h"
#include "assets/embedded_html.h"

#if ION_USE_LITTLEFS
    #if ION_PLATFORM_ESP32
        #include <LittleFS.h>
    #elif ION_PLATFORM_ESP8266
        #include <LittleFS.h>
    #endif
#endif

namespace IonConnect {

AssetManager::AssetManager() : littleFSAvailable(false) {
}

bool AssetManager::begin() {
    #if ION_USE_LITTLEFS
    if (LittleFS.begin()) {
        littleFSAvailable = true;
        ION_LOG("LittleFS mounted successfully");
        return true;
    } else {
        ION_LOG_W("LittleFS mount failed, using embedded assets only");
        return false;
    }
    #else
    ION_LOG("LittleFS disabled, using embedded assets only");
    return false;
    #endif
}

void AssetManager::end() {
    #if ION_USE_LITTLEFS
    if (littleFSAvailable) {
        LittleFS.end();
        littleFSAvailable = false;
    }
    #endif
}

String AssetManager::loadHTML() {
    String html = loadFromLittleFS("/ionconnect/index.html");
    if (html.isEmpty()) {
        html = loadFromPROGMEM("html");
    }
    return html;
}

String AssetManager::loadJS() {
    #if ION_MINIMAL_MODE
    // In minimal mode, JS is inline in HTML
    return "";
    #else
    String js = loadFromLittleFS("/ionconnect/script.js");
    if (js.isEmpty()) {
        js = loadFromPROGMEM("js");
    }
    return js;
    #endif
}

bool AssetManager::hasLittleFSOverride(const char* filename) {
    #if ION_USE_LITTLEFS
    if (!littleFSAvailable) return false;
    
    String path = "/ionconnect/";
    path += filename;
    
    return LittleFS.exists(path);
    #else
    return false;
    #endif
}

String AssetManager::loadFile(const char* path) {
    String content = loadFromLittleFS(path);
    if (content.isEmpty()) {
        ION_LOG_W("File not found: %s", path);
    }
    return content;
}

String AssetManager::loadFromLittleFS(const char* path) {
    #if ION_USE_LITTLEFS
    if (!littleFSAvailable) return "";
    
    if (!LittleFS.exists(path)) {
        return "";
    }
    
    File file = LittleFS.open(path, "r");
    if (!file) {
        ION_LOG_W("Failed to open file: %s", path);
        return "";
    }
    
    String content = file.readString();
    file.close();
    
    ION_LOG("Loaded from LittleFS: %s (%d bytes)", path, content.length());
    return content;
    #else
    return "";
    #endif
}

String AssetManager::loadFromPROGMEM(const char* assetType) {
    if (strcmp(assetType, "html") == 0) {
        ION_LOG("Loading embedded HTML");
        return String(FPSTR(EMBEDDED_HTML));
    } 
    #if !ION_MINIMAL_MODE
    else if (strcmp(assetType, "js") == 0) {
        ION_LOG("Loading embedded JS");
        return String(FPSTR(EMBEDDED_JS));
    }
    #endif
    
    return "";
}

} // namespace IonConnect

