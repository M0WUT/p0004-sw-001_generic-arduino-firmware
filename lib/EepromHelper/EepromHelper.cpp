#include "EepromHelper.h"

EepromHelper::EepromHelper(EEPROM24AA256UID eeprom, HelperContext *ctx) : BaseHelper(ctx, "EEPROM"), _eeprom(std::move(eeprom))
{

#ifdef DEBUG_EEPROM
    size_t fakeEepromSize = 32768;
    _fakeEeprom = (uint8_t *)malloc(fakeEepromSize);
    memset(_fakeEeprom, 0, fakeEepromSize);
    _info("Using fake EEPROM");
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

int EepromHelper::read_bytes(uint16_t regAddr, uint8_t *data, int numBytes)
{
#ifdef DEBUG_EEPROM
    memcpy(data, &_fakeEeprom[regAddr], numBytes);
    return numBytes;
#else
    return _eeprom.read_bytes(regAddr, data, numBytes);

#endif
}

int EepromHelper::write_bytes(uint16_t regAddr, uint8_t *data, int numBytes)
{
#ifdef DEBUG_EEPROM
    memcpy(&_fakeEeprom[regAddr], data, numBytes);
    return numBytes;
#else
    return _eeprom.write_bytes(regAddr, data, numBytes);

#endif
}

int EepromHelper::load_service_data(ServiceId serviceId, uint8_t *eepromStruct, size_t eepromStructSize, int version)
{
    _debugf("%s requested data load of %lu bytes for version %d\r\n", serviceId._to_string(), eepromStructSize, version);
    return 0;
}

void EepromHelper::hexdump(uint16_t start_addr, uint16_t size)
{
    int line_length = 8;
    uint8_t data[size];
    this->read_bytes(start_addr, data, size);

    String output = "";
    _debug("EEPROM HEXDUMP: Offset(Address)");
    char buf[32];
    for (int i = 0; i < size; i++)
    {
        if (i % line_length == 0)
        {
            if (output != "")
            {
                _debug(output.c_str());
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
        _debug(output.c_str());
    }
}
