#ifndef SERVICETRACKER_H
#define SERVICETRACKER_H

#include "HelperContext.h"
#include "ServiceId.h"

class BaseService;

class ServiceHelper
{
public:
    ServiceHelper(HelperContext *ctx);
    void register_services(BaseService **serviceList, size_t len);
    BaseService *get_service(ServiceId serviceId);
    void tick();
    BaseService **get_service_list();
    int get_num_services();

private:
    HelperContext *_ctx;
    // Service _coreService = Service(ServiceId::CORE);
    // Service _powerService = Service(ServiceId::POWER);
    // Service _thermalService = Service(ServiceId::THERMAL);
    // Service _lcdService = Service(ServiceId::LCD);
    BaseService **_serviceList; // List containing pointers to all running services
};

#endif // SERVICETRACKER_H
