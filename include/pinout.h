#ifndef INC_PINOUT_H
#define INC_PINOUT_H

#include <Arduino.h>

// DEBUG UART
constexpr int gpio_uart_debug_tx = PD5;
constexpr int gpio_uart_debug_rx = PD6;
constexpr int UART_DEBUG_BAUD = 115200;

// LCD
constexpr int gpio_tft_dc = PE1;
constexpr int gpio_tft_rst = PE0;
constexpr int gpio_tft_bl = PE5;
constexpr int gpio_tft_cs = PE4;
constexpr int gpio_tft_mosi = PE6;
constexpr int gpio_tft_miso = PE13; // Dummy pin
constexpr int gpio_tft_sclk = PE2;
#define TFT_EXPECTED_SPI_CONTROLLER SPI4
constexpr int TFT_PWM_FREQUENCY = 25000;

// W5500 (Ethernet PHY)
constexpr int gpio_w5500_cs = PB9;
constexpr int gpio_w5500_miso = PC2_C;
constexpr int gpio_w5500_mosi = PC1;
constexpr int gpio_w5500_sclk = PB13;
constexpr int gpio_w5500_rstn = PC6;

// RGB LED
constexpr int gpio_led_red = PA10;
constexpr int gpio_led_green = PA8;
constexpr int gpio_led_blue = PA9;

// Internal I2C
constexpr int gpio_i2c_int_scl = PD12;
constexpr int gpio_i2c_int_sda = PD13;
constexpr int int_eeprom_i2c_addr = 0x50;

// Buzzer
constexpr int gpio_buzzer = PB0;

// Buttons
constexpr int gpio_button_left = PE10;
constexpr int gpio_button_right = PE12;
constexpr int gpio_button_up = PE9;
constexpr int gpio_button_down = PE14;

#endif // INC_PINOUT_H
