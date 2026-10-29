#ifndef SERVICE_H
#define SERVICE_H

#include <Arduino.h>
#include <variant>
#include "ServiceId.h"
#include "ServiceContext.h"
#include "ConfigData.h"
#include "NamedTask.h"

class BaseService : public NamedTask
{
public:
    BaseService(ServiceId serviceId) : _serviceId(serviceId) {}
    virtual void initialise(ServiceContext *ctx) = 0;
    virtual void tick();
    ServiceId getServiceId();

protected:
    ServiceId _serviceId;
    ServiceContext *_ctx;

    void _initialise(ServiceContext *ctx);
    virtual ConfigData *_getConfig() = 0;
    bool _load_eeprom_config();
    virtual void _generate_default_config() = 0;
    virtual void _print_config() = 0;

private:
    LoggingHelper *_getLoggingHelper();
    const char *_getName();
};

template <typename TConfig>
class Service : public BaseService
{
public:
    Service(ServiceId serviceId, TConfig config) : BaseService(serviceId), _config(config) {}
    TConfig _config;
};

#endif // SERVICE_H
