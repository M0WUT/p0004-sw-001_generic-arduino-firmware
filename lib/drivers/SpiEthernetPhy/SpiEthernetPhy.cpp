#include "SpiEthernetPhy.h"

SpiEthernetPhy::SpiEthernetPhy(int gpio_miso, int gpio_mosi, int gpio_sclk, int gpio_cs, int gpio_rstn) : _gpio_miso(gpio_miso), _gpio_mosi(gpio_mosi), _gpio_sclk(gpio_sclk), _gpio_cs(gpio_cs), _gpio_rstn(gpio_rstn)
{
    _initialise_io();
    _reset_phy();
}

String SpiEthernetPhy::get_spi_instance_name()
{
    return String(spi_controller_name(&SPI));
}

void SpiEthernetPhy::_initialise_io()
{
    // Setup reset pin
    pinMode(_gpio_rstn, OUTPUT);
    digitalWrite(_gpio_rstn, HIGH);

    // Setup SPI bus
    SPI.setMISO(_gpio_miso);
    SPI.setMOSI(_gpio_mosi);
    SPI.setSCLK(_gpio_sclk);
    // Although the CS pin is a hardware CS pin
    // assigning to the SPI bus causing extra toggling
    // which breaks communication so tell SPI driver it
    // doesn't have a CS...
    SPI.setSSEL(PNUM_NOT_DEFINED);
    SPI.begin();
    // ... and let the Ethernet driver handle it exclusively
    Ethernet.init(_gpio_cs);
}

void SpiEthernetPhy::_reset_phy()
{
    digitalWrite(_gpio_rstn, LOW);
    delay(10);
    digitalWrite(_gpio_rstn, HIGH);
    delay(10);
}
