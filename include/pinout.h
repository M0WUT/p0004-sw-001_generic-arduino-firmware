#ifndef INC_PINOUT_H
#define INC_PINOUT_H

// DEBUG UART
const int gpio_uart_debug_tx = PD5;
const int gpio_uart_debug_rx = PD6;
const int UART_DEBUG_BAUD = 115200;

// LCD
const int gpio_tft_dc = PD15;
const int gpio_tft_rst = PA2;
const int gpio_tft_bl = PC6;
const int gpio_tft_cs = PA4;
const int gpio_tft_mosi = PB2;
const int gpio_tft_miso = PB4_ALT1;
const int gpio_tft_sclk = PB3_ALT1;
#define TFT_EXPECTED_SPI_CONTROLLER SPI3

// W5500 (Ethernet PHY)
const int gpio_w5500_cs = PD14;
const int gpio_w5500_miso = PA6;
const int gpio_w5500_mosi = PB5;
const int gpio_w5500_sck = PA5;
const int gpio_w5500_rstn = PC7;

// RGB LED
const int gpio_led_red = PA10;
const int gpio_led_green = PA8;
const int gpio_led_blue = PA9;

// Internal I2C
const int gpio_i2c_int_scl = PD12;
const int gpio_i2c_int_sda = PD13;

#endif // INC_PINOUT_H
