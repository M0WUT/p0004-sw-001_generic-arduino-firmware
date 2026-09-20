#include "EepromHelper.h"

EepromHelper::EepromHelper(EEPROM24AA256UID eeprom, DebugUartHelper *uartHelper) : _eeprom(std::move(eeprom))
{
    _printfBuffer = (char *)malloc(_printfBufferSize);
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

int EepromHelper::load_service_data(ServiceId serviceId, uint8_t *eepromStruct, size_t eepromStructSize, int version)
{
    _debug_printf("%s requested data load of %lu bytes for version %d\n", serviceId._to_string(), eepromStructSize, version);
    return 0;
}

void EepromHelper::_debug_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    snprintf(_printfBuffer, _printfBufferSize, "[EEPROM] %s", fmt);
    _uartHelper->vprintf(_printfBuffer, args);
    va_end(args);
}
