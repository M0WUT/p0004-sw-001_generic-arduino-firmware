#ifndef EEPROM_24AA256UID_H
#define EEPROM_24AA256UID_H

#include "i2c.h"

class EEPROM24AA256UID : public I2CDevice
{
public:
    EEPROM24AA256UID(I2CBus *bus, int dev_addr);
    void read_mac_address(uint8_t mac[6]);
    int write_bytes(int regAddr, uint8_t *data, int numBytes);
    int read_bytes(int regAddr, uint8_t *data, int numBytes);
    bool is_detected();

private:
    const int pageSizeBytes = 64;
    bool _detected = false;
    bool verify_manufacturer_code();
    bool verify_device_id();
};

#endif // EEPROM_24AA256UID_H