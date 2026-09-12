#include "main.h"

StatusMonitor statusMonitor = StatusMonitor();
I2CBus i2c_internal = I2CBus(gpio_i2c_int_scl, gpio_i2c_int_sda);
lv_display_t *disp;
TFTHandler *tft;
Buzzer *buzzer;
// lv_style_t *largeTextStyle;
// SPIClass *spi;

// static lv_obj_t *channelViewCells[MAX_GRID_CELLS];
// GridHandler *gh;
// int GRID_SIZE = 12;
// const int READING_INTERVAL_MS = 1000;
// int previous_reading_time_ms = 0;
// int x = 1;
lv_obj_t *main_scr;
// lv_obj_t *rect;

EthernetHandler *ethernetHandler;

const int button_list[] = {gpio_button_left, gpio_button_right, gpio_button_up, gpio_button_down};

void setup()
{
  // TFT handler first so the LCD can be blanked immediately on startup
  tft = create_tft_handler(SPISettings(60000000, MSBFIRST, SPI_MODE0), gpio_tft_mosi, gpio_tft_miso, gpio_tft_sclk, gpio_tft_cs, gpio_tft_dc, gpio_tft_rst, gpio_tft_bl, tft_width, tft_height, tft_rotation);
  disp = tft->get_lvgl_display();

  // i2c_internal.scan();
  TMP102 tmp102 = TMP102(&i2c_internal, 0x48);
  float temperature = tmp102.read_temperature();
  DEBUG_PRINTF("Temperature: %.2f°C\n", temperature);
  EEPROM24AA256UID eeprom = EEPROM24AA256UID(&i2c_internal, 0x50);
  uint8_t mac[6];
  eeprom.read_mac_address(mac);

  buzzer = new Buzzer(gpio_buzzer);
  buzzer->play_startup_sound();

  ethernetHandler = new EthernetHandler();
  ethernetHandler->initialise();

  main_scr = create_home_screen();
  lv_screen_load(main_scr);

  lv_obj_t *label = lv_label_create(lv_screen_active());
  lv_label_set_text(label, "Hello world");
  lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  // Allow for all of the data on the LCD to be updated before we enable the backlight
  // Just looks bad if we see garbage on the screen
  for (int i = 0; i < 20; i++)
  {
    lv_task_handler();
  }

  for (int i = 0; i < sizeof(button_list) / sizeof(button_list[0]); i++)
  {
    pinMode(button_list[i], INPUT_PULLUP);
  }
}

void loop()
{
  lv_task_handler();
  ethernetHandler->tick();
}

//   tft = create_tft_handler(SPISettings(60000000, MSBFIRST, SPI_MODE0), gpio_tft_mosi, gpio_tft_miso, gpio_tft_sclk, gpio_tft_cs, gpio_tft_dc, gpio_tft_rst, tft_width, tft_height, tft_rotation);
//   disp = tft->get_lvgl_display();

//   main_scr = create_home_screen();
//   lv_screen_load(main_scr);

//   gh = new GridHandler(main_scr, channelViewCells, GRID_SIZE);

//   tft = create_tft_handler(SPISettings(60000000, MSBFIRST, SPI_MODE0), gpio_tft_mosi, gpio_tft_miso, gpio_tft_sclk, gpio_tft_cs, gpio_tft_dc, gpio_tft_rst, tft_width, tft_height, tft_rotation);
//   disp = tft->get_lvgl_display();

//   main_scr = create_home_screen();
//   lv_screen_load(main_scr);

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
