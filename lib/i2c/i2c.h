#ifndef I2C_H
#define I2C_H

#include "Arduino.h"
#include <Wire.h>
#include "debug.h"

class I2C
{
public:
    I2C(const int gpio_scl, const int gpio_sda);
    int scan(int start_addr = 1, int stop_addr = 127, int *buf = NULL, int buf_length = 0xFF);

private:
    TwoWire _iic;
    String _to_str();
};

#endif // I2C_H