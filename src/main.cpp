#include "main.h"

StatusMonitor statusMonitor = StatusMonitor();
I2CBus i2c_internal = I2CBus(gpio_i2c_int_scl, gpio_i2c_int_sda);
// lv_display_t *disp;
// TFTHandler *tft;
// lv_style_t *largeTextStyle;
// SPIClass *spi;

// static lv_obj_t *channelViewCells[MAX_GRID_CELLS];
// GridHandler *gh;
// int GRID_SIZE = 12;
// const int READING_INTERVAL_MS = 1000;
// int previous_reading_time_ms = 0;
// int x = 1;
// lv_obj_t *main_scr;
// lv_obj_t *rect;

// EthernetHandler *ethernetHandler;

void setup()
{
  // i2c_internal.scan();
  TMP102 tmp102 = TMP102(&i2c_internal, 0x48);
  float temperature = tmp102.read_temperature();
  DEBUG_PRINTF("Temperature: %.2f°C\n", temperature);
  EEPROM24AA256UID eeprom = EEPROM24AA256UID(&i2c_internal, 0x50);
  uint8_t mac[6];
  eeprom.read_mac_address(mac);
}

void loop() { ; }

//   tft = create_tft_handler(SPISettings(60000000, MSBFIRST, SPI_MODE0), gpio_tft_mosi, gpio_tft_miso, gpio_tft_sclk, gpio_tft_cs, gpio_tft_dc, gpio_tft_rst, tft_width, tft_height, tft_rotation);
//   disp = tft->get_lvgl_display();

//   main_scr = create_home_screen();
//   lv_screen_load(main_scr);

//   gh = new GridHandler(main_scr, channelViewCells, GRID_SIZE);

//   // lv_obj_t *spinner = lv_spinner_create(lv_screen_active());
//   // lv_obj_set_size(spinner, 240, 240);
//   // lv_obj_center(spinner);
//   // lv_spinner_set_anim_params(spinner, 1000, 200);

//   ethernetHandler = new EthernetHandler();
//   ethernetHandler->initialise();
// }

// void loop()
// {
//   lv_task_handler();
//   ethernetHandler->tick();

//   if (millis() - previous_reading_time_ms > READING_INTERVAL_MS)
//   {
//     gh->update_num_cells(x);
//     for (int i = 0; i < x; i++)
//     {

//       lv_obj_t *label = lv_label_create(main_scr);
//       lv_label_set_text_fmt(label, "%hd.25°C", -111 * ((uint16_t)i + 1));
//       lv_obj_set_style_text_color(label, m0wut_channel_colours[i % m0wut_num_colours], LV_PART_MAIN);
//       const lv_font_t *font = gh->get_max_size_font();
//       lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
//       lv_obj_center(label);
//       gh->set_cell_contents(i, label);
//     }

//     previous_reading_time_ms = millis();
//     x %= GRID_SIZE;
//     x += 1;
//   }
// }
