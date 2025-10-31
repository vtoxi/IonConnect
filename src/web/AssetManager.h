#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <Arduino.h>
#include "../core/IonTypes.h"

namespace IonConnect {

/**
 * @brief Manages web assets with hybrid LittleFS/PROGMEM delivery
 * 
 * Attempts to load assets from LittleFS first, falls back to embedded PROGMEM
 */
class AssetManager {
public:
    AssetManager();
    
    bool begin();
    void end();
    
    // Asset loading
    String loadHTML();
    String loadJS();
    bool hasLittleFSOverride(const char* filename);
    String loadFile(const char* path);
    
private:
    bool littleFSAvailable;
    
    String loadFromLittleFS(const char* path);
    String loadFromPROGMEM(const char* assetType);
};

} // namespace IonConnect

#endif // ASSET_MANAGER_H

