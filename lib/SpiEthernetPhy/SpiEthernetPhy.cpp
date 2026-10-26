#include "SpiEthernetPhy.h"

SpiEthernetPhy::SpiEthernetPhy(int gpio_miso, int gpio_mosi, int gpio_sclk, int gpio_cs, int gpio_rstn) : _phySpi(gpio_mosi, gpio_miso, gpio_sclk, PNUM_NOT_DEFINED), _gpio_csn(gpio_cs), _gpio_rstn(gpio_rstn)
{
    // Setup reset pin
    pinMode(_gpio_rstn, OUTPUT);
    digitalWrite(_gpio_rstn, HIGH);

    pinMode(_gpio_csn, OUTPUT);
    digitalWrite(_gpio_csn, HIGH);

    _reset_phy();
    _initialise_io();
    SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
}

const char *SpiEthernetPhy::get_spi_instance_name()
{
    const char *spiName = spi_controller_name(&SPI);
    return spiName;
}

void SpiEthernetPhy::_initialise_io()
{

    // Setup SPI bus - Ethernet library is annoying and hard codes use of SPI
    _phySpi.begin();
    SPI = _phySpi;
    // ... and let the Ethernet driver handle it exclusively
    Ethernet.init(_gpio_csn);
}

void SpiEthernetPhy::_reset_phy()
{
    digitalWrite(_gpio_rstn, LOW);
    delay(500);
    digitalWrite(_gpio_rstn, HIGH);
    delay(10);
}
