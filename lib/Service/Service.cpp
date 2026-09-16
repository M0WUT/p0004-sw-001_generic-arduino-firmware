#include "Service.h"

Service::Service(ServiceID serviceID) : _serviceID(serviceID)
{
}

void Service::initialise(ServiceContext *ctx)
{
    _ctx = ctx;
    _debug_println("Initialised");
}

void Service::tick()
{
    _debug_println("Tick");
}

void Service::_debug_print(const char *str)
{
    _ctx->debugUartHelper->printf("[%s] %s", _serviceID._to_string(), str);
}

void Service::_debug_printf(const char *fmt, ...)
{
    char format[128];

    va_list args;
    va_start(args, fmt);
    snprintf(format, sizeof(format), "[%s] %s", _serviceID._to_string(), fmt);
    _ctx->debugUartHelper->printf(format, args);
    va_end(args);
}

void Service::_debug_println(const char *str)
{
    _ctx->debugUartHelper->printf("[%s] %s\n", _serviceID._to_string(), str);
}
