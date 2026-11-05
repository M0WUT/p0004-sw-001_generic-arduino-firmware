#include "main.h"
#include <tft_handler_wrapper.h>
#include <lcd_screens.h>

lv_display_t *disp;
TFTHandler *tft;

TimeHelper timeHelper = TimeHelper();
LoggingHelper loggingHelper = LoggingHelper(gpio_uart_debug_tx, gpio_uart_debug_rx, &timeHelper, gpio_led_red, gpio_led_green, gpio_led_blue, LOGGING_LEVEL);

HelperContext helperContext = {
    &timeHelper,
    &loggingHelper};

I2CBus i2c_internal = I2CBus(gpio_i2c_int_scl, gpio_i2c_int_sda);
EepromHelper eepromHelper = EepromHelper(EEPROM24AA256UID(&i2c_internal, int_eeprom_i2c_addr), &helperContext);
ServiceHelper serviceHelper = ServiceHelper(&helperContext);

ServiceContext serviceContext = {
    &timeHelper,
    &loggingHelper,
    &eepromHelper,
    &serviceHelper,
};

EthernetService ethernetService = EthernetService();
WebpageService webpageService = WebpageService();
BaseService *coreServiceList[] = {&ethernetService, &webpageService};
BaseService *extraServiceList[] = {};
lv_obj_t *startup_screen;

void setup()
{
  tft = create_tft_handler(SPISettings(60000000, MSBFIRST, SPI_MODE0), gpio_tft_mosi, gpio_tft_miso, gpio_tft_sclk, gpio_tft_cs, gpio_tft_dc, gpio_tft_rst, gpio_tft_bl, tft_width, tft_height, tft_rotation);
  disp = tft->get_lvgl_display();
  startup_screen = lv_obj_create(nullptr);
  create_startup_screen(startup_screen);
  lv_screen_load(startup_screen);

  // lv_obj_set_size(img1, 150, 150);

  // Allow for all of the data on the LCD to be updated before we enable the backlight
  // Just looks bad if we see garbage on the screen
  for (int i = 0; i < 20; i++)
  {
    lv_task_handler();
  }
  tft->set_lcd_brightness(255);

  serviceHelper.register_services(coreServiceList, sizeof(coreServiceList) / sizeof(coreServiceList[0]));
  serviceHelper.register_services(extraServiceList, sizeof(extraServiceList) / sizeof(extraServiceList[0]));

  lv_obj_t *init_label = lv_label_create(startup_screen);
  lv_label_set_text(init_label, "By Dan M0WUT");
  lv_obj_align(init_label, LV_ALIGN_BOTTOM_MID, 0, -7);
  lv_obj_set_style_text_align(init_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_color(init_label, lv_color_white(), LV_PART_MAIN);
  for (int i = 0; i < 500; i++)
  {
    lv_task_handler();
    delay(1);
  }

  BaseService **serviceList = serviceHelper.get_service_list();
  for (int i = 0; i < serviceHelper.get_num_services(); i++)
  {
    if (serviceList[i] == nullptr)
      continue;
    lv_label_set_text_fmt(init_label, "Starting %s service...", serviceList[i]->get_name());

    lv_task_handler();
    serviceList[i]->initialise(&serviceContext);
  }

  lv_label_set_text(init_label, "Initialisation complete");
  lv_task_handler();
}

void loop()
{
  serviceHelper.tick();
}
