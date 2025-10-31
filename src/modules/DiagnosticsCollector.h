#ifndef DIAGNOSTICS_COLLECTOR_H
#define DIAGNOSTICS_COLLECTOR_H

#include "../core/IonTypes.h"

#if ION_ENABLE_DIAGNOSTICS

namespace IonConnect {

/**
 * @brief Collects and exposes system health metrics
 */
class DiagnosticsCollector {
public:
    DiagnosticsCollector();
    
    void collect(); // Call periodically
    DiagnosticsData getData();
    String getJSON();
    
    void incrementCounter(const char* counter);
    void recordError(const String& error);
    
private:
    DiagnosticsData data;
    uint32_t lastCollectTime;
    
    void collectMemoryStats();
    void collectWiFiStats();
    void collectSystemStats();
};

} // namespace IonConnect

#endif // ION_ENABLE_DIAGNOSTICS
#endif // DIAGNOSTICS_COLLECTOR_H

