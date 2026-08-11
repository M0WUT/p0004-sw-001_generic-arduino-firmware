#include "lcd_screens.h"

lv_obj_t *create_home_screen()
{
    lv_obj_t *home_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(home_screen, M0WUT_DEFAULT_BACKGROUND, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(home_screen, LV_OPA_COVER, LV_PART_MAIN);
    return home_screen;

    // lv_obj_t *base_screen = lv_obj_create(NULL);
    // lv_obj_set_style_bg_color(base_screen, M0WUT_DEFAULT_BACKGROUND, LV_PART_MAIN);
    // lv_obj_set_style_bg_opa(base_screen, LV_OPA_COVER, LV_PART_MAIN);

    // lv_obj_set_flex_flow(base_screen, LV_FLEX_FLOW_COLUMN);

    // lv_obj_t *status_bar = lv_obj_create(base_screen);
    // lv_obj_set_size(status_bar, 320, 25);

    // lv_obj_t *main_screen = lv_obj_create(base_screen);
    // lv_obj_set_width(main_screen, tft_width);
    // lv_obj_set_flex_grow(main_screen, 1);

    // return main_screen;
}