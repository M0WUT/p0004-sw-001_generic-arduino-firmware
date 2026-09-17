#include "eeprom_24aa256uid.h"

const int REG_MAC_ADDRESS = 0x7F7A;
const int REG_MANUFACTUER_CODE = 0x7FFA;
const int EXPECTED_MANUFACTURER_CODE = 0x29;
const int REG_DEVICE_ID = 0x7FFB;
const int EXPECTED_DEVICE_ID = 0x48;

EEPROM24AA256UID::EEPROM24AA256UID(I2CBus *bus, int dev_addr) : I2CDevice(bus, dev_addr)
{
    _initialised = (verify_manufacturer_code() && verify_device_id());
}

void EEPROM24AA256UID::read_mac_address(uint8_t mac[6])
{
    read_bytes(REG_MAC_ADDRESS, mac, 6, true);
    // DEBUG_PRINTF(
    //     "Read MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",
    //     mac[0], mac[1], mac[2],
    //     mac[3], mac[4], mac[5]);
}

void EEPROM24AA256UID::write_bytes(int regAddr, uint8_t *data, int numBytes, bool twoByteAddress)
{
    int startPage = regAddr / pageSizeBytes;
    int endPage = regAddr / pageSizeBytes;
    for (int i = startPage; i <= endPage; i++)
    {
        int pageStartAddr = i * pageSizeBytes;
        _write_bytes(max(regAddr, pageStartAddr), data + pageSizeBytes * (i - startPage), min(numBytes, pageSizeBytes), twoByteAddress);
    }
}

int EEPROM24AA256UID::read_bytes(int regAddr, uint8_t *data, int numBytes, bool twoByteAddress)
{
    int bytesRead = 0;
    int startPage = regAddr / pageSizeBytes;
    int endPage = regAddr / pageSizeBytes;
    for (int i = startPage; i <= endPage; i++)
    {
        int pageStartAddr = i * pageSizeBytes;
        bytesRead += _read_bytes(max(regAddr, pageStartAddr), data + pageSizeBytes * (i - startPage), min(numBytes, pageSizeBytes), twoByteAddress);
    }
    return bytesRead;
}

bool EEPROM24AA256UID::is_initialised()
{
    return _initialised;
}

bool EEPROM24AA256UID::verify_manufacturer_code()
{
    return (read8(REG_MANUFACTUER_CODE, true) == EXPECTED_MANUFACTURER_CODE);
}

bool EEPROM24AA256UID::verify_device_id()
{
    return (read8(REG_DEVICE_ID, true) == EXPECTED_DEVICE_ID);
}
