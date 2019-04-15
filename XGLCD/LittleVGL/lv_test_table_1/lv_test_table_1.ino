/**
 * Create tables to test their functionalities
 */

#include "lv_xg.h"
#include "lvgl.h"

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
    
  static lv_style_t cell1_style;
  lv_style_copy(&cell1_style, &lv_style_plain);
  cell1_style.body.border.width = 1;
  cell1_style.line.width = 1;

  static lv_style_t cell_head_style;
  lv_style_copy(&cell_head_style, &lv_style_plain);
  cell_head_style.body.border.width = 1;
  cell_head_style.body.padding.ver = 20;
  cell_head_style.line.width = 1;
  cell_head_style.text.color = LV_COLOR_RED;
  cell_head_style.text.line_space = 0;
  cell_head_style.text.letter_space = 5;
  cell_head_style.text.letter_space = 3;

  /* Create a default object*/
  lv_obj_t * table1 = lv_table_create(lv_scr_act(), NULL);
  lv_obj_set_pos(table1, 10, 10);
  lv_table_set_style(table1, LV_TABLE_STYLE_CELL1, &cell1_style);
  lv_table_set_style(table1, LV_TABLE_STYLE_CELL2, &cell_head_style);
  lv_table_set_col_cnt(table1, 4);
  lv_table_set_row_cnt(table1, 5);

  /*Set the type 1 for the first row. Thye will use */
  lv_table_set_cell_type(table1, 0, 0, 2);
  lv_table_set_cell_type(table1, 0, 1, 2);
  lv_table_set_cell_type(table1, 0, 2, 2);
  lv_table_set_cell_type(table1, 0, 3, 2);

  lv_table_set_cell_value(table1, 0, 0, "First\nnew");

  lv_table_set_cell_value(table1, 0, 1, "Very long second");

  lv_table_set_cell_value(table1, 0, 2, "Center aligned third");
  lv_table_set_cell_align(table1, 0, 2, LV_LABEL_ALIGN_CENTER);

  lv_table_set_cell_value(table1, 0, 3, "Right aligned fourth ");
  lv_table_set_cell_align(table1, 0, 3, LV_LABEL_ALIGN_RIGHT);

  lv_table_set_cell_value(table1, 2, 2, "Merge ");
  lv_table_set_cell_merge_right(table1, 2, 2, true);


  lv_table_set_cell_value(table1, 3, 1, "Vert. center");

  lv_table_set_cell_value(table1, 3, 2, "Merge center\nin three\nrows");
  lv_table_set_cell_merge_right(table1, 3, 2, true);
  lv_table_set_cell_align(table1, 3, 2, LV_LABEL_ALIGN_CENTER);

  lv_table_set_cell_value(table1, 4, 2, "Merge right");
  lv_table_set_cell_merge_right(table1, 4, 2, true);
  lv_table_set_cell_align(table1, 4, 2, LV_LABEL_ALIGN_RIGHT);


  /*Add some extra rows*/
  lv_table_set_row_cnt(table1, lv_table_get_row_cnt(table1) + 2);
  lv_table_set_cell_value(table1, 6, 0, "Multiple merge");
  lv_table_set_cell_merge_right(table1, 6, 0, true);
  lv_table_set_cell_merge_right(table1, 6, 1, true);
  lv_table_set_cell_merge_right(table1, 6, 2, true);
}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}
