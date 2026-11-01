#ifndef SERVICE_H
#define SERVICE_H

#include <Arduino.h>
#include "ServiceContext.h"
#include "ServiceId.h"
#include "ConfigData.h"
#include "NamedTask.h"

class BaseService : public NamedTask
{
public:
    BaseService(ServiceId serviceId) : _serviceId(serviceId) {}
    virtual void initialise(ServiceContext *ctx) = 0;
    virtual void tick();
    ServiceId getServiceId();
    const char *get_name();
    virtual void get_config_fields(ConfigFieldList &fields) const = 0;
    virtual bool validate_config_field(const char *, const ConfigValue &, char *, size_t) = 0;
    virtual bool update_config_field(const char *id, const ConfigValue &value,
                                     char *error, size_t errorSize) = 0;
    static bool parse_new_config_value(const ConfigField &f, const char *text, ConfigValue &v,
                                       char *error, size_t errorSize);
    bool apply_config_update(const char *id, const char *requested,
                             char *message, size_t messageSize,
                             char *accepted, size_t acceptedSize);

protected:
    ServiceId _serviceId;
    ServiceContext *_ctx;

    void _initialise(ServiceContext *ctx);
    bool _load_eeprom_config();
    virtual ConfigData *_get_config() = 0;
    virtual void _generate_default_config() = 0;
    virtual void _print_config() = 0;

private:
    LoggingHelper *_getLoggingHelper();
};

template <typename TConfig>
class Service : public BaseService
{
public:
    Service(ServiceId serviceId, TConfig config) : BaseService(serviceId), _config(config) {}
    TConfig _config;
};

#endif // SERVICE_H
