#include "spi_dma.h"

SPIDMA::SPIDMA(int gpio_mosi, int gpio_miso, int gpio_sclk, SPISettings settings)
{
    // Assign GPIO
    _pspi = new SPIClass(gpio_mosi, gpio_miso, gpio_sclk);
    _pspi->begin();
    hspi = _pspi->getHandle();

    DEBUG_PRINTLN("Started SPI DMA driver on " + spi_controller_name(_pspi));

    _spiSettings = settings;
    _pspi->beginTransaction(_spiSettings);

    _init_dma();
    _init_interrupts();

    HAL_StatusTypeDef status = HAL_SPI_Init(hspi);
    if (status != HAL_OK)
    {
        while (1)
            ;
    }
}

void SPIDMA::transfer(void *buf, size_t count)
{
    // _pspi->transfer(buf, count, true);
    HAL_SPI_Transmit(hspi, (uint8_t *)buf, count, _timeout_ms);
}

void SPIDMA::transferDMA(void *buf, size_t count)
{
    // Must make sure cache has been flushed to RAM before
    // starting DMA transfer or just get corrupted data
    SCB_CleanDCache_by_Addr(buf, count);
    HAL_SPI_Transmit_DMA(hspi, (uint8_t *)buf, count);
}

void SPIDMA::_init_dma()
{
    __HAL_RCC_DMA1_CLK_ENABLE();

    hdma.Instance = DMA1_Stream0;
    hdma.Init.Request = DMA_REQUEST_SPI3_TX;
    hdma.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma.Init.MemInc = DMA_MINC_ENABLE;
    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma.Init.Mode = DMA_NORMAL;
    hdma.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma) != HAL_OK)
    {
        while (1)
            ;
    }

    hspi->hdmatx = &hdma;
    hdma.Parent = hspi;
}

void SPIDMA::_init_interrupts()
{
    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

    HAL_NVIC_SetPriority(SPI3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(SPI3_IRQn);
}
