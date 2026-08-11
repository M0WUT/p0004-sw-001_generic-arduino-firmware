#ifndef TFT_HANDLER_H
#define TFT_HANDLER_H

#include <lvgl.h>
#include "spi_dma.h"
#include "debug.h"

class TFTHandler
{
public:
    TFTHandler(SPIDMA *pspi, int gpioDc, int gpioCs, int gpioReset, int horRes, int verRes, lv_disp_rotation_t rotation);

    lv_display_t *get_lvgl_display();

    // Functions that are required by wrapper class
    void lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmdSize, const uint8_t *param, size_t paramSize);
    void lcd_send_colour(lv_display_t *disp, const uint8_t *cmd, size_t cmdSize, uint8_t *param, size_t paramSize);
    void create_lvgl_screen(lv_st7789_send_cmd_cb_t send_cmd_func, lv_st7789_send_color_cb_t send_colour_func);
    void spi_transfer_complete_cb();

protected:
    SPIDMA *_pspi;
    int _gpioDc;
    int _gpioCs;
    int _gpioReset;
    int _horRes;
    int _verRes;
    lv_disp_rotation_t _rotation;
    int _framebufferSizeBytes;
    void *_buf0;
    void *_buf1;
    SPISettings _spiSettings = SPISettings(60000000, MSBFIRST, SPI_MODE0);
    lv_display_t *_disp = NULL;

    bool _bus_busy = false;
    void _init_io();
    void _init_lvgl();
    void _reset_tft();
    void _allocate_buffers(int bufferSizeBytes);
};
#endif // TFT_HANDLER_H