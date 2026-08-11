#include "i2c.h"

I2C::I2C(const int gpio_scl, const int gpio_sda) : _iic(gpio_sda, gpio_scl)
{
    _iic.begin();
    DEBUG_PRINTLN("Initialised " + _to_str());
}

int I2C::scan(int start_addr, int stop_addr, int *buf, int buf_length)
{
    DEBUG_PRINTF("Scanning %s. Addresses: %d-%d\n", _to_str().c_str(), start_addr, stop_addr);

    int num_devices = 0;

    for (int addr = start_addr; addr <= stop_addr; addr++)
    {
        _iic.beginTransmission(addr);
        int error = _iic.endTransmission();

        // error will be 0 if device ack-ed
        if (error != 0)
            continue;

        // Device ack-ed
        DEBUG_PRINTF("Found device at 0x%02X\n", addr);

        // Only put as many found addresses in the buffer as it can hold
        if (num_devices < buf_length)
        {
            buf[num_devices] = addr;
        }

        // but always log the entire number of devices that responded
        num_devices++;
    }

    DEBUG_PRINTLN("Scan complete");

    return num_devices;
}

String I2C::_to_str()
{
    I2C_TypeDef *x = _iic.getHandle()->Instance;
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
