
#pragma once

#include <Arduino.h>

constexpr size_t MAX_CONFIG_FIELDS = 16;
constexpr size_t FIELD_ID_BUFFER_SIZE = 32;
constexpr size_t FIELD_NAME_BUFFER_SIZE = 48;
constexpr size_t DISPLAY_VALUE_BUFFER_SIZE = 48;

// Deals specifically with data struct and associated things needed for EEPROM read/write
class ConfigData
{
public:
    ConfigData(int version) : version(version) {}
    int version;
    virtual size_t size() = 0;
};

template <typename T>
class ConfigTemplate : public ConfigData
{
public:
    ConfigTemplate(int version, T data) : ConfigData(version), data(std::move(data)) {}
    T data;
};

// Used to indicate type of data stored in each config field
enum class ConfigFieldType : uint8_t
{
    Text,
    Integer,
    Float,
    Boolean
};

struct ConfigValue
{
    ConfigFieldType type = ConfigFieldType::Text;
    int32_t integer = 0;
    double number = 0;
    bool boolean = false;
    const char *text = nullptr; // Borrowed from HTTP request buffer.
};

// Struct that holds all the auxiliary stuff about an item in config data
// but not the dat itself
struct ConfigField
{
    char id[FIELD_ID_BUFFER_SIZE] = {};
    char label[FIELD_NAME_BUFFER_SIZE] = {};
    char displayValue[DISPLAY_VALUE_BUFFER_SIZE] = {};
    ConfigFieldType type = ConfigFieldType::Text;
    int32_t intMin = 0, intMax = 0, intStep = 1;
    double floatMin = 0, floatMax = 0, floatStep = 1;
    bool hasRange = false;
};

// Class for containing all of the ConfigFields for a Service
// allows for iteration over them all and
struct ConfigFieldList
{
    ConfigField fields[MAX_CONFIG_FIELDS];
    size_t count = 0;
    ConfigField *add(const char *id, const char *label, ConfigFieldType type, const char *value)
    {
        if (!id || !label || !value || count >= MAX_CONFIG_FIELDS ||
            strlen(id) >= FIELD_ID_BUFFER_SIZE || strlen(label) >= FIELD_NAME_BUFFER_SIZE ||
            strlen(value) >= DISPLAY_VALUE_BUFFER_SIZE)
            return nullptr;
        ConfigField &f = fields[count++];
        snprintf(f.id, sizeof(f.id), "%s", id);
        snprintf(f.label, sizeof(f.label), "%s", label);
        snprintf(f.displayValue, sizeof(f.displayValue), "%s", value);
        f.type = type;
        return &f;
    }
    bool addText(const char *id, const char *label, const char *value)
    {
        return add(id, label, ConfigFieldType::Text, value) != nullptr;
    }
    bool addInteger(const char *id, const char *label, int32_t value,
                    int32_t minimum, int32_t maximum, int32_t step = 1)
    {
        if (minimum > maximum || step <= 0)
            return false;
        char tmp[DISPLAY_VALUE_BUFFER_SIZE];
        snprintf(tmp, sizeof(tmp), "%" PRId32, value);
        ConfigField *f = add(id, label, ConfigFieldType::Integer, tmp);
        if (!f)
            return false;
        f->intMin = minimum;
        f->intMax = maximum;
        f->intStep = step;
        f->hasRange = true;
        return true;
    }
    bool addFloat(const char *id, const char *label, double value,
                  double minimum, double maximum, double step, unsigned decimals = 2)
    {
        if (!isfinite(value) || !isfinite(minimum) || !isfinite(maximum) ||
            !isfinite(step) || minimum > maximum || step <= 0 || decimals > 8)
            return false;
        char tmp[DISPLAY_VALUE_BUFFER_SIZE];
        int n = snprintf(tmp, sizeof(tmp), "%.*f", (int)decimals, value);
        if (n < 0 || (size_t)n >= sizeof(tmp))
            return false;
        ConfigField *f = add(id, label, ConfigFieldType::Float, tmp);
        if (!f)
            return false;
        f->floatMin = minimum;
        f->floatMax = maximum;
        f->floatStep = step;
        f->hasRange = true;
        return true;
    }
    bool addBoolean(const char *id, const char *label, bool value)
    {
        return add(id, label, ConfigFieldType::Boolean, value ? "true" : "false") != nullptr;
    }
    const ConfigField *find(const char *id) const
    {
        if (!id)
            return nullptr;
        for (size_t i = 0; i < count; ++i)
            if (strcmp(fields[i].id, id) == 0)
                return &fields[i];
        return nullptr;
    }
};
