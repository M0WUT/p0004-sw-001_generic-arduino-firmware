#include "EepromHelper.h"

EepromHelper::EepromHelper(EEPROM24AA256UID eeprom, DebugUartHelper *uartHelper) : _eeprom(std::move(eeprom))
{
    _printfBuffer = (char *)malloc(_printfBufferSize);
    _uartHelper = uartHelper;

#ifdef DEBUG_EEPROM
    size_t fakeEepromSize = 32768;
    _fakeEeprom = (uint8_t *)malloc(fakeEepromSize);
    memset(_fakeEeprom, 0, fakeEepromSize);
    _debug_println("Using fake EEPROM");
    hexdump(0, 24);
#else
    if (_eeprom.is_detected())
    {
        _debug_println("EEPROM detected");
    }
    else
    {
        _debug_println("Failed to detect EEPROM");
        while (1)
            ;
    }
#endif
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
        // _debug_printf("Read %#04x from address %#06x\n", data[i], regAddr + i);
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
        // _debug_printf("Writing %#04x to address %#06x\n", data[i], regAddr + i);
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

void EepromHelper::hexdump(uint16_t start_addr, uint16_t size)
{
    int line_length = 8;
    uint8_t data[size];
    this->read_bytes(start_addr, data, size);

    String output = "";
    _debug_println("EEPROM HEXDUMP: Offset(Address)");
    char buf[32];
    for (int i = 0; i < size; i++)
    {
        if (i % line_length == 0)
        {
            if (output != "")
            {
                _debug_println(output.c_str());
            }
            output = "";
            snprintf(buf, sizeof(buf),
                     "\t0x%04X (0x%04X):\t",
                     i,
                     start_addr + i);

            output += buf;
        }
        snprintf(buf, sizeof(buf), "%02X\t", data[i]);
        output += buf;
    }
    if (output != "")
    {
        _debug_println(output.c_str());
    }
}

void EepromHelper::_debug_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    snprintf(_printfBuffer, _printfBufferSize, "[EEPROM] %s", fmt);
    _uartHelper->vprintf(_printfBuffer, args);
    va_end(args);
}
