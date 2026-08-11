#ifndef SPI_DMA_H
#define SPI_DMA_H

#include <SPI.h>
#include "pinout.h"
#include "misc.h"
#include "debug.h"

class SPIDMA
{
public:
    SPIDMA(int gpio_mosi, int gpio_miso, int gpio_sclk, SPISettings settings = SPISettings(15000000, MSBFIRST, SPI_MODE0));
    void transfer(void *buf, size_t count);
    void transferDMA(void *buf, size_t count);
    void complete_cb(SPI_HandleTypeDef *hdma);
    DMA_HandleTypeDef hdma;
    SPI_HandleTypeDef *hspi;

private:
    SPIClass *_pspi;
    SPISettings _spiSettings;
    int _timeout_ms = 1000;
    void _init_dma();
    void _init_interrupts();
};

#endif // SPI_DMA_H