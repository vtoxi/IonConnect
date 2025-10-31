#include "DNSHandler.h"
#include "../utils/Logger.h"

namespace IonConnect {

DNSHandler::DNSHandler() : running(false) {
}

DNSHandler::~DNSHandler() {
    stop();
}

bool DNSHandler::start(IPAddress portalIP, uint16_t port) {
    if (running) {
        ION_LOG_W("DNS handler already running");
        return true;
    }
    
    currentIP = portalIP;
    
    // Start DNS server - redirect all requests to portal IP
    if (!dnsServer.start(port, "*", portalIP)) {
        ION_LOG_E("Failed to start DNS server");
        return false;
    }
    
    running = true;
    ION_LOG("DNS handler started on port %d -> %s", port, portalIP.toString().c_str());
    
    return true;
}

void DNSHandler::stop() {
    if (running) {
        dnsServer.stop();
        running = false;
        ION_LOG("DNS handler stopped");
    }
}

void DNSHandler::handle() {
    if (running) {
        dnsServer.processNextRequest();
    }
}

bool DNSHandler::isRunning() {
    return running;
}

} // namespace IonConnect

