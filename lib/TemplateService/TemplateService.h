#pragma once

#include "BaseService.h"
#include "ServiceContext.h"

struct TemplateServiceConfigData
{
    // Populate with config data
    uint8_t foo;
};

class TemplateServiceConfig : public ConfigTemplate<TemplateServiceConfigData>
{
public:
    TemplateServiceConfig() : ConfigTemplate<TemplateServiceConfigData>(1, {}) {}
    size_t size() { return sizeof(TemplateServiceConfigData); }
};

class TemplateService : public Service<TemplateServiceConfig>
{
public:
    // Constructor
    TemplateService();

    // Required overrides from BaseService
    void initialise(ServiceContext *ctx);
    // Stuff that should happen every time around the loop()
    // Keep it quick and don't be blocking waiting for something
    // display updates also happen in this loop
    void tick();
    // Use fields.addX functions to add config info for reporting
    // id (first argument) should be a unique name used to identify this variable behind the scenes
    // label (second argument) is human-readable string for printing
    void get_config_fields(ConfigFieldList &fields) const;
    // id will contain the id string defined in get_config_fields
    // Validate (but don't yet update) that happy to accept the new value (in value) and return true
    // Otherwise, return false and put error message in error
    bool validate_config_field(const char *id, const ConfigValue &value, char *error, size_t errorSize);
    // id and value should have been previously validated in valid_config_field
    // so just accept the new value and write to EEPROM
    bool update_config_field(const char *id, const ConfigValue &value,
                             char *error, size_t errorSize);

private:
    // Required overrides from BaseService
    ConfigData *_get_config() { return &_config; }
    const char *get_name() override { return "Template"; }
    // Populate config.data with default values
    void _generate_default_config();
};
