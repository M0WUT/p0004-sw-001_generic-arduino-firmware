#include "ServiceTracker.h"

ServiceTracker::ServiceTracker(BaseService *extraServiceList, size_t len)
{
    // This list of pointers will get destroyed after this function
    // But the pointers are still valid as the objects will still exist
    BaseService *defaultServiceList[] = {(BaseService *)&_ethernetService};
    //{&_coreService, &_powerService, &_thermalService, &_lcdService, &_ethernetService};

    int numDefaultServices = sizeof(defaultServiceList) / sizeof(defaultServiceList[0]);
    _numServices = len + numDefaultServices;

    _serviceList = (BaseService **)malloc((sizeof(BaseService *) * _numServices));

    for (int i = 0; i < numDefaultServices; i++)
    {
        _serviceList[i] = defaultServiceList[i];
    }

    for (int i = numDefaultServices; i < _numServices; i++)
    {
        _serviceList[i] = &extraServiceList[i - numDefaultServices];
    }
}

void ServiceTracker::initialiseServices(ServiceContext *ctx)
{
    for (int i = 0; i < _numServices; i++)
    {
        _serviceList[i]->initialise(ctx);
    }
}