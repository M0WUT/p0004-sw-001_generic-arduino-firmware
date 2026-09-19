#ifndef SERVICETRACKER_H
#define SERVICETRACKER_H

#include "Service.h"
#include "ServiceContext.h"
#include "EthernetService.h"

class ServiceTracker
{
public:
    ServiceTracker(Service *extraServiceList, size_t len);
    void initialiseServices(ServiceContext *ctx);

private:
    // Service _coreService = Service(ServiceId::CORE);
    // Service _powerService = Service(ServiceId::POWER);
    // Service _thermalService = Service(ServiceId::THERMAL);
    // Service _lcdService = Service(ServiceId::LCD);
    EthernetService _ethernetService = EthernetService();
    Service **_serviceList; // List containing pointers to all running services
    int _numServices;       // We'll iterate over the _serviceList many times. Save length to avoid recalculations
};

#endif // SERVICETRACKER_H
