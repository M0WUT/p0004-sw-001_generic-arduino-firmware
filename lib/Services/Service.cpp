#include "Service.h"

Service::Service(ServiceID serviceID) : _serviceID(serviceID)
{
    _printfBuffer = (char *)malloc(_printfBufferSize);
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
    va_list args;
    va_start(args, fmt);
    snprintf(_printfBuffer, _printfBufferSize, "[%s] %s", _serviceID._to_string(), fmt);
    _ctx->debugUartHelper->printf(_printfBuffer, args);
    va_end(args);
}

void Service::_debug_println(const char *str)
{
    _ctx->debugUartHelper->printf("[%s] %s\n", _serviceID._to_string(), str);
}
