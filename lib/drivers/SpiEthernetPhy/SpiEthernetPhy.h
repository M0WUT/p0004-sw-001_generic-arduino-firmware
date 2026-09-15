#ifndef SPIETHERNETPHY_H
#define SPIETHERNETPHY_H

#include <SPI.h>
#include <Ethernet.h>

#include "misc/misc.h"

class SpiEthernetPhy
{
public:
    SpiEthernetPhy(int gpio_miso, int gpio_mosi, int gpio_sclk, int gpio_cs, int gpio_rstn);
    String get_spi_instance_name();

private:
    int _gpio_miso;
    int _gpio_mosi;
    int _gpio_sclk;
    int _gpio_cs;
    int _gpio_rstn;

    void _initialise_io();
    void _reset_phy();
};

#endif // SPIETHERNETPHY_H
