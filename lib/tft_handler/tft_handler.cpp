#include "tft_handler.h"

TFTHandler::TFTHandler(SPIDMA *pspi, int gpioDc, int gpioCs, int gpioReset, int horRes, int verRes, lv_disp_rotation_t rotation)
{
    // Copy variables into class members
    _pspi = pspi;
    _gpioDc = gpioDc;
    _gpioCs = gpioCs;
    _gpioReset = gpioReset;
    _horRes = horRes;
    _verRes = verRes;
    _rotation = rotation;

    // Initialise
    _init_io();
    _reset_tft();
    _init_lvgl();
}

lv_display_t *TFTHandler::get_lvgl_display()
{
    return _disp;
}

// DMA / SPI Stuff
void TFTHandler::lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmdSize, const uint8_t *param, size_t paramSize)
{
    LV_UNUSED(disp);
    while (_bus_busy)
        ;
    _bus_busy = 1;

    digitalWrite(_gpioDc, 0);
    digitalWrite(_gpioCs, 0);
    /* send command */
    _pspi->transfer((void *)cmd, cmdSize);
    digitalWrite(_gpioDc, 1);

    _pspi->transfer((void *)param, paramSize);

    digitalWrite(_gpioCs, 1);
    _bus_busy = 0;
}

void TFTHandler::lcd_send_colour(lv_display_t *disp, const uint8_t *cmd, size_t cmdSize, uint8_t *param, size_t paramSize)
{
    LV_UNUSED(disp);
    while (_bus_busy)
    {
        ;
    }
    _bus_busy = 1;

    /* DCX low (command) */
    digitalWrite(_gpioDc, 0);
    /* CS low */
    digitalWrite(_gpioCs, 0);
    /* send command */
    _pspi->transfer((void *)cmd, cmdSize);

    /* DCX high (data) */
    digitalWrite(_gpioDc, 1);

    // // Non-DMA way
    // _pspi->transfer(param, paramSize);
    // spi_transfer_complete_cb();

    // DMA way
    _pspi->transferDMA(param, paramSize);
}

void TFTHandler::_init_io()
{
    pinMode(_gpioReset, OUTPUT);
    digitalWrite(_gpioReset, HIGH);

    pinMode(_gpioDc, OUTPUT);
    digitalWrite(_gpioDc, HIGH);

    pinMode(_gpioCs, OUTPUT);
    digitalWrite(_gpioCs, HIGH);
}

void TFTHandler::_init_lvgl()
{
    lv_init();
    lv_tick_set_cb((lv_tick_get_cb_t)millis);
}

void TFTHandler::_reset_tft()
{
    digitalWrite(_gpioReset, 0);
    delay(100);
    digitalWrite(_gpioReset, 1);
    delay(100);
}

void TFTHandler::_allocate_buffers(int bufferSizeBytes)
{
    _buf0 = (uint16_t *)malloc(bufferSizeBytes);
    _buf1 = (uint16_t *)malloc(bufferSizeBytes);
    if ((_buf0 == NULL) || (_buf1 == NULL))
    {
        while (1)
            ;
    }
}

void TFTHandler::create_lvgl_screen(lv_st7789_send_cmd_cb_t send_cmd_func, lv_st7789_send_color_cb_t send_colour_func)
{
    int hSize, vSize;

    if (_rotation == LV_DISPLAY_ROTATION_90 || _rotation == LV_DISPLAY_ROTATION_270)
    {
        hSize = _verRes;
        vSize = _horRes;
    }
    else
    {
        hSize = _horRes;
        vSize = _verRes;
    }

    _disp = lv_st7789_create(hSize, vSize, LV_LCD_FLAG_NONE, send_cmd_func, send_colour_func);
    lv_st7789_set_gap(_disp, 0, 0x23);
    lv_st7789_set_invert(_disp, true);
    lv_display_set_rotation(_disp, _rotation);

    // Buffers
    _framebufferSizeBytes = _horRes * _verRes * (LV_COLOR_DEPTH / 8) / 10;
    _allocate_buffers(_framebufferSizeBytes);
    lv_display_set_buffers(_disp, _buf0, _buf1, _framebufferSizeBytes, LV_DISPLAY_RENDER_MODE_PARTIAL);
}

void TFTHandler::spi_transfer_complete_cb()
{
    digitalWrite(_gpioCs, 1);
    lv_disp_flush_ready(_disp);
    _bus_busy = 0;
}
