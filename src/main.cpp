#include "main.h"
#include <tft_handler_wrapper.h>
#include <lcd_screens.h>

lv_display_t *disp;
TFTHandler *tft;
lv_obj_t *main_scr;

ServiceTracker *serviceTracker;
TimeHelper timeHelper = TimeHelper();
DebugUartHelper debugUartHelper = DebugUartHelper(gpio_uart_debug_tx, gpio_uart_debug_rx, &timeHelper);
StatusHelper statusHelper = StatusHelper(&debugUartHelper, gpio_led_red, gpio_led_green, gpio_led_blue);
I2CBus i2c_internal = I2CBus(gpio_i2c_int_scl, gpio_i2c_int_sda);
EepromHelper eepromHelper = EepromHelper(EEPROM24AA256UID(&i2c_internal, int_eeprom_i2c_addr), &debugUartHelper);
ServiceContext serviceContext = {&timeHelper, &debugUartHelper, &eepromHelper, &statusHelper};

#include <stdio.h>
#include <string.h>
#include <math.h>

/* Display geometry */
#define DISP_W 320
#define DISP_H 170

#define SKY_W 170
#define SKY_H 170
#define SKY_CX 85
#define SKY_CY 85
#define SKY_R 70

#define PANEL_X 170
#define PANEL_W 150

/* Colours */
#define C_BG 0x08111D
#define C_PANEL 0x101E2D
#define C_GRID 0x294153
#define C_TEXT 0xD8E6F3
#define C_MUTED 0x7892A8
#define C_CYAN 0x38D9F5
#define C_GREEN 0x46E39A
#define C_YELLOW 0xFFFF00
#define C_ORANGE 0xFF8C00
#define C_RED 0xFF637D

/* Signal thresholds in dB-Hz */
#define CN0_GREEN 40.0f
#define CN0_YELLOW 30.0f
#define CN0_ORANGE 20.0f

static lv_obj_t *root;
static lv_obj_t *sky;
static lv_obj_t *panel;

static lv_obj_t *time_label;
static lv_obj_t *count_label;
static lv_obj_t *hdop_label;
static lv_obj_t *row_label[GNSS_TOP_SATS];

static gnss_data_t current;

static lv_color_t color(uint32_t hex)
{
  return lv_color_hex(hex);
}

/* ----------------------------------------------------------
 * Signal colour
 * ---------------------------------------------------------- */

static uint32_t gnss_signal_color(float cn0)
{
  if (!isfinite(cn0) || cn0 < CN0_ORANGE)
    return C_RED;

  if (cn0 < CN0_YELLOW)
    return C_ORANGE;

  if (cn0 < CN0_GREEN)
    return C_YELLOW;

  return C_GREEN;
}

/* ----------------------------------------------------------
 * Drawing helpers
 * ---------------------------------------------------------- */

static void draw_line(lv_layer_t *layer,
                      int x1, int y1,
                      int x2, int y2,
                      uint32_t col, int width)
{
  lv_draw_line_dsc_t d;
  lv_draw_line_dsc_init(&d);

  d.color = color(col);
  d.width = width;
  d.p1.x = x1;
  d.p1.y = y1;
  d.p2.x = x2;
  d.p2.y = y2;

  lv_draw_line(layer, &d);
}

static void draw_text(lv_layer_t *layer,
                      const char *txt,
                      int x, int y,
                      uint32_t col,
                      const lv_font_t *font,
                      int width)
{
  lv_draw_label_dsc_t d;
  lv_draw_label_dsc_init(&d);

  d.color = color(col);
  d.font = font;
  d.text = txt;

  lv_area_t area = {
      .x1 = x,
      .y1 = y,
      .x2 = x + width - 1,
      .y2 = y + 11};

  lv_draw_label(layer, &d, &area);
}

static void draw_circle(lv_layer_t *layer,
                        int cx, int cy,
                        int radius, uint32_t col)
{
  lv_draw_arc_dsc_t d;
  lv_draw_arc_dsc_init(&d);

  d.center.x = cx;
  d.center.y = cy;
  d.radius = radius;
  d.start_angle = 0;
  d.end_angle = 360;
  d.width = 1;
  d.color = color(col);
  d.opa = LV_OPA_COVER;

  lv_draw_arc(layer, &d);
}

/* Used satellite: filled dot */
static void draw_sat_dot(lv_layer_t *layer,
                         int x, int y, uint32_t col)
{
  lv_draw_arc_dsc_t d;
  lv_draw_arc_dsc_init(&d);

  d.center.x = x;
  d.center.y = y;
  d.radius = 3;
  d.start_angle = 0;
  d.end_angle = 360;
  d.width = 6;
  d.color = color(col);
  d.opa = LV_OPA_COVER;

  lv_draw_arc(layer, &d);
}

