#ifndef SERVICETRACKER_H
#define SERVICETRACKER_H

#include "services/Service/Service.h"
#include "services/EthernetService/EthernetService.h"

class ServiceTracker
{
public:
    ServiceTracker(Service *extraServiceList, size_t len);
    void initialiseServices(ServiceContext *ctx);

private:
    Service _coreService = Service(ServiceID::CORE);
    Service _powerService = Service(ServiceID::POWER);
    Service _thermalService = Service(ServiceID::THERMAL);
    Service _lcdService = Service(ServiceID::LCD);
    EthernetService _ethernetService = EthernetService();
    Service **_serviceList; // List containing pointers to all running services
    int _numServices;       // We'll iterate over the _serviceList many times. Save length to avoid recalculations
};

#endif // SERVICETRACKER_H
