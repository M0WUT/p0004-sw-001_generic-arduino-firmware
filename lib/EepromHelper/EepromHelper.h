#ifndef EEPROMHELPER_H
#define EEPROMHELPER_H

#include "eeprom_24aa256uid.h"
#include "ServiceId.h"
#include "EepromStruct.h"
#include "HelperContext.h"
#include "BaseHelper.h"

class EepromHelper : public BaseHelper
{
public:
    EepromHelper(EEPROM24AA256UID eeprom, HelperContext *ctx);
    int load_service_data(ServiceId serviceId, uint8_t *eepromStruct, size_t eepromStructSize, int version);
    void hexdump(uint16_t start_addr, uint16_t size);

private:
    EEPROM24AA256UID _eeprom;
    int read_bytes(uint16_t regAddr, uint8_t *data, int numBytes);
    int write_bytes(uint16_t regAddr, uint8_t *data, int numBytes);
#ifdef DEBUG_EEPROM
    uint8_t *_fakeEeprom; // Used for debugging to have the EEPROM in RAM, not used normally
#endif
};

#endif // EEPROMHELPER_H
