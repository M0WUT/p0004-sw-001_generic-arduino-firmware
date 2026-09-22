#include "i2c.h"

I2CBus::I2CBus(const int gpio_scl, const int gpio_sda) : _i2c(gpio_sda, gpio_scl)
{
    _i2c.begin();
}

int I2CBus::scan(int start_addr, int stop_addr, int *buf, int buf_length)
{
    // DEBUG_PRINTF("Scanning %s. Addresses: %d-%d\n", _to_str().c_str(), start_addr, stop_addr);

    int num_devices = 0;

    for (int addr = start_addr; addr <= stop_addr; addr++)
    {
        _i2c.beginTransmission(addr);
        int error = _i2c.endTransmission();

        // error will be 0 if device ack-ed
        if (error != 0)
            continue;

        // Device ack-ed
        // DEBUG_PRINTF("Found device at 0x%02X\n", addr);

        // Only put as many found addresses in the buffer as it can hold
        if (num_devices < buf_length)
        {
            buf[num_devices] = addr;
        }

        // but always log the entire number of devices that responded
        num_devices++;
    }

    // DEBUG_PRINTLN("Scan complete");

    return num_devices;
}

String I2CBus::_to_str()
{
    I2C_TypeDef *x = _i2c.getHandle()->Instance;
    if (x == I2C1)
        return "I2C1";
    else if (x == I2C2)
        return "I2C2";
    else if (x == I2C3)
        return "I2C3";
    else if (x == I2C4)
        return "I2C4";
    else
        return "Unknown I2C controller";
}

I2CDevice::I2CDevice(I2CBus *bus, int dev_addr)
{
    _bus = bus;
    _dev_addr = dev_addr;
    _i2c = &_bus->_i2c;
}

uint8_t I2CDevice::read8(int reg_addr, bool two_byte_address)
{
    uint8_t data[] = {0};
    read_bytes(reg_addr, data, 1, two_byte_address);
    return data[0];
}

void I2CDevice::write8(int reg_addr, uint8_t data, bool two_byte_address)
{
    write_bytes(reg_addr, &data, 1, two_byte_address);
}

uint16_t I2CDevice::read16(int reg_addr, bool two_byte_address, bool littleEndian = true)
{
    uint16_t result;
    uint8_t data[] = {0, 0};

    read_bytes(reg_addr, data, 2, two_byte_address);

    result = (uint16_t)data[1] << (littleEndian ? 8 : 0) | (uint16_t)data[0] << (littleEndian ? 0 : 8);

    return result;
}

void I2CDevice::write16(int reg_addr, uint16_t data, bool two_byte_address, bool littleEndian = true)
{
    uint8_t data_buf[] = {(uint8_t)((data >> (littleEndian ? 0 : 8)) & 0xFF), (uint8_t)((data >> (littleEndian ? 8 : 0)) & 0xFF)};
    write_bytes(reg_addr, data_buf, 2, two_byte_address);
}

void I2CDevice::_write_bytes(int reg_addr, uint8_t *data, int numBytes, bool two_byte_address)
{
    _i2c->beginTransmission(_dev_addr);
    if (two_byte_address)
        _i2c->write((reg_addr >> 8) & 0xFF);
    _i2c->write(reg_addr & 0xFF);
    for (int i = 0; i < numBytes; i++)
    {
        _i2c->write(data[i]);
    }
    _i2c->endTransmission();
}

int I2CDevice::_read_bytes(int reg_addr, uint8_t *data, int numBytes, bool two_byte_address)
{
    _i2c->beginTransmission(_dev_addr);
    if (two_byte_address)
        _i2c->write((reg_addr >> 8) & 0xFF);
    _i2c->write(reg_addr & 0xFF);
    _i2c->endTransmission();
    _i2c->requestFrom(_dev_addr, numBytes);

    int read_bytes = _i2c->readBytes(data, numBytes);
    if (read_bytes != numBytes)
    {
        // DEBUG_PRINTLN("I2C read failed to read expected number of bytes");
    }

    _i2c->endTransmission();
    return read_bytes;
}

void I2CDevice::write_bytes(int reg_addr, uint8_t *data, int numBytes, bool two_byte_address)
{
    // Broken out into separate function as some things (like EEPROMs) may require multi-byte
    // read/write operations to be split up to not cross for page boundaries
    _write_bytes(reg_addr, data, numBytes, two_byte_address);
}

int I2CDevice::read_bytes(int reg_addr, uint8_t *data, int numBytes, bool two_byte_address)
{
    return _read_bytes(reg_addr, data, numBytes, two_byte_address);
}