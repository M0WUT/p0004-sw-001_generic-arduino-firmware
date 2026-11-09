#pragma once

#include <Arduino.h>

static constexpr int MAX_STATUS_FIELD_LENGTH = 32;

struct StatusField
{
    char label[MAX_STATUS_FIELD_LENGTH];
    char value[MAX_STATUS_FIELD_LENGTH];
};

class StatusFieldList
{
public:
    static constexpr size_t MAX_FIELDS = 16;

    StatusField fields[MAX_FIELDS];
    size_t count = 0;

    void add(const char *label, const char *value)
    {
        if (count >= MAX_FIELDS)
            return;

        StatusField &field = fields[count++];

        snprintf(field.label, sizeof(field.label), "%s", label ? label : "");
        snprintf(field.value, sizeof(field.value), "%s", value ? value : "");
    }
};