#include "Service.h"
#include "EepromHelper.h"

void BaseService::initialise(ServiceContext *ctx)
{
    _ctx = ctx;
    _info("Initialising...");
    if (!_load_eeprom_config())
    {
        // Data read failed. If this hasn't errored, probably that EEPROM is unitialised
        _info("Creating default config");
        _generate_default_config();
        ConfigData *config = _getConfig();
        _ctx->eepromHelper->write_service_config(_serviceId, config, config->size(), config->version);
    }
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

bool BaseService::_load_eeprom_config()
{
    ConfigData *config = _getConfig();
    _ctx->eepromHelper->read_service_config(_serviceId, config, config->size(), config->version);
    return 0;
}
