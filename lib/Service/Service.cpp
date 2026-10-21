#include "Service.h"
#include "EepromHelper.h"

void BaseService::initialise(ServiceContext *ctx)
{
    _ctx = ctx;
    _info("Initialising...");
    _load_eeprom_data();
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

int BaseService::_load_eeprom_data()
{
    ConfigData *config = _getConfig();
    _ctx->eepromHelper->read_service_config(_serviceId, config, config->size(), config->version);
    return 0;
}
