#ifndef LCD_SCREENS_H
#define LCD_SCREENS_H

#include <lvgl.h>
#include "colours.h"
#include "lcd_properties.h"
#include "logo.h"
#include "gte_logo.h"
#include "version.h"

void create_startup_screen(lv_obj_t *startup_screen, lv_subject_t *init_progress_percent);

#endif
