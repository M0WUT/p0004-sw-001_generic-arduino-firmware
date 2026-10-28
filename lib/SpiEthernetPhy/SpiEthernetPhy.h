#ifndef SPIETHERNETPHY_H
#define SPIETHERNETPHY_H

#include <SPI.h>
#include <Ethernet.h>

#include "misc.h"

#define SPI_ETHERNET_DEFAULT_SETTINGS SPISettings(14000000, MSBFIRST, SPI_MODE0)

class SpiEthernetPhy
{
public:
    SpiEthernetPhy(int gpio_miso, int gpio_mosi, int gpio_sclk, int gpio_cs, int gpio_rstn);
    const char *get_spi_instance_name();
    bool w5500_present();

private:
    int _gpio_csn;
    int _gpio_rstn;
    void _reset_phy();
};

#endif // SPIETHERNETPHY_H
