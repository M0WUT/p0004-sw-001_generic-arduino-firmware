#ifndef TFT_HANDLER_WRAPPER_H
#define TFT_HANDLER_WRAPPER_H

#include <SPI.h>
#include <lvgl.h>
#include "tft_handler.h"

TFTHandler *create_tft_handler(SPISettings settings, int gpioMosi, int gpioMiso, int gpioSclk, int gpioCs, int gpioDc, int gpioReset, int horRes, int verRes, lv_disp_rotation_t rotation);

#endif