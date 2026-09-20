
#ifndef EEPROMSTRUCT_H
#define EEPROMSTRUCT_H

#include <Arduino.h>

class EepromStruct
{
public:
    virtual ~EepromStruct() = default;
};

template <typename T>
class EepromStructTemplate : public EepromStruct
{
public:
    EepromStructTemplate(T data) : data(std::move(data)) {}

protected:
    T data;
};

struct EthernetServiceEepromStruct
{
    uint8_t macAddress[6];
    bool useDhcp;
    uint8_t staticIpAddress[4];
    uint8_t maxClients;
};

class EthernetServiceEeprom : public EepromStructTemplate<EthernetServiceEepromStruct>
{
    using EepromStructTemplate<EthernetServiceEepromStruct>::EepromStructTemplate;
};

#endif // EEPROMSTRUCT_H
