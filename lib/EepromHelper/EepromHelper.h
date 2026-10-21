#ifndef EEPROMHELPER_H
#define EEPROMHELPER_H

#include "eeprom_24aa256uid.h"
#include "ServiceId.h"
#include "HelperContext.h"
#include "BaseHelper.h"
#include <CRC8.h>

class EepromHelper : public BaseHelper
{
public:
    EepromHelper(EEPROM24AA256UID eeprom, HelperContext *ctx);
    int read_service_config(ServiceId serviceId, void *eepromStruct, size_t eepromStructSize, int version);
    int write_service_config(ServiceId serviceId, void *eepromStruct, size_t eepromStructSize, int version);
    void hexdump(uint16_t start_addr, uint16_t size);

private:
    EEPROM24AA256UID _eeprom;
    int read_bytes(uint16_t regAddr, uint8_t *data, int numBytes, CRC8 *crc = NULL);
    int write_bytes(uint16_t regAddr, uint8_t *data, int numBytes, CRC8 *crc = NULL);
    bool _eeprom_initialised();
    void initialise_eeprom();
    void read_eat_table();
    uint16_t _serviceBaseAddresses[NUM_KNOWN_SERVICES];
    const char _initValues[3] = {'L', 'I', 'D'};
#ifdef DEBUG_EEPROM
    uint8_t *_fakeEeprom; // Used for debugging to have the EEPROM in RAM, not used normally
#endif
};

#endif // EEPROMHELPER_H
