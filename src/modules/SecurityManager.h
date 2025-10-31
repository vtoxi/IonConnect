#ifndef SECURITY_MANAGER_H
#define SECURITY_MANAGER_H

#include <Arduino.h>
#include <map>
#include "../core/IonTypes.h"

#if ION_PLATFORM_ESP32
    #include <ESPAsyncWebServer.h>
#elif ION_PLATFORM_ESP8266
    #include <ESPAsyncTCP.h>
    #include <ESPAsyncWebServer.h>
#endif

namespace IonConnect {

/**
 * @brief Handles authentication, tokens, and encryption for IonConnect
 */
class SecurityManager {
public:
    SecurityManager();
    
    // Token Management
    void setAccessToken(const String& token);
    bool validateToken(const String& token);
    String generateToken(size_t length = 32);
    String getAccessToken();
    
    // Portal Password
    void setPortalPassword(const String& password);
    bool validatePortalAuth(AsyncWebServerRequest* request);
    bool hasPortalPassword();
    
    // Request Validation
    bool authorizeRequest(AsyncWebServerRequest* request);
    bool requiresAuth();
    
    // Session Management
    String createSession();
    bool validateSession(const String& sessionId);
    void removeSession(const String& sessionId);
    
private:
    String accessToken;
    String portalPassword;
    std::map<String, uint32_t> sessions; // sessionId -> expiry time
    
    bool isPublicEndpoint(const String& url);
    String extractToken(AsyncWebServerRequest* request);
    String extractSessionCookie(AsyncWebServerRequest* request);
    
    static const uint32_t SESSION_TIMEOUT = 3600000; // 1 hour
};

} // namespace IonConnect

#endif // SECURITY_MANAGER_H

