#ifndef EEPROMHELPER_H
#define EEPROMHELPER_H

#include "eeprom_24aa256uid.h"
#include "DebugUartHelper.h"

class EepromHelper
{
public:
    EepromHelper(EEPROM24AA256UID eeprom, DebugUartHelper *uartHelper);

private:
    EEPROM24AA256UID _eeprom;
    DebugUartHelper *_uartHelper;
    char *_printfBuffer;
    const int _printfBufferSize = 128 * sizeof(char);
    void _debug_print_header();
    void _debug_print(const char *str);
    void _debug_printf(const char *fmt, ...);
    void _debug_println(const char *str);
};

#endif // EEPROMHELPER_H
