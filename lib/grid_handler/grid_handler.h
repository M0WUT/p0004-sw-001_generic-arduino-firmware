#ifndef GRID_HANDLER_H
#define GRID_HANDLER_H

#include "lvgl.h"
#include <Arduino.h>

#define MAX_GRID_CELLS 16

class GridHandler
{
public:
    GridHandler(lv_obj_t *obj, lv_obj_t **cells, int numCells);
    void set_cell_contents(int cellIndex, lv_obj_t *obj);
    lv_obj_t *get_cell(int cellIndex);
    const lv_font_t *get_max_size_font();
    void update_num_cells(int numCells);

private:
    lv_obj_t *_parentObj;
    void _create_max_size_grid();
    lv_obj_t **_cells;
    int _numCells;
    int _numRows;
    int _numCols;
    const lv_font_t *_maxSizes[4] = {&lv_font_montserrat_48, &lv_font_montserrat_34, &lv_font_montserrat_22                                                                                                                                                                                                                                    , &lv_font_montserrat_18};
};

#endif