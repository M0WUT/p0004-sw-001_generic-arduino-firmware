#include "BaseService.h"

BaseService::BaseService(ServiceId serviceId) : _serviceId(serviceId)
{
    ;
}

void BaseService::initialise_service(ServiceContext *ctx)
{
    _ctx = ctx;
    _info("Initialising...");
}

LoggingHelper *BaseService::_getLoggingHelper()
{
    return _ctx->loggingHelper;
}

const char *BaseService::_getName()
{
    return _serviceId._to_string();
}

void BaseService::tick()
{
    _debug("Tick");
}

int BaseService::load_eeprom_data()
{
    _ctx->eepromHelper->load_service_data(_serviceId, NULL, 4, 1);
    return 0;
}
