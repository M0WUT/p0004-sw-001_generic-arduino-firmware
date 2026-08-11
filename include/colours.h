#ifndef INC_COLOURS_H
#define INC_COLOURS_H

#include <lvgl.h>

#define M0WUT_DEFAULT_BACKGROUND lv_color_hex(0x080808)
#define M0WUT_DEFAULT_FOREGROUND lv_color_hex(0xe1970c)

const lv_color_t m0wut_channel_colours[] = {
    M0WUT_DEFAULT_FOREGROUND,
    lv_palette_main(LV_PALETTE_LIGHT_GREEN),
    lv_palette_main(LV_PALETTE_LIGHT_BLUE),
    lv_palette_main(LV_PALETTE_PINK),
    lv_palette_main(LV_PALETTE_YELLOW),
    lv_palette_main(LV_PALETTE_PURPLE),
    lv_palette_main(LV_PALETTE_BROWN),
    lv_palette_main(LV_PALETTE_INDIGO),

};

const int m0wut_num_colours = sizeof(m0wut_channel_colours) / sizeof(m0wut_channel_colours[0]);

#endif // INC_COLOURS_H