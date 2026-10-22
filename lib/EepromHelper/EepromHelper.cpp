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
    _read_cat_table();
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

uint8_t EepromHelper::read8(uint16_t regAddr)
{
    uint8_t data;
    read_bytes(regAddr, &data, 1);
    return data;
}

void EepromHelper::write8(uint16_t regAddr, uint8_t data)
{
    write_bytes(regAddr, &data, 1);
}

uint8_t EepromHelper::read16(uint16_t regAddr)
{
    uint8_t data[2];
    read_bytes(regAddr, data, 2);
    return (uint16_t)data[1] << 8 | (uint16_t)data[0];
}

void EepromHelper::write16(uint16_t regAddr, uint16_t data)
{
    uint8_t buffer[2];
    buffer[0] = data & 0xFF;
    buffer[1] = (data >> 8) & 0xFF;
    write_bytes(regAddr, buffer, 2);
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
    uint16_t catTable[NUM_KNOWN_SERVICES];
    size_t catTableSize = sizeof(catTable);
    memset(catTable, 0xFF, catTableSize);
    write_bytes(eepromAddress, (uint8_t *)catTable, catTableSize, &crc);
    eepromAddress += catTableSize;

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

    // Write CRC and verify
    write8(eepromAddress, expectedCrc);
    if (read8(eepromAddress) != expectedCrc)
    {
        hexdump(0, ++eepromAddress);
        _error("Failed to write CRC");
    }
}

void EepromHelper::_read_cat_table()
{
    read_bytes(1 + sizeof(_initValues), (uint8_t *)&_serviceBaseAddresses, sizeof(_serviceBaseAddresses));
    _print_cat_table();
}

void EepromHelper::_print_cat_table()
{
    _info("CAT Table:");
    for (int i = 0; i < NUM_KNOWN_SERVICES; i++)
    {
        unsigned int serviceBaseAddress = static_cast<unsigned int>(_serviceBaseAddresses[i]);
        _infof("\t%16s: 0x%04X%s", ServiceId::_values()[i]._to_string(), serviceBaseAddress, serviceBaseAddress == 0xFFFF ? " (Uninitalised)" : "");
    }
}

uint16_t EepromHelper::_allocate_new_config(ServiceId serviceId, size_t configSize)
{
    // Shuffles all the data around to add the config in the correct order in memory and
    // updates the CAT.Returns the base address of the newly created config entry

    // Reason for shuffling is that it saves fragmentation (config will always be sorted and tightly packed)
    // and config updates are likely to be rare events
    return 0;
}

uint16_t EepromHelper::_get_config_base_address(ServiceId serviceId)
{
    return _serviceBaseAddresses[serviceId._to_integral()];
}

bool EepromHelper::read_service_config(ServiceId serviceId, void *config, size_t configSize, int version)
{
    _debugf("%s requested data load of %lu bytes for version %d", serviceId._to_string(), configSize, version);
    uint16_t configBaseAddress = _get_config_base_address(serviceId);
    if (configBaseAddress == 0xFFFF)
    {
        // No entry exists
        _warningf("No valid config data found for %s", serviceId._to_string());
        return false;
    }
    return false; // @TODO actually read data if entry does exist
}

int EepromHelper::write_service_config(ServiceId serviceId, void *config, size_t configSize, int version)
{
    _debugf("%s requested to write of %lu bytes for version %d", serviceId._to_string(), configSize, version);
    uint16_t configBaseAddress = _get_config_base_address(serviceId);
    if (configBaseAddress == 0xFFFF)
    {
        // No entry exists
        configBaseAddress = _allocate_new_config(serviceId, configSize);
    }
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
