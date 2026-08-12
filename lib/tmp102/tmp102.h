#ifndef TMP102_H
#define TMP102_H

#include "i2c.h"

class TMP102 : public I2CDevice
{
public:
    TMP102(I2CBus *bus, int dev_addr);
    float read_temperature();
};

#endif // TMP102_H