#include "EepromHelper.h"

EepromHelper::EepromHelper(EEPROM24AA256UID eeprom, DebugUartHelper *uartHelper) : _eeprom(std::move(eeprom))
{
    _printfBuffer = (char *)malloc(_printfBufferSize);
    _uartHelper = uartHelper;

#ifdef DEBUG_EEPROM
    _fakeEeprom = (uint8_t *)malloc(32768);
    _debug_println("Using fake EEPROM");
#else
    _debug_println(_eeprom.is_detected() ? "EEPROM detected" : "EEPROM not detected");
#endif
    struct test_t
    {
        uint8_t x;
        uint16_t y;
    } test, test2;
    test.x = 0x12;
    test.y = 0x3456;
    write_bytes(0, (uint8_t *)&test, sizeof(test));
    read_bytes(0, (uint8_t *)&test2, sizeof(test2));
    delay(10);
}

void EepromHelper::_debug_print(const char *str)
{
    _uartHelper->printf("[EEPROM] %s", str);
}

void EepromHelper::_debug_println(const char *str)
{
    _uartHelper->printf("[EEPROM] %s\n", str);
}

int EepromHelper::read_bytes(uint16_t regAddr, uint8_t *data, int numBytes)
{
#ifdef DEBUG_EEPROM
    for (int i = 0; i < numBytes; i++)
    {
        data[i] = _fakeEeprom[regAddr + i];
        _debug_printf("Read %#04x from address %#06x\n", data[i], regAddr + i);
    }
    return numBytes;
#else
    return _eeprom.read_bytes(regAddr, data, numBytes);

#endif
}

int EepromHelper::write_bytes(uint16_t regAddr, uint8_t *data, int numBytes)
{
#ifdef DEBUG_EEPROM
    for (int i = 0; i < numBytes; i++)
    {
        _fakeEeprom[regAddr + i] = data[i];
        _debug_printf("Writing %#04x to address %#06x\n", data[i], regAddr + i);
    }
    return numBytes;
#else
    return _eeprom.write_bytes(regAddr, data, numBytes);

#endif
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
