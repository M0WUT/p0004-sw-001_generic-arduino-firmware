
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

struct EthernetServiceConfigData
{
    uint8_t macAddress[6];
    bool useDhcp;
    uint8_t staticIpAddress[4];
    uint8_t serverPort;
    uint8_t maxClients;
};

class EthernetServiceConfig : public ConfigTemplate<EthernetServiceConfigData>
{
public:
    EthernetServiceConfig() : ConfigTemplate<EthernetServiceConfigData>(1, {}) {}
    size_t size() { return sizeof(EthernetServiceConfigData); }
};

#endif // CONFIGDATA_H
