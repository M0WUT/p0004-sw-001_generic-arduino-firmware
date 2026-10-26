
#ifndef CONFIGDATA_H
#define CONFIGDATA_H

#include <Arduino.h>

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

#endif // CONFIGDATA_H
