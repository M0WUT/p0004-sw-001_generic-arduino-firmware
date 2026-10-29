#include "ServiceHelper.h"

ServiceHelper::ServiceHelper(BaseService **extraServiceList, size_t len, HelperContext *ctx) : _ctx(ctx)
{
    // This list of pointers will get destroyed after this function
    // But the pointers are still valid as the objects will still exist
    BaseService *defaultServiceList[] = {(BaseService *)&_ethernetService};
    //{&_coreService, &_powerService, &_thermalService, &_lcdService, &_ethernetService};

    _serviceList = (BaseService **)malloc((sizeof(BaseService *) * ServiceId::_size()));
    for (int i = 0; i < ServiceId::_size(); i++)
    {
        _serviceList[i] = NULL;
    }

    // Default services
    int numDefaultServices = sizeof(defaultServiceList) / sizeof(defaultServiceList[0]);
    for (int i = 0; i < numDefaultServices; i++)
    {
        ServiceId id = defaultServiceList[i]->getServiceId();
        int index = id._to_integral();
        if (_serviceList[index] != NULL)
        {
            _ctx->loggingHelper->error("Service Helper", "Attempted to create multiple instances of the same service");
        }
        _serviceList[index] = defaultServiceList[i];
    }

    // Extra Services
    for (int i = 0; i < len; i++)
    {
        ServiceId id = extraServiceList[i]->getServiceId();
        int index = id._to_integral();
        if (_serviceList[index] != NULL)
        {
            _ctx->loggingHelper->error("Service Helper", "Attempted to create multiple instances of the same service");
        }
        _serviceList[index] = extraServiceList[i];
    }
}

void ServiceHelper::initialiseServices(ServiceContext *ctx)
{
    for (int i = 0; i < ServiceId::_size(); i++)
    {
        if (_serviceList[i] != NULL)
            _serviceList[i]->initialise(ctx);
    }
}

BaseService *ServiceHelper::getService(ServiceId serviceId)
{
    int index = serviceId._to_integral();
    BaseService *ret = _serviceList[index];
    if (ret == NULL)
    {
        _ctx->loggingHelper->error("Service Helper", "Requested information on non-existent service");
    }
    return ret;
}

void ServiceHelper::tick()
{
    for (int i = 0; i < ServiceId::_size(); i++)
    {
        if (_serviceList[i] != NULL)
            _serviceList[i]->tick();
    }
}
