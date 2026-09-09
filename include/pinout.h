#ifndef INC_PINOUT_H
#define INC_PINOUT_H

// DEBUG UART
const int gpio_uart_debug_tx = PD5;
const int gpio_uart_debug_rx = PD6;
const int UART_DEBUG_BAUD = 115200;

// LCD
const int gpio_tft_dc = PE1;
const int gpio_tft_rst = PE0;
const int gpio_tft_bl = PE3;
const int gpio_tft_cs = PE4;
const int gpio_tft_mosi = PE6;
const int gpio_tft_miso = PE5;
const int gpio_tft_sclk = PE2;
#define TFT_EXPECTED_SPI_CONTROLLER SPI4

// W5500 (Ethernet PHY)
const int gpio_w5500_cs = PB9;
const int gpio_w5500_miso = PC2_C;
const int gpio_w5500_mosi = PC1;
const int gpio_w5500_sck = PB13;
const int gpio_w5500_rstn = PC6;

// RGB LED
const int gpio_led_red = PA10;
const int gpio_led_green = PA8;
const int gpio_led_blue = PA9;

// Internal I2C
const int gpio_i2c_int_scl = PD12;
const int gpio_i2c_int_sda = PD13;

// Buzzer
const int gpio_buzzer = PB0;

// Buttons
const int gpio_button_left = PE10;
const int gpio_button_right = PE12;
const int gpio_button_up = PE9;
const int gpio_button_down = PE13;

#endif // INC_PINOUT_H
