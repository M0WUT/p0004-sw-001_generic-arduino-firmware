#ifndef EEPROMHELPER_H
#define EEPROMHELPER_H

#include "eeprom_24aa256uid.h"
#include "DebugUartHelper.h"
#include "ServiceId.h"
#include "EepromStruct.h"

class EepromHelper
{
public:
    EepromHelper(EEPROM24AA256UID eeprom, DebugUartHelper *uartHelper);
    int load_service_data(ServiceId serviceId, uint8_t *eepromStruct, size_t eepromStructSize, int version);

private:
    EEPROM24AA256UID _eeprom;
    DebugUartHelper *_uartHelper;
    char *_printfBuffer;
    const int _printfBufferSize = 128 * sizeof(char);
    void _debug_print_header();
    void _debug_print(const char *str);
    void _debug_printf(const char *fmt, ...);
    void _debug_println(const char *str);
    int read_bytes(uint16_t regAddr, uint8_t *data, int numBytes);
    int write_bytes(uint16_t regAddr, uint8_t *data, int numBytes);

    uint8_t *_fakeEeprom; // Used for debugging to have the EEPROM in RAM, not used normally
};

#endif // EEPROMHELPER_H
