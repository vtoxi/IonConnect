#ifndef PLUGIN_REGISTRY_H
#define PLUGIN_REGISTRY_H

#include "../core/IonTypes.h"

#if ION_ENABLE_PLUGINS

#include <vector>
#include <map>
#include "../plugins/IonPlugin.h"

#if ION_PLATFORM_ESP32
    #include <ESPAsyncWebServer.h>
#elif ION_PLATFORM_ESP8266
    #include <ESPAsyncWebServer.h>
#endif

namespace IonConnect {

// Forward declaration
class IonConnectBase;

/**
 * @brief Manages plugin registration and lifecycle
 */
class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    bool registerPlugin(IonPlugin* plugin);
    bool unregisterPlugin(const String& name);
    IonPlugin* getPlugin(const String& name);
    std::vector<IonPlugin*> getAllPlugins();
    
    void initAll(IonConnectBase* ion);
    void handleAll();
    void registerAllRoutes(AsyncWebServer* server);
    String getAllUIPanels();
    
    void notifyConfigChanged(const String& key, const String& value);
    void notifyWiFiConnect();
    void notifyWiFiDisconnect();
    
private:
    std::map<String, IonPlugin*> plugins;
    bool initialized;
};

} // namespace IonConnect

#endif // ION_ENABLE_PLUGINS
#endif // PLUGIN_REGISTRY_H

