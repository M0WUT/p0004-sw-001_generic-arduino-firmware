#include "Service.h"

Service::Service(ServiceId serviceId) : _serviceId(serviceId)
{
    _printfBuffer = (char *)malloc(_printfBufferSize);
}

void Service::initialise_service(ServiceContext *ctx)
{
    _ctx = ctx;
    _debug_println("Initialising...");
}

void Service::tick()
{
    _debug_println("Tick");
}

void Service::_debug_print(const char *str)
{
    _ctx->debugUartHelper->printf("[%s] %s", _serviceId._to_string(), str);
}

void Service::_debug_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    snprintf(_printfBuffer, _printfBufferSize, "[%s] %s", _serviceId._to_string(), fmt);
    _ctx->debugUartHelper->vprintf(_printfBuffer, args);
    va_end(args);
}

void Service::_debug_println(const char *str)
{
    _ctx->debugUartHelper->printf("[%s] %s\n", _serviceId._to_string(), str);
}

int Service::load_eeprom_data()
{

    _ctx->eepromHelper->load_service_data(_serviceId, NULL, 4, 1);
    return 0;
}
