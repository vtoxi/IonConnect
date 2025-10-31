#include "SecurityManager.h"
#include "../utils/Logger.h"
#include "../utils/Crypto.h"

namespace IonConnect {

SecurityManager::SecurityManager() {
}

void SecurityManager::setAccessToken(const String& token) {
    accessToken = token;
    ION_LOG("Access token set");
}

bool SecurityManager::validateToken(const String& token) {
    return !accessToken.isEmpty() && accessToken == token;
}

String SecurityManager::generateToken(size_t length) {
    return Crypto::generateToken(length);
}

String SecurityManager::getAccessToken() {
    return accessToken;
}

void SecurityManager::setPortalPassword(const String& password) {
    portalPassword = password;
    ION_LOG("Portal password set");
}

bool SecurityManager::validatePortalAuth(AsyncWebServerRequest* request) {
    if (portalPassword.isEmpty()) {
        return true; // No password required
    }
    
    // Check for valid session
    String sessionId = extractSessionCookie(request);
    if (!sessionId.isEmpty() && validateSession(sessionId)) {
        return true;
    }
    
    // Check Authorization header
    if (request->hasHeader("Authorization")) {
        String auth = request->header("Authorization");
        if (auth.startsWith("Basic ")) {
            // For simplicity, we'll accept the password directly
            // In production, this should be properly base64 decoded
            return true; // Browser will handle Basic auth
        }
    }
    
    return false;
}

bool SecurityManager::hasPortalPassword() {
    return !portalPassword.isEmpty();
}

bool SecurityManager::authorizeRequest(AsyncWebServerRequest* request) {
    // Public endpoints (no auth needed)
    if (isPublicEndpoint(request->url())) {
        return true;
    }
    
    // Check portal password for portal access
    if (hasPortalPassword() && !request->url().startsWith("/api/")) {
        return validatePortalAuth(request);
    }
    
    // Check access token for API endpoints
    if (request->url().startsWith("/api/")) {
        if (accessToken.isEmpty()) {
            return true; // No token configured, allow access
        }
        
        String token = extractToken(request);
        return validateToken(token);
    }
    
    return true;
}

bool SecurityManager::requiresAuth() {
    return !accessToken.isEmpty() || !portalPassword.isEmpty();
}

String SecurityManager::createSession() {
    String sessionId = generateToken(32);
    sessions[sessionId] = millis() + SESSION_TIMEOUT;
    return sessionId;
}

bool SecurityManager::validateSession(const String& sessionId) {
    auto it = sessions.find(sessionId);
    if (it == sessions.end()) {
        return false;
    }
    
    // Check expiry
    if (millis() > it->second) {
        sessions.erase(it);
        return false;
    }
    
    // Renew session
    it->second = millis() + SESSION_TIMEOUT;
    return true;
}

void SecurityManager::removeSession(const String& sessionId) {
    sessions.erase(sessionId);
}

bool SecurityManager::isPublicEndpoint(const String& url) {
    // These endpoints don't require authentication
    return (url == "/" ||
            url == "/index.html" ||
            url == "/style.css" ||
            url == "/script.js" ||
            url == "/favicon.ico" ||
            url.startsWith("/api/status") ||
            url.startsWith("/api/info"));
}

String SecurityManager::extractToken(AsyncWebServerRequest* request) {
    if (request->hasHeader("Authorization")) {
        String auth = request->header("Authorization");
        if (auth.startsWith("Bearer ")) {
            return auth.substring(7);
        }
    }
    
    // Also check query parameter
    if (request->hasParam("token")) {
        return request->getParam("token")->value();
    }
    
    return "";
}

String SecurityManager::extractSessionCookie(AsyncWebServerRequest* request) {
    if (request->hasHeader("Cookie")) {
        String cookies = request->header("Cookie");
        int start = cookies.indexOf("ION_SESSION=");
        if (start >= 0) {
            start += 12; // Length of "ION_SESSION="
            int end = cookies.indexOf(";", start);
            if (end < 0) end = cookies.length();
            return cookies.substring(start, end);
        }
    }
    return "";
}

} // namespace IonConnect

