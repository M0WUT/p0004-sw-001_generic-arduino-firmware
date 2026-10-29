#ifndef SERVICETRACKER_H
#define SERVICETRACKER_H

#include "BaseService.h"
#include "ServiceContext.h"
#include "EthernetService.h"
#include "HelperContext.h"

class ServiceHelper
{
public:
    ServiceHelper(BaseService **extraServiceList, size_t len, HelperContext *ctx);
    void initialiseServices(ServiceContext *ctx);
    BaseService *getService(ServiceId serviceId);
    void tick();

private:
    HelperContext *_ctx;
    // Service _coreService = Service(ServiceId::CORE);
    // Service _powerService = Service(ServiceId::POWER);
    // Service _thermalService = Service(ServiceId::THERMAL);
    // Service _lcdService = Service(ServiceId::LCD);
    EthernetService _ethernetService = EthernetService();
    BaseService **_serviceList; // List containing pointers to all running services
};

#endif // SERVICETRACKER_H
