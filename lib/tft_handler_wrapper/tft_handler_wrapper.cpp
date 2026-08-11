#include "tft_handler_wrapper.h"

/*
This file is entirely a nasty hack to allow member functions of the TFTHandler class
to be passed to LVGL which uses C-style callbacks that cannot be in another namespace.

It does this by converting the various callback functions into static functions that point
at the instance of the class.

N.B. really bad things will happen if these callbacks are used while tft is unitialised

*/
static TFTHandler *tft;
SPIDMA *tftSpiDma;

extern "C" void DMA1_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&tftSpiDma->hdma);
}

extern "C" void SPI3_IRQHandler(void)
{
    HAL_SPI_IRQHandler(tftSpiDma->hspi);
}

extern "C" void lcd_transfer_complete_callback(SPI_HandleTypeDef *hspi)
{
    tft->spi_transfer_complete_cb();
}

static void lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmdSize, const uint8_t *param, size_t paramSize)
{
    return tft->lcd_send_cmd(disp, cmd, cmdSize, param, paramSize);
}

static void lcd_send_colour(lv_display_t *disp, const uint8_t *cmd, size_t cmdSize, uint8_t *param, size_t paramSize)
{
    return tft->lcd_send_colour(disp, cmd, cmdSize, param, paramSize);
}

TFTHandler *create_tft_handler(SPISettings settings, int gpioMosi, int gpioMiso, int gpioSclk, int gpioCs, int gpioDc, int gpioReset, int horRes, int verRes, lv_disp_rotation_t rotation)
{
    tftSpiDma = new SPIDMA(gpioMosi, gpioMiso, gpioSclk, settings);
    if (tftSpiDma->hspi->Instance != TFT_EXPECTED_SPI_CONTROLLER)
    {
        // I'm being really lazy and hardcoded this for SPI3 and DMA1 Stream 0
        // Checking for this (as only one SPI instance can be SPI1)
        // The other ones don't need it
        while (1)
            ;
    }
    tft = new TFTHandler(tftSpiDma, gpioDc, gpioCs, gpioReset, horRes, verRes, rotation);
    HAL_SPI_RegisterCallback(tftSpiDma->hspi, HAL_SPI_TX_COMPLETE_CB_ID, lcd_transfer_complete_callback);
    tft->create_lvgl_screen(lcd_send_cmd, lcd_send_colour);
    return tft;
}