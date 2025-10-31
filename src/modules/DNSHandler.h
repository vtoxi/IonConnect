#ifndef DNS_HANDLER_H
#define DNS_HANDLER_H

#include <Arduino.h>
#include <DNSServer.h>
#include <IPAddress.h>

namespace IonConnect {

/**
 * @brief Handles DNS redirection for captive portal
 * 
 * Redirects all DNS requests to the portal IP address
 */
class DNSHandler {
public:
    DNSHandler();
    ~DNSHandler();
    
    bool start(IPAddress portalIP, uint16_t port = 53);
    void stop();
    void handle(); // Call in loop()
    bool isRunning();
    
private:
    DNSServer dnsServer;
    bool running;
    IPAddress currentIP;
};

} // namespace IonConnect

#endif // DNS_HANDLER_H

