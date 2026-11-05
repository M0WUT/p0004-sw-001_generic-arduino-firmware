#include "lcd_screens.h"

void create_startup_screen(lv_obj_t *startup_screen)
{
    // Could do all sorts of clever things with layout
    // but this screen is very deterministic in layout
    // so magic numbers ftw!

    // Generic setup
    lv_obj_set_style_bg_color(startup_screen, M0WUT_DEFAULT_BACKGROUND, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(startup_screen, LV_OPA_COVER, LV_PART_MAIN);

    // M0WUT logo
    LV_IMAGE_DECLARE(logo);
    lv_obj_t *logo_img = lv_img_create(startup_screen);
    lv_img_set_src(logo_img, &logo);
    lv_obj_set_style_image_recolor(logo_img, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_image_recolor_opa(logo_img, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_align(logo_img, LV_ALIGN_TOP_LEFT, 7, 10);

    // GTE logo - white
    LV_IMAGE_DECLARE(gte_logo);
    lv_obj_t *gteLogoOrange = lv_img_create(startup_screen);
    lv_img_set_src(gteLogoOrange, &gte_logo);
    lv_obj_set_style_image_recolor(gteLogoOrange, M0WUT_DEFAULT_FOREGROUND, LV_PART_MAIN);
    lv_obj_set_style_image_recolor_opa(gteLogoOrange, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_align(gteLogoOrange, LV_ALIGN_TOP_RIGHT, -7, 10);

    // Logo is 115 pixels high plus 10 pixels offset from top
    // so we have 170-125 = 45 pixels spare

    const int version_y_offset = -25;

    lv_obj_t *version_label = lv_label_create(startup_screen);
    lv_label_set_text_fmt(version_label, "%s       %s", VERSION_STRING, BUILD_TIMESTAMP);
    lv_obj_align(version_label, LV_ALIGN_BOTTOM_MID, 0, version_y_offset);
    lv_obj_set_style_text_align(version_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(version_label, lv_color_white(), LV_PART_MAIN);
}