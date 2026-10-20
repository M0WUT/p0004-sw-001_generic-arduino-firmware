#ifndef SERVICETRACKER_H
#define SERVICETRACKER_H

#include "BaseService.h"
#include "ServiceContext.h"
#include "EthernetService.h"

class ServiceTracker
{
public:
    ServiceTracker(BaseService *extraServiceList, size_t len);
    void initialiseServices(ServiceContext *ctx);

private:
    // Service _coreService = Service(ServiceId::CORE);
    // Service _powerService = Service(ServiceId::POWER);
    // Service _thermalService = Service(ServiceId::THERMAL);
    // Service _lcdService = Service(ServiceId::LCD);
    EthernetService _ethernetService = EthernetService();
    BaseService **_serviceList; // List containing pointers to all running services
    int _numServices;           // We'll iterate over the _serviceList many times. Save length to avoid recalculations
};

#endif // SERVICETRACKER_H
