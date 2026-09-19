
#ifndef EEPROMSTRUCT_H
#define EEPROMSTRUCT_H

#include <Arduino.h>

template <typename T>
class EepromStructBase
{
public:
    T data;
    virtual ~EepromStructBase() = default;
};

struct EthernetServiceEepromStruct
{
    uint8_t macAddress[6];
    bool useDhcp;
    uint8_t staticIpAddress[4];
    uint8_t maxClients;
};

class EthernetServiceEeprom : EepromStructBase<EthernetServiceEepromStruct>
{
    ;
};

#endif // EEPROMSTRUCT_H
