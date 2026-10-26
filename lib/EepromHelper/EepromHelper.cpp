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
    _print_cat_table();
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
    CRC8 crc = _get_crc();
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
    CRC8 readbackCrc = _get_crc();
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
}

void EepromHelper::_print_cat_table()
{
    _info("CAT Table:");
    _infof("\t%16s: 0x%04X (%d) - %d bytes", "CAT", 0, 0, _cat_table_size());
    for (int i = 0; i < NUM_KNOWN_SERVICES; i++)
    {
        uint16_t serviceBaseAddress = _serviceBaseAddresses[i];

        if (serviceBaseAddress == UNINITIALISED_CONFIG_BASE_ADDRESS)
        {
            _infof("\t%16s: No entry", ServiceId::_values()[i]._to_string());
        }
        else
        {
            _infof("\t%16s: 0x%04X (%d) - %d bytes", ServiceId::_values()[i]._to_string(), serviceBaseAddress, serviceBaseAddress, _get_config_size(ServiceId::_values()[i]));
        }
    }
}

CRC8 EepromHelper::_get_crc()
{
    // Use XOR out of 0x55 as the default (0x00) will mean any all-zero entry will have a CRC of 0
    // That's susceptible to missing errors
    return CRC8(7, 0, 0x55);
}

void EepromHelper::_write_cat_base_address(ServiceId serviceId, uint16_t baseAddress)
{
    uint8_t serviceIndex = serviceId._to_index();
    write16(4 + 2 * serviceIndex, baseAddress);
    _serviceBaseAddresses[serviceIndex] = baseAddress;
    uint8_t newCrc = _calculate_cat_crc();
    write8(_cat_table_size() - 1, newCrc);
    _verify_cat_crc();
}

uint16_t EepromHelper::_cat_table_size()
{
    uint16_t catLengthAddress = sizeof(_initValues);
    uint8_t catEntries = read8(catLengthAddress);
    // +1 for conversion from address to number of bytes
    // +1 for CRC
    uint16_t catLength = catLengthAddress + 2 * catEntries + 2;
    return catLength;
}

uint8_t EepromHelper::_calculate_cat_crc()
{
    // Calculates the CRC of the stored data in the CAT table
    CRC8 crc = _get_crc();
    uint16_t bytesToRead = _cat_table_size() - 1;
    uint8_t buffer[bytesToRead];
    read_bytes(0, buffer, bytesToRead, &crc);
    return crc.calc();
}

uint8_t EepromHelper::_calculate_expected_cat_crc()
{
    CRC8 crc = _get_crc();
    crc.add((uint8_t *)_initValues, sizeof(_initValues));
    crc.add(NUM_KNOWN_SERVICES);
    crc.add((uint8_t *)_serviceBaseAddresses, sizeof(_serviceBaseAddresses));
    return crc.calc();
}

bool EepromHelper::_verify_cat_crc()
{
    return _calculate_cat_crc() == read8(_cat_table_size() - 1);
}

