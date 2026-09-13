#include "service.h"

Service::Service(ServiceID serviceID) : _serviceID(serviceID)
{
}

void Service::initialise(ServiceContext *ctx)
{
    _ctx = ctx;
    _debug_println("Initialised");
}

void Service::_debug_print(const char *str)
{
    DEBUG_PRINTF("[%s] %s", _serviceID._to_string(), str);
}

void Service::_debug_printf(const char *fmt, ...)
{
    char format[128];

    va_list args;
    va_start(args, fmt);
    snprintf(format, sizeof(format), "[%s] %s", _serviceID._to_string(), fmt);
    DEBUG_PRINTF(format, args);
    va_end(args);
}

void Service::_debug_println(const char *str)
{
    DEBUG_PRINTF("[%s] %s\n", _serviceID._to_string(), str);
}

void Service::_debug_print_service_id()
{
    DEBUG_PRINTF("[%s] ", _serviceID._to_string());
}

ServiceTracker::ServiceTracker(Service *extraServiceList, size_t len)
{
    // This list of pointers will get destroyed after this function
    // But the pointers are still valid as the objects will still exist
    Service *defaultServiceList[] = {&_coreService, &_powerService, &_thermalService, &_lcdService, &_ethernetService};

    int numDefaultServices = sizeof(defaultServiceList) / sizeof(defaultServiceList[0]);
    _numServices = len + numDefaultServices;

    _serviceList = (Service **)malloc((sizeof(Service *) * _numServices));

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
