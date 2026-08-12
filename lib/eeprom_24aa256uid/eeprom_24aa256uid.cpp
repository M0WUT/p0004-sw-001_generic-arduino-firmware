#include "eeprom_24aa256uid.h"

const int REG_MAC_ADDRESS = 0x7F7A;
const int REG_MANUFACTUER_CODE = 0x7FFA;
const int EXPECTED_MANUFACTURER_CODE = 0x29;
const int REG_DEVICE_ID = 0x7FFB;
const int EXPECTED_DEVICE_ID = 0x48;

EEPROM24AA256UID::EEPROM24AA256UID(I2CBus *bus, int dev_addr) : I2CDevice(bus, dev_addr)
{
    if (verify_manufacturer_code() && verify_device_id())
    {
        DEBUG_PRINTLN("Detected correct internal EEPROM");
        _initialised = true;
    }
}

void EEPROM24AA256UID::read_mac_address(uint8_t mac[6])
{
    read_bytes(REG_MAC_ADDRESS, mac, 6, true);
    DEBUG_PRINTF(
        "Read MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",
        mac[0], mac[1], mac[2],
        mac[3], mac[4], mac[5]);
}

bool EEPROM24AA256UID::verify_manufacturer_code()
{
    return (read8(REG_MANUFACTUER_CODE, true) == EXPECTED_MANUFACTURER_CODE);
}

bool EEPROM24AA256UID::verify_device_id()
{
    return (read8(REG_DEVICE_ID, true) == EXPECTED_DEVICE_ID);
}
