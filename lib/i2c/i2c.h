#ifndef I2C_BUS_H
#define I2C_BUS_H

#include "Arduino.h"
#include <Wire.h>
#include "debug.h"

class I2CBus
{
public:
    I2CBus(const int gpio_scl, const int gpio_sda);
    int scan(int start_addr = 1, int stop_addr = 127, int *buf = NULL, int buf_length = 0xFF);
    friend class I2CDevice;

private:
    TwoWire _i2c;
    String _to_str();
};

class I2CDevice
{
public:
    I2CDevice(I2CBus *bus, int dev_addr);
    uint8_t read8(int reg_addr, bool two_byte_address = false);
    void write8(int reg_addr, uint8_t data, bool two_byte_address = false);
    uint16_t read16(int reg_addr, bool two_byte_address = false);
    void write16(int reg_addr, uint16_t data, bool two_byte_address = false);
    void write_bytes(int reg_addr, uint8_t *data, uint8_t num_bytes, bool two_byte_address = false);
    int read_bytes(int reg_addr, uint8_t *data, uint8_t num_bytes, bool two_byte_address = false);

private:
    I2CBus *_bus;
    int _dev_addr;
    TwoWire *_i2c;
};

#endif // I2C_BUS_H