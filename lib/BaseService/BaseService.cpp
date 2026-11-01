#include "BaseService.h"
#include "ServiceContext.h"
#include "ServiceHelper.h"
#include "LoggingHelper.h"
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
        ConfigData *config = _get_config();
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

const char *BaseService::get_name()
{
    return _serviceId._to_string();
}

void BaseService::tick()
{
    _debug("Tick");
}

ServiceId BaseService::getServiceId()
{
    return _serviceId;
}

bool BaseService::_load_eeprom_config()
{
    ConfigData *config = _get_config();
    _ctx->eepromHelper->read_service_config(_serviceId, config, config->size(), config->version);
    return 0;
}

bool BaseService::parse_new_config_value(
    const ConfigField &f, // Field to be updated
    const char *text,     // String version of new value
    ConfigValue &v,       // Return: value having been processed and assigned the correct type
    char *error,          // Return: Error message if the supplied data was invalid
    size_t errorSize      // Size of buffer for error message
)
// Returns true if the new value passes validation rules and is now in v
// Otherwise returns false and issue can be found in "error"
{
    if (!text)
    {
        snprintf(error, errorSize, "Missing value");
        return false;
    }
    v = {};
    v.type = f.type;
    if (f.type == ConfigFieldType::Integer)
    {
        if (!*text)
        {
            snprintf(error, errorSize, "Integer required");
            return false;
        }
        char *end = nullptr;
        errno = 0;
        int32_t n = strtoll(text, &end, 10);
        if (end == text || *end || errno == ERANGE)
        {
            snprintf(error, errorSize, "Invalid integer");
            return false;
        }
        int32_t x = (int32_t)n;
        if (f.hasRange)
        {
            if (x < f.intMin || x > f.intMax)
            {
                snprintf(error, errorSize, "Value must be %" PRId64 " to %" PRId64, f.intMin, f.intMax);
                return false;
            }
            uint32_t offset = (uint32_t)x - (uint32_t)f.intMin;
            if (f.intStep <= 0 || offset % (uint32_t)f.intStep)
            {
                snprintf(error, errorSize, "Step must be %" PRId64, f.intStep);
                return false;
            }
        }
        v.integer = x;
        return true;
    }
    if (f.type == ConfigFieldType::Float)
    {
        if (!*text)
        {
            snprintf(error, errorSize, "Number required");
            return false;
        }
        char *end = nullptr;
        errno = 0;
        double x = strtod(text, &end);
        if (end == text || *end || errno == ERANGE || !isfinite(x))
        {
            snprintf(error, errorSize, "Invalid number");
            return false;
        }
        if (f.hasRange)
        {
            if (x < f.floatMin || x > f.floatMax)
            {
                snprintf(error, errorSize, "Value must be %.6g to %.6g", f.floatMin, f.floatMax);
                return false;
            }
            double steps = (x - f.floatMin) / f.floatStep;
            if (f.floatStep <= 0 || !isfinite(steps) ||
                fabs(steps - round(steps)) > 1e-7 * fmax(1.0, fabs(steps)))
            {
                snprintf(error, errorSize, "Step must be %.6g", f.floatStep);
                return false;
            }
        }
        v.number = x;
        return true;
    }
    if (f.type == ConfigFieldType::Boolean)
    {
        if (!strcmp(text, "true"))
        {
            v.boolean = true;
            return true;
        }
        if (!strcmp(text, "false"))
        {
            v.boolean = false;
            return true;
        }
        snprintf(error, errorSize, "Invalid boolean");
        return false;
    }
    if (strlen(text) >= DISPLAY_VALUE_BUFFER_SIZE)
    {
        snprintf(error, errorSize, "Text too long");
        return false;
    }
    v.text = text;
    return true;
}

bool BaseService::apply_config_update(const char *id, const char *requested,
                                      char *message, size_t messageSize,
                                      char *accepted, size_t acceptedSize)
{
    if (!message || !accepted || !messageSize || !acceptedSize)
        return false;
    message[0] = accepted[0] = 0;
    // @TODO sanity check service Id and conversion thereof
    // if (!services || index >= count || !services[index] || !id || !requested)
    // {
    //     snprintf(message, messageSize, "Invalid service or parameter");
    //     return false;
    // }

    ConfigFieldList fields;
    get_config_fields(fields);
    const ConfigField *f = fields.find(id);
    if (!f)
    {
        snprintf(message, messageSize, "Unknown parameter");
        return false;
    }
    ConfigValue v;
    if (!BaseService::parse_new_config_value(*f, requested, v, message, messageSize))
        return false;
    if (!validate_config_field(id, v, message, messageSize))
    {
        if (!message[0])
            snprintf(message, messageSize, "Change refused");
        return false;
    }
    if (!update_config_field(id, v, message, messageSize))
    {
        if (!message[0])
            snprintf(message, messageSize, "Unable to apply change");
        return false;
    }
    ConfigFieldList updated;
    get_config_fields(updated);
    const ConfigField *stored = updated.find(id);
    if (!stored || strlen(stored->displayValue) >= acceptedSize)
    {
        snprintf(message, messageSize, "Applied, but readback unavailable");
        return false;
    }
    strcpy(accepted, stored->displayValue);
    return true;
}