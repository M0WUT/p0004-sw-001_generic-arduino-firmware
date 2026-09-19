#include "EepromHelper.h"

EepromHelper::EepromHelper(EEPROM24AA256UID eeprom, DebugUartHelper *uartHelper) : _eeprom(eeprom)
{
    _uartHelper = uartHelper;
    if (_eeprom.is_initialised())
    {
        _debug_println("EEPROM detected");
    }
    else
    {
        _debug_println("EEPROM not detected");
    }
}

void EepromHelper::_debug_print(const char *str)
{
    _uartHelper->printf("[EEPROM] %s", str);
}

void EepromHelper::_debug_println(const char *str)
{
    _uartHelper->printf("[EEPROM] %s\n", str);
}

int EepromHelper::load_service_data(ServiceId serviceId, void *eepromStruct, size_t eepromStructSize, int version)
{
    _debug_printf("%s requested read of %d bytes", serviceId._to_string(), eepromStructSize);
    return 0;
}

void EepromHelper::_debug_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    snprintf(_printfBuffer, _printfBufferSize, "[EEPROM] %s", fmt);
    _uartHelper->printf(_printfBuffer, args);
    va_end(args);
}