/* Tracked, unused satellite: cross */
static void draw_sat_cross(lv_layer_t *layer,
                           int x, int y, uint32_t col)
{
  draw_line(layer, x - 3, y - 3,
            x + 3, y + 3, col, 2);

  draw_line(layer, x - 3, y + 3,
            x + 3, y - 3, col, 2);
}

/* ----------------------------------------------------------
 * Sky plot drawing callback
 * ---------------------------------------------------------- */

static void sky_draw_cb(lv_event_t *e)
{
  if (lv_event_get_code(e) != LV_EVENT_DRAW_MAIN)
    return;

  lv_obj_t *obj = (lv_obj_t *)lv_event_get_current_target(e);
  lv_layer_t *layer = lv_event_get_layer(e);

  /*
   * The sky object is positioned at (0,0).
   * Drawing coordinates are absolute screen coordinates.
   */
  int ox = lv_obj_get_x(obj);
  int oy = lv_obj_get_y(obj);

  int cx = ox + SKY_CX;
  int cy = oy + SKY_CY;

  /* Horizon and elevation rings */
  draw_circle(layer, cx, cy, SKY_R, C_GRID);
  draw_circle(layer, cx, cy, SKY_R * 2 / 3, C_GRID);
  draw_circle(layer, cx, cy, SKY_R / 3, C_GRID);

  /* Cardinal axes */
  draw_line(layer, cx, cy - SKY_R,
            cx, cy + SKY_R, C_GRID, 1);

  draw_line(layer, cx - SKY_R, cy,
            cx + SKY_R, cy, C_GRID, 1);

  /* Cardinal labels */
  draw_text(layer, "N", cx - 4, cy - SKY_R - 12,
            C_CYAN, &lv_font_montserrat_10, 12);

  draw_text(layer, "S", cx - 4, cy + SKY_R + 2,
            C_CYAN, &lv_font_montserrat_10, 12);

  draw_text(layer, "W", cx - SKY_R - 12, cy - 5,
            C_CYAN, &lv_font_montserrat_10, 12);

  draw_text(layer, "E", cx + SKY_R + 3, cy - 5,
            C_CYAN, &lv_font_montserrat_10, 12);

  /* Satellite positions */
  int count = current.sat_count;

  if (count > GNSS_MAX_SATS)
    count = GNSS_MAX_SATS;

  for (int i = 0; i < count; i++)
  {
    const gnss_sat_t *s = &current.sats[i];

    if (!s->tracked)
      continue;

    if (!isfinite(s->azimuth) ||
        !isfinite(s->elevation))
      continue;

    if (s->elevation < 0.0f ||
        s->elevation > 90.0f)
      continue;

    /* Normalize azimuth to [0,360). */
    float azimuth = fmodf(s->azimuth, 360.0f);

    if (azimuth < 0.0f)
      azimuth += 360.0f;

    float az = azimuth * 0.01745329252f;

    /*
     * Elevation 90 degrees -> centre.
     * Elevation 0 degrees  -> horizon.
     */
    float r = (90.0f - s->elevation) / 90.0f * SKY_R;

    int x = cx + (int)lroundf(r * sinf(az));
    int y = cy - (int)lroundf(r * cosf(az));

    uint32_t col = gnss_signal_color(s->cn0);

    /* Shape represents usage, colour represents signal. */
    if (s->used)
      draw_sat_dot(layer, x, y, col);
    else
      draw_sat_cross(layer, x, y, col);

    /* PRN labels share the satellite's signal colour. */
    char prn[5];
    snprintf(prn, sizeof(prn), "%u", s->prn);

    /*
     * Keep labels inside the sky area when near edges.
     */
    int label_x = x + 5;

    if (label_x > 145)
      label_x = x - 15;

    if (label_x < 1)
      label_x = 1;

    int label_y = y - 5;

    if (label_y < 0)
      label_y = 0;

    if (label_y > 158)
      label_y = 158;

    draw_text(layer, prn, label_x, label_y,
              col, &lv_font_montserrat_10, 20);
  }
}

/* ----------------------------------------------------------
 * Label styling
 * ---------------------------------------------------------- */

static void style_label(lv_obj_t *obj, uint32_t col)
{
  lv_obj_set_style_text_color(obj, color(col), 0);
  lv_obj_set_style_text_font(
      obj, &lv_font_montserrat_10, 0);

  lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(obj, 0, 0);
  lv_obj_set_style_pad_all(obj, 0, 0);
}

