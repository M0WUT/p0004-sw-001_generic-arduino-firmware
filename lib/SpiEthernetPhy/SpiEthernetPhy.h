#ifndef SPIETHERNETPHY_H
#define SPIETHERNETPHY_H

#include <SPI.h>
#include <Ethernet.h>

#include "misc.h"

class SpiEthernetPhy
{
public:
    SpiEthernetPhy(int gpio_miso, int gpio_mosi, int gpio_sclk, int gpio_cs, int gpio_rstn);
    const char *get_spi_instance_name();

private:
    SPIClass _phySpi;
    int _gpio_csn;
    int _gpio_rstn;

    void _initialise_io();
    void _reset_phy();
};

#endif // SPIETHERNETPHY_H
