#include "tmp102.h"

int REG_TEMPERATURE = 0x00;
int REG_CONFIGURATION = 0x01;
int REG_TLOW = 0x02;
int REG_THIGH = 0x03;

TMP102::TMP102(I2CBus *bus, int dev_addr) : I2CDevice(bus, dev_addr)
{
    ;
}

float TMP102::read_temperature()
{
    const float celsius_per_lsb = 0.0625;
    // Raw data contains 12 bits of data in the top-most 12 bits
    // as two's complement with 1 LSB
    int16_t temp = read16(REG_TEMPERATURE) >> 4;
    // Sign-extend the 12-bit value
    if (temp & 0x0800)
    {
        temp |= 0xF000;
    }

    return temp * celsius_per_lsb;
}
