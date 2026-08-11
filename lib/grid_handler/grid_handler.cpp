#include "grid_handler.h"

GridHandler::GridHandler(lv_obj_t *obj, lv_obj_t **cells, int numCells)
{
  {
    _parentObj = obj;
    _cells = cells;
    _numCells = numCells;
    _create_max_size_grid();
  }
}

void GridHandler::_create_max_size_grid()
{
  lv_obj_update_layout(_parentObj);
  lv_coord_t width = lv_obj_get_width(_parentObj);
  lv_coord_t height = lv_obj_get_height(_parentObj);

  switch (_numCells)
  {
  case 1:
    _numRows = 1;
    _numCols = 1;
    break;
  case 2:
    _numRows = 2;
    _numCols = 1;
    break;
  case 3: // Deliberate fallthrough
  case 4:
    _numRows = 2;
    _numCols = 2;
    break;
  case 5: // Deliberate fallthrough
  case 6:
    _numRows = 3;
    _numCols = 2;
    break;
  case 7: // Deliberate fallthrough
  case 8: // Deliberate fallthrough
  case 9:
    _numRows = 3;
    _numCols = 3;
    break;
  case 10: // Deliberate fallthrough
  case 11: // Deliberate fallthrough
  case 12:
    _numRows = 4;
    _numCols = 3;
    break;
  default: // 13,14,15,16
    // This assumes you'll never want more than 16 items
    // If you do, feel free to extend this table
    _numRows = 4;
    _numCols = 4;
    break;
  };

  static int32_t rowDsc[MAX_GRID_CELLS];
  static int32_t colDsc[MAX_GRID_CELLS];

  // Grab all the padding parameters from the style
  int topPadding = lv_obj_get_style_pad_top(_parentObj, LV_PART_MAIN);
  int bottomPadding = lv_obj_get_style_pad_top(_parentObj, LV_PART_MAIN);
  int leftPadding = lv_obj_get_style_pad_left(_parentObj, LV_PART_MAIN);
  int rightPadding = lv_obj_get_style_pad_right(_parentObj, LV_PART_MAIN);
  int colPadding = lv_obj_get_style_pad_column(_parentObj, LV_PART_MAIN);
  int rowPadding = lv_obj_get_style_pad_row(_parentObj, LV_PART_MAIN);
  int outlinePadding = lv_obj_get_style_outline_pad(_parentObj, LV_PART_MAIN);
  int borderWidth = lv_obj_get_style_border_width(_parentObj, LV_PART_MAIN);

  int rowHeight = (height - topPadding - bottomPadding - 2 * (outlinePadding + borderWidth) - (rowPadding * (_numRows - 1))) / _numRows;
  int colWidth = (width - leftPadding - rightPadding - 2 * (outlinePadding + borderWidth) - (colPadding * (_numCols - 1))) / _numCols;

  for (int i = 0; i < _numRows; i++)
  {
    rowDsc[i] = rowHeight;
  }
  rowDsc[_numRows] = LV_GRID_TEMPLATE_LAST;

  for (int i = 0; i < _numCols; i++)
  {
    colDsc[i] = colWidth;
  }
  colDsc[_numCols] = LV_GRID_TEMPLATE_LAST;

  lv_obj_set_style_grid_column_dsc_array(_parentObj, colDsc, 0);
  lv_obj_set_style_grid_row_dsc_array(_parentObj, rowDsc, 0);
  lv_obj_set_layout(_parentObj, LV_LAYOUT_GRID);
}

void GridHandler::update_num_cells(int numCells)
{
  lv_obj_clean(_parentObj);
  _numCells = numCells;
  _create_max_size_grid();
}

void GridHandler::set_cell_contents(int cellIndex, lv_obj_t *obj)
{
  uint8_t col = cellIndex % _numCols;
  uint8_t row = cellIndex / _numCols;

  lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_CENTER, col, 1,
                       LV_GRID_ALIGN_CENTER, row, 1);

  _cells[cellIndex] = obj;
}

lv_obj_t *GridHandler::get_cell(int cellIndex)
{
  if (cellIndex >= _numCells)
  {
    return NULL;
  }

  return _cells[cellIndex];
}

const lv_font_t *GridHandler::get_max_size_font()
{
  return _maxSizes[_numCols - 1];
}