/* ----------------------------------------------------------
 * Create UI
 * ---------------------------------------------------------- */

void gnss_tracker_create(void)
{
  memset(&current, 0, sizeof(current));

  root = lv_obj_create(lv_screen_active());

  lv_obj_set_pos(root, 0, 0);
  lv_obj_set_size(root, DISP_W, DISP_H);

  lv_obj_set_style_bg_color(root, color(C_BG), 0);
  lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(root, 0, 0);
  lv_obj_set_style_radius(root, 0, 0);
  lv_obj_set_style_pad_all(root, 0, 0);

  lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

  /* Sky plot: 170 x 170 */
  sky = lv_obj_create(root);

  lv_obj_set_pos(sky, 0, 0);
  lv_obj_set_size(sky, SKY_W, SKY_H);

  lv_obj_set_style_bg_color(sky, color(C_BG), 0);
  lv_obj_set_style_bg_opa(sky, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(sky, 0, 0);
  lv_obj_set_style_radius(sky, 0, 0);
  lv_obj_set_style_pad_all(sky, 0, 0);

  lv_obj_clear_flag(sky, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_add_event_cb(
      sky, sky_draw_cb, LV_EVENT_DRAW_MAIN, NULL);

  /* Right information panel: 150 x 170 */
  panel = lv_obj_create(root);

  lv_obj_set_pos(panel, PANEL_X, 0);
  lv_obj_set_size(panel, PANEL_W, DISP_H);

  lv_obj_set_style_bg_color(panel, color(C_PANEL), 0);
  lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(panel, 0, 0);
  lv_obj_set_style_radius(panel, 0, 0);
  lv_obj_set_style_pad_all(panel, 4, 0);

  lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

  /* UTC clock */
  time_label = lv_label_create(panel);
  lv_obj_set_pos(time_label, 2, 2);
  lv_label_set_text(time_label, "--:--:-- UTC");
  style_label(time_label, C_CYAN);

  /* Used / tracked counts */
  count_label = lv_label_create(panel);
  lv_obj_set_pos(count_label, 2, 16);
  lv_label_set_text(count_label, "Used: 00 Tracking: 00");
  style_label(count_label, C_TEXT);

  /* HDOP */
  hdop_label = lv_label_create(panel);
  lv_obj_set_pos(hdop_label, 2, 29);
  lv_label_set_text(hdop_label, "HDOP --.-");
  style_label(hdop_label, C_GREEN);

  /* Divider */
  lv_obj_t *sep = lv_obj_create(panel);

  lv_obj_set_pos(sep, 0, 43);
  lv_obj_set_size(sep, PANEL_W - 8, 1);

  lv_obj_set_style_bg_color(sep, color(C_GRID), 0);
  lv_obj_set_style_bg_opa(sep, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(sep, 0, 0);
  lv_obj_set_style_radius(sep, 0, 0);
  lv_obj_set_style_pad_all(sep, 0, 0);

  /* Top eight C/N0 entries */
  for (int i = 0; i < GNSS_TOP_SATS; i++)
  {
    row_label[i] = lv_label_create(panel);

    lv_obj_set_pos(row_label[i], 2, 46 + i * 15);
    lv_obj_set_width(row_label[i], PANEL_W - 10);

    lv_label_set_text(row_label[i], "--  --.- dBHz");

    style_label(row_label[i], C_MUTED);
  }

  lv_obj_invalidate(sky);
}

/* ----------------------------------------------------------
 * Sort tracked satellites by descending C/N0
 * ---------------------------------------------------------- */

static void sort_satellites(gnss_sat_t *out, int *count)
{
  *count = 0;

  int n = current.sat_count;

  if (n > GNSS_MAX_SATS)
    n = GNSS_MAX_SATS;

  for (int i = 0; i < n; i++)
  {
    const gnss_sat_t *s = &current.sats[i];

    if (!s->tracked)
      continue;

    if (!isfinite(s->cn0))
      continue;

    out[*count] = *s;
    (*count)++;
  }

  /* Descending signal strength */
  for (int i = 0; i < *count - 1; i++)
  {
    for (int j = i + 1; j < *count; j++)
    {
      if (out[j].cn0 > out[i].cn0)
      {
        gnss_sat_t tmp = out[i];
        out[i] = out[j];
        out[j] = tmp;
      }
    }
  }
}

/* ----------------------------------------------------------
 * Update from GNSS receiver
 * ---------------------------------------------------------- */

void gnss_tracker_update(const gnss_data_t *data)
{
  if (data == NULL || root == NULL)
    return;

  /*
   * Call from the LVGL thread.
   * If your GNSS parser runs in another task, transfer its
   * data safely to the LVGL task before calling this.
   */
  current = *data;

  if (current.sat_count > GNSS_MAX_SATS)
    current.sat_count = GNSS_MAX_SATS;

  /* Current UTC time */
  if (current.utc[0] != '\0')
  {
    lv_label_set_text_fmt(
        time_label, "%s UTC", current.utc);
  }
  else
  {
    lv_label_set_text(time_label, "--:--:-- UTC");
  }

  /* Count tracked satellites independently of used satellites */
  int tracked_count = 0;

  for (int i = 0; i < current.sat_count; i++)
  {
    if (current.sats[i].tracked)
      tracked_count++;
  }

  lv_label_set_text_fmt(
      count_label, "Used: %2u Tracking: %2d",
      current.used_count, tracked_count);

  /* HDOP */
  if (isfinite(current.hdop) && current.hdop >= 0.0f)
  {
    lv_label_set_text_fmt(
        hdop_label, "HDOP %.1f", current.hdop);

    uint32_t hdop_col;

    if (current.hdop < 2.0f)
      hdop_col = C_GREEN;
    else if (current.hdop < 5.0f)
      hdop_col = C_YELLOW;
    else
      hdop_col = C_RED;

    lv_obj_set_style_text_color(
        hdop_label, color(hdop_col), 0);
  }
  else
  {
    lv_label_set_text(hdop_label, "HDOP --.-");
    lv_obj_set_style_text_color(
        hdop_label, color(C_MUTED), 0);
  }

  /* Rank tracked satellites by C/N0 */
  gnss_sat_t sorted[GNSS_MAX_SATS];
  int n = 0;

  sort_satellites(sorted, &n);

  /* Refresh eight signal rows */
  for (int i = 0; i < GNSS_TOP_SATS; i++)
  {
    if (i < n)
    {
      lv_label_set_text_fmt(
          row_label[i],
          "%02u  %4.1f dBHz%s",
          sorted[i].prn,
          sorted[i].cn0,
          sorted[i].used ? "*" : "");

      lv_obj_set_style_text_color(
          row_label[i],
          color(gnss_signal_color(sorted[i].cn0)),
          0);
    }
    else
    {
      lv_label_set_text(
          row_label[i], "--  --.- dBHz");

      lv_obj_set_style_text_color(
          row_label[i], color(C_MUTED), 0);
    }
  }

  /* Redraw the sky plot with the latest satellite data */
  lv_obj_invalidate(sky);
}

void setup()
{
  tft = create_tft_handler(SPISettings(10000000, MSBFIRST, SPI_MODE0), gpio_tft_mosi, gpio_tft_miso, gpio_tft_sclk, gpio_tft_cs, gpio_tft_dc, gpio_tft_rst, gpio_tft_bl, tft_width, tft_height, tft_rotation);
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

  gnss_tracker_create();
  gnss_data_t d = {0};

  snprintf(d.utc, sizeof(d.utc), "20:31:45");

  d.sat_count = 8;
  d.used_count = 5;
  d.hdop = 1.2f;

  d.sats[0] = (gnss_sat_t){
      .prn = 3, .azimuth = 35, .elevation = 65, .cn0 = 43.0f, .tracked = true, .used = true};

  d.sats[1] = (gnss_sat_t){
      .prn = 8, .azimuth = 120, .elevation = 40, .cn0 = 36.0f, .tracked = true, .used = true};

  d.sats[2] = (gnss_sat_t){
      .prn = 14, .azimuth = 220, .elevation = 25, .cn0 = 28.0f, .tracked = true, .used = false};

  d.sats[3] = (gnss_sat_t){
      .prn = 18, .azimuth = 290, .elevation = 50, .cn0 = 19.0f, .tracked = true, .used = true};

  d.sats[4] = (gnss_sat_t){
      .prn = 22, .azimuth = 175, .elevation = 15, .cn0 = 32.0f, .tracked = true, .used = true};

  d.sats[5] = (gnss_sat_t){
      .prn = 27, .azimuth = 70, .elevation = 30, .cn0 = 24.0f, .tracked = true, .used = true};

  d.sats[6] = (gnss_sat_t){
      .prn = 31, .azimuth = 330, .elevation = 20, .cn0 = 41.0f, .tracked = true, .used = false};

  d.sats[7] = (gnss_sat_t){
      .prn = 6, .azimuth = 250, .elevation = 10, .cn0 = 17.0f, .tracked = true, .used = false};

  gnss_tracker_update(&d);
}

void loop()
{
  lv_task_handler();
  delay(5);
}
