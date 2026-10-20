#include "main.h"
#include <tft_handler_wrapper.h>
#include <lcd_screens.h>

lv_display_t *disp;
TFTHandler *tft;
lv_obj_t *main_scr;

ServiceTracker *serviceTracker;
TimeHelper timeHelper = TimeHelper();
LoggingHelper loggingHelper = LoggingHelper(gpio_uart_debug_tx, gpio_uart_debug_rx, &timeHelper, gpio_led_red, gpio_led_green, gpio_led_blue, LOGGING_LEVEL);

HelperContext helperContext = {
    &timeHelper,
    &loggingHelper};

I2CBus i2c_internal = I2CBus(gpio_i2c_int_scl, gpio_i2c_int_sda);
EepromHelper eepromHelper = EepromHelper(EEPROM24AA256UID(&i2c_internal, int_eeprom_i2c_addr), &helperContext);
ServiceContext serviceContext = {
    &timeHelper,
    &loggingHelper,
    &eepromHelper,
};

void setup()
{
  tft = create_tft_handler(SPISettings(60000000, MSBFIRST, SPI_MODE0), gpio_tft_mosi, gpio_tft_miso, gpio_tft_sclk, gpio_tft_cs, gpio_tft_dc, gpio_tft_rst, gpio_tft_bl, tft_width, tft_height, tft_rotation);
  disp = tft->get_lvgl_display();
  main_scr = create_home_screen();
  lv_screen_load(main_scr);
  // Allow for all of the data on the LCD to be updated before we enable the backlight
  // Just looks bad if we see garbage on the screen
  for (int i = 0; i < 20; i++)
  {
    lv_task_handler();
  }
  tft->set_lcd_brightness(255);

  serviceTracker = new ServiceTracker(NULL, 0);
  serviceTracker->initialiseServices(&serviceContext);
}

void loop()
{
  lv_task_handler();
  delay(5);
}
