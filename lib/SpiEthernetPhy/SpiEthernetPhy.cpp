#include "SpiEthernetPhy.h"

SpiEthernetPhy::SpiEthernetPhy(int gpio_miso, int gpio_mosi, int gpio_sclk, int gpio_csn, int gpio_rstn) : _gpio_csn(gpio_csn), _gpio_rstn(gpio_rstn)
{
    // Setup reset pin
    pinMode(_gpio_rstn, OUTPUT);
    digitalWrite(_gpio_rstn, HIGH);

    // Setup reset pin
    pinMode(_gpio_csn, OUTPUT);
    digitalWrite(_gpio_csn, HIGH);

    SPI.setMISO(gpio_miso);
    SPI.setMOSI(gpio_mosi);
    SPI.setSCLK(gpio_sclk);
    // If the hardware CS is used, it injects extra edges
    // so disable it for the SPI driver
    SPI.setSSEL(PNUM_NOT_DEFINED);

    // ... and let the Ethernet driver handle it exclusively
    Ethernet.init(gpio_csn);

    // Have to begin transaction to apply new settings
    SPI.beginTransaction(SPI_ETHERNET_DEFAULT_SETTINGS);
    SPI.endTransaction();

    _reset_phy();
}

const char *SpiEthernetPhy::get_spi_instance_name()
{
    const char *spiName = spi_controller_name(&SPI);
    return spiName;
}

bool SpiEthernetPhy::w5500_present()
{

    constexpr int W5500_VERSION_REG = 0x0039;
    constexpr int W5500_EXPECTED_VER = 0x04;

    // W5500 SPI header:
    // Address[15:8], Address[7:0], Control byte
    // Control byte: BSB=00000 (Common Register), RWB=0 (Read), OM=00 (VDM)
    SPI.beginTransaction(SPI_ETHERNET_DEFAULT_SETTINGS);
    digitalWrite(_gpio_csn, LOW);

    SPI.transfer((W5500_VERSION_REG >> 8) & 0xFF);
    SPI.transfer(W5500_VERSION_REG & 0xFF);
    SPI.transfer(0x00); // Common register block, read, variable-length mode

    uint8_t version = SPI.transfer(0x00);
    SPI.endTransaction();

    digitalWrite(_gpio_csn, HIGH);

    return version == W5500_EXPECTED_VER;
}

void SpiEthernetPhy::_reset_phy()
{
    digitalWrite(_gpio_rstn, LOW);
    delay(500);
    digitalWrite(_gpio_rstn, HIGH);
    delay(10);
}