uint16_t EepromHelper::_allocate_new_config(ServiceId serviceId, size_t configSize)
{
    // Shuffles all the data around to add the config in the correct order in memory and
    // updates the CAT.Returns the base address of the newly created config entry

    // Actual size is 4 bytes larger than data (2 for length, 1 for version, 1 for CRC)

    // Reason for shuffling is that it saves fragmentation (config will always be sorted and tightly packed)
    // and config updates are likely to be rare events
    _infof("Allocating space for new %s config (%d bytes)", serviceId._to_string(), configSize);

    // Because of the inherent assumption that all existing configs are closely packed in order
    // we can just shuffle all configs with serviceId > the serviceId of the new config to the right
    // by configSize bytes which should create a perfectly sized gap

    uint16_t baseAddress = UNINITIALISED_CONFIG_BASE_ADDRESS;
    for (int i = ServiceId::_size() - 1; i > serviceId._to_index(); i--)
    {
        ServiceId tmpServiceID = ServiceId::_values()[i];
        // _debugf("Checking %s", tmpServiceID._to_string());
        uint16_t tmpBaseAddress = _get_config_base_address(tmpServiceID);
        if (tmpBaseAddress == UNINITIALISED_CONFIG_BASE_ADDRESS)
        {
            continue;
        }

        uint16_t tmpconfigSize = _get_config_size(tmpServiceID);
        uint16_t newBaseAddress = tmpBaseAddress + configSize;

        // Read data into buffer and write back to new address
        // Don't need to worry about CRC calculation - just move it
        // If there is an issue, it will be detected when the data is read
        uint8_t tmpBuffer[tmpconfigSize];

        read_bytes(tmpBaseAddress, tmpBuffer, tmpconfigSize);
        write_bytes(newBaseAddress, tmpBuffer, tmpconfigSize);
        _write_cat_base_address(tmpServiceID, newBaseAddress);

        // Save the address of the last valid config we moved
        // that should be the start address of our serivce
        baseAddress = tmpBaseAddress;
    }

    // All the higher number services have moved out of the way, just need to work
    // out where the previous entry ends
    uint16_t newBaseAddress = UNINITIALISED_CONFIG_BASE_ADDRESS;

    // If we've moved something out of the way, then the address of where it was should be where
    // the new config starts
    if (baseAddress != UNINITIALISED_CONFIG_BASE_ADDRESS)
    {
        newBaseAddress = baseAddress;
    }
    else
    {
        // Otherwise, iterate through the entries before the new one
        // and tack onto the end of the last valid one
        for (int i = 0; i < serviceId._to_index(); i++)
        {
            ServiceId tmpServiceID = ServiceId::_values()[i];
            // _debugf("Checking %s", tmpServiceID._to_string());
            uint16_t tmpBaseAddress = _get_config_base_address(tmpServiceID);
            if (tmpBaseAddress != UNINITIALISED_CONFIG_BASE_ADDRESS)
            {
                newBaseAddress = tmpBaseAddress + _get_config_size(tmpServiceID);
            }
        }
        if (newBaseAddress == UNINITIALISED_CONFIG_BASE_ADDRESS)
        {
            // No preceding entries either so go directly after CAT table
            newBaseAddress = _cat_table_size();
        }
    }
    _write_cat_base_address(serviceId, newBaseAddress);
    // Reload table into RAM
    _read_cat_table();
    return newBaseAddress;
}

uint16_t EepromHelper::_get_config_base_address(ServiceId serviceId)
{
    return _serviceBaseAddresses[serviceId._to_integral()];
}

uint16_t EepromHelper::_get_config_data_base_address(ServiceId serviceId)
{
    // 2 lengths bytes + 1 version byte
    return _get_config_base_address(serviceId) + 3;
}

uint16_t EepromHelper::_get_config_size(ServiceId serviceId)
{
    uint16_t baseAddress = _get_config_base_address(serviceId);
    if (baseAddress == UNINITIALISED_CONFIG_BASE_ADDRESS)
    {
        _errorf("Attempted to get size of unitialised config: %s", serviceId._to_string());
    }
    return read16(baseAddress);
}

uint8_t EepromHelper::_get_config_crc(ServiceId serviceId)
{
    // CRC is last byte of record
    uint16_t baseAddress = _get_config_base_address(serviceId);
    uint16_t configSize = _get_config_size(serviceId);
    return read8(baseAddress + configSize - 1);
}

