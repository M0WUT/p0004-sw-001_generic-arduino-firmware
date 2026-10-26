#include "BaseService.h"
#include "EepromHelper.h"

void BaseService::_initialise(ServiceContext *ctx)
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

        // Once written load back - may seem unneccessary but ensures data matches and routines that
        // apply configuration can be called from a single place
        _load_eeprom_config();
    }
    _print_config();
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
