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
    if (!_eeprom_initialised())
    {
        initialise_eeprom();
    }
    read_eat_table();
}

int EepromHelper::read_bytes(uint16_t regAddr, uint8_t *data, int numBytes, CRC8 *crc)
{
    int numBytesRead;
#ifdef DEBUG_EEPROM
    memcpy(data, &_fakeEeprom[regAddr], numBytes);
    numBytesRead = numBytes;
#else
    numBytesRead = _eeprom.read_bytes(regAddr, data, numBytes);
#endif
    if (numBytesRead != numBytes)
    {
        _warningf("Attempted to read %d bytes. Only managed %d", numBytes, numBytesRead);
    }
    if (crc != NULL)
    {
        crc->add(data, numBytesRead);
    }
    return numBytesRead;
}

int EepromHelper::write_bytes(uint16_t regAddr, uint8_t *data, int numBytes, CRC8 *crc)
{
    int numBytesWritten;
#ifdef DEBUG_EEPROM
    memcpy(&_fakeEeprom[regAddr], data, numBytes);
    numBytesWritten = numBytes;
#else
    numBytesWritten = _eeprom.write_bytes(regAddr, data, numBytes);
#endif
    if (numBytesWritten != numBytes)
    {
        _warningf("Attempted to write %d bytes. Only managed %d", numBytes, numBytesWritten);
    }
    if (crc != NULL)
    {
        crc->add(data, numBytesWritten);
    }
    return numBytesWritten;
}

bool EepromHelper::_eeprom_initialised()
{
    // The data in _initValues is expected to be at the start of the EEPROM if it has been initialised
    int numInitValues = sizeof(_initValues);
    char buffer[numInitValues];
    read_bytes(0, (uint8_t *)buffer, numInitValues);
    for (int i = 0; i < numInitValues; i++)
    {
        if (buffer[i] != _initValues[i])
        {
            _warning("Uninitialised EEPROM detected");
            return false;
        }
    }
    _info("Initialised EEPROM detected");

    return true;
}

void EepromHelper::initialise_eeprom()
{
    _info("Initialising EEPROM");
    CRC8 crc;

    size_t numInitValues = sizeof(_initValues);
    write_bytes(0, (uint8_t *)_initValues, numInitValues, &crc);
    int eepromAddress = numInitValues;
    // Write length field
    uint8_t knownServices = NUM_KNOWN_SERVICES;
    write_bytes(eepromAddress++, &knownServices, 1, &crc);
    // Populate entries
    uint16_t eatTable[NUM_KNOWN_SERVICES];
    size_t eatTableSize = sizeof(eatTable);
    memset(eatTable, 0xFF, eatTableSize);
    write_bytes(eepromAddress, (uint8_t *)eatTable, eatTableSize, &crc);
    eepromAddress += eatTableSize;

    // Readback all data
    uint8_t readbackData[eepromAddress];
    CRC8 readbackCrc;
    read_bytes(0, readbackData, sizeof(readbackData), &readbackCrc);

    // Compare CRC
    uint8_t expectedCrc = crc.calc();
    if (readbackCrc.calc() != expectedCrc)
    {
        hexdump(0, eepromAddress);
        _error("Initialisation readback failed");
    }

    // Write CRC
    write_bytes(eepromAddress, &expectedCrc, 1);
    uint8_t eepromCrc;
    read_bytes(eepromAddress++, &eepromCrc, 1);
    if (eepromCrc != expectedCrc)
    {
        hexdump(0, eepromAddress);
        _error("Failed to write CRC");
    }
}

void EepromHelper::read_eat_table()
{
    read_bytes(1 + sizeof(_initValues), (uint8_t *)&_serviceBaseAddresses, sizeof(_serviceBaseAddresses));
    _info("EAT Table:");
    for (int i = 0; i < NUM_KNOWN_SERVICES; i++)
    {
        unsigned int serviceBaseAddress = static_cast<unsigned int>(_serviceBaseAddresses[i]);
        _infof("\t%16s: 0x%04X%s", ServiceId::_values()[i]._to_string(), serviceBaseAddress, serviceBaseAddress == 0xFFFF ? " (Uninitalised)" : "");
    }
}

int EepromHelper::read_service_config(ServiceId serviceId, void *eepromStruct, size_t eepromStructSize, int version)
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
