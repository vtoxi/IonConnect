#include "PluginRegistry.h"

#if ION_ENABLE_PLUGINS

#include "../utils/Logger.h"

namespace IonConnect {

PluginRegistry::PluginRegistry() : initialized(false) {
}

PluginRegistry::~PluginRegistry() {
    // Don't delete plugins - they're owned by user code
    plugins.clear();
}

bool PluginRegistry::registerPlugin(IonPlugin* plugin) {
    if (!plugin) {
        ION_LOG_E("Cannot register null plugin");
        return false;
    }
    
    String name = plugin->getName();
    
    if (plugins.find(name) != plugins.end()) {
        ION_LOG_W("Plugin already registered: %s", name.c_str());
        return false;
    }
    
    plugins[name] = plugin;
    ION_LOG("Plugin registered: %s v%s", plugin->getName(), plugin->getVersion());
    
    return true;
}

bool PluginRegistry::unregisterPlugin(const String& name) {
    auto it = plugins.find(name);
    if (it == plugins.end()) {
        return false;
    }
    
    plugins.erase(it);
    ION_LOG("Plugin unregistered: %s", name.c_str());
    
    return true;
}

IonPlugin* PluginRegistry::getPlugin(const String& name) {
    auto it = plugins.find(name);
    if (it != plugins.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<IonPlugin*> PluginRegistry::getAllPlugins() {
    std::vector<IonPlugin*> result;
    for (auto& pair : plugins) {
        result.push_back(pair.second);
    }
    return result;
}

void PluginRegistry::initAll(IonConnectBase* ion) {
    if (initialized) {
        ION_LOG_W("Plugins already initialized");
        return;
    }
    
    ION_LOG("Initializing %d plugins", plugins.size());
    
    for (auto& pair : plugins) {
        IonPlugin* plugin = pair.second;
        ION_LOG("Initializing plugin: %s", plugin->getName());
        
        if (!plugin->init(ion)) {
            ION_LOG_E("Plugin init failed: %s", plugin->getName());
        }
    }
    
    initialized = true;
}

void PluginRegistry::handleAll() {
    for (auto& pair : plugins) {
        pair.second->handle();
    }
}

void PluginRegistry::registerAllRoutes(AsyncWebServer* server) {
    if (!server) return;
    
    ION_LOG("Registering plugin routes");
    
    for (auto& pair : plugins) {
        IonPlugin* plugin = pair.second;
        ION_LOG("Registering routes for: %s", plugin->getName());
        plugin->registerRoutes(server);
    }
}

String PluginRegistry::getAllUIPanels() {
    String panels = "";
    
    for (auto& pair : plugins) {
        String panel = pair.second->getUIPanel();
        if (!panel.isEmpty()) {
            panels += panel;
        }
    }
    
    return panels;
}

void PluginRegistry::notifyConfigChanged(const String& key, const String& value) {
    for (auto& pair : plugins) {
        pair.second->onConfigChanged(key, value);
    }
}

void PluginRegistry::notifyWiFiConnect() {
    for (auto& pair : plugins) {
        pair.second->onWiFiConnect();
    }
}

void PluginRegistry::notifyWiFiDisconnect() {
    for (auto& pair : plugins) {
        pair.second->onWiFiDisconnect();
    }
}

} // namespace IonConnect

#endif // ION_ENABLE_PLUGINS