bool EepromHelper::read_service_config(ServiceId serviceId, void *config, size_t configDataSize, uint8_t version)
{
    _debugf("%s requested config data load of %lu config data bytes (version %d)", serviceId._to_string(), configDataSize, version);
    uint16_t configBaseAddress = _get_config_base_address(serviceId);
    if (configBaseAddress == UNINITIALISED_CONFIG_BASE_ADDRESS)
    {
        // No entry exists
        _warningf("No valid config data found for %s", serviceId._to_string());
        return false;
    }
    uint16_t eepromConfigSize = _get_config_size(serviceId);
    uint16_t eepromConfigDataSize = eepromConfigSize - _configDataSizeToConfigSizeOffset;
    if (eepromConfigDataSize != configDataSize)
    {
        _errorf("Config size mismatch for %s. Code expected %d bytes of data, EEPROM contains %d bytes", serviceId._to_string(), configDataSize, eepromConfigDataSize);
    }

    uint8_t eepromConfigVersion = get_config_version(serviceId);
    if (version != eepromConfigVersion)
    {
        // @TODO maybe some clever version migration strategy but for now, error
        _errorf("Config version mismatch for %s. Code expected version %d, EEPROM contains version %d", serviceId._to_string(), version, eepromConfigVersion);
    }

    CRC8 crc = _get_crc();

    // Length and version fields are part of the CRC so also need to read those, even though they're not
    // part of the wanted data
    uint8_t buffer[3];
    read_bytes(_get_config_base_address(serviceId), buffer, 3, &crc);

    read_bytes(_get_config_data_base_address(serviceId), (uint8_t *)config, configDataSize, &crc);
    if (_get_config_crc(serviceId) != crc.calc())
    {
        _errorf("Readback data has incorrect CRC");
    }
    return true;
}

void EepromHelper::write_service_config(ServiceId serviceId, void *configData, size_t configDataSize, uint8_t version)
{
    _debugf("%s requested config data write of %lu config data bytes (version %d)", serviceId._to_string(), configDataSize, version);
    uint16_t configBaseAddress = _get_config_base_address(serviceId);
    uint16_t configSize = configDataSize + _configDataSizeToConfigSizeOffset;
    if (configBaseAddress == UNINITIALISED_CONFIG_BASE_ADDRESS)
    {
        // No entry exists
        configBaseAddress = _allocate_new_config(serviceId, configSize);
    }
    else
    {
        // Need to check previous config is the same size as this one
        uint16_t oldConfigSize = _get_config_size(serviceId);

        if (oldConfigSize != configSize)
        {
            _errorf("Mismatched size / version between existing config (%d bytes) and new config (%d bytes)", oldConfigSize, configDataSize);
        }
    }

    // Sanity check complete
    CRC8 crc = _get_crc();
    uint16_t writeAddress = configBaseAddress;

    uint8_t configHeader[] = {(uint8_t)(configSize & 0xFF), (uint8_t)((configSize >> 8) & 0xFF), version};
    write_bytes(writeAddress, configHeader, sizeof(configHeader), &crc);
    writeAddress += sizeof(configHeader);
    write_bytes(writeAddress, (uint8_t *)configData, configDataSize, &crc);
    writeAddress += configDataSize;

    // Check data before writing CRC (so if there's an error, it should fail CRC check next time)
    uint16_t numBytesWritten = writeAddress - configBaseAddress;
    uint8_t buffer[numBytesWritten];
    CRC8 readbackCrc = _get_crc();
    read_bytes(configBaseAddress, buffer, numBytesWritten, &readbackCrc);
    if (crc.calc() != readbackCrc.calc())
    {
        _errorf("Error writing data for %s", serviceId._to_string());
    }

    // Write CRC and verify
    write8(writeAddress, crc.calc());
    if (read8(writeAddress) != crc.calc())
    {
        _errorf("CRC readback for %s failed", serviceId._to_string());
    }

    _print_cat_table();
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

uint8_t EepromHelper::get_config_version(ServiceId serviceId)
{
    return read8(_get_config_base_address(serviceId) + 2);
}

void EepromHelper::get_mac_address(uint8_t *buffer, size_t bufferSize)
{
    if (bufferSize != 6)
    {
        _error("Invalid buffer size allocated for MAC address load");
    }
    _eeprom.read_mac_address(buffer);
}
