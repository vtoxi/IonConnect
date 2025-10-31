#ifndef OTA_HANDLER_H
#define OTA_HANDLER_H

#include "../core/IonTypes.h"

#if ION_ENABLE_OTA

#include <Arduino.h>
#include <functional>
#include "SecurityManager.h"

#if ION_PLATFORM_ESP32
    #include <Update.h>
    #include <ESPAsyncWebServer.h>
#elif ION_PLATFORM_ESP8266
    #include <Updater.h>
    #include <ESPAsyncWebServer.h>
#endif

namespace IonConnect {

/**
 * @brief Handles over-the-air firmware updates
 */
class OTAHandler {
public:
    OTAHandler(SecurityManager* security);
    ~OTAHandler();
    
    bool begin(AsyncWebServer* server);
    void setUpdateURL(const String& url);
    
    // Callbacks
    void onStart(std::function<void()> cb);
    void onProgress(std::function<void(size_t current, size_t total)> cb);
    void onEnd(std::function<void()> cb);
    void onError(std::function<void(int error)> cb);
    
private:
    SecurityManager* security;
    String updateURL;
    
    std::function<void()> startCallback;
    std::function<void(size_t, size_t)> progressCallback;
    std::function<void()> endCallback;
    std::function<void(int)> errorCallback;
    
    void setupRoutes(AsyncWebServer* server);
    void handleUpdatePage(AsyncWebServerRequest* request);
    void handleUpdateUpload(AsyncWebServerRequest* request, String filename, 
                           size_t index, uint8_t* data, size_t len, bool final);
    bool validateFirmware(uint8_t* data, size_t len);
    
    size_t updateProgress;
    size_t updateTotal;
    bool updateInProgress;
};

} // namespace IonConnect

#endif // ION_ENABLE_OTA
#endif // OTA_HANDLER_H

