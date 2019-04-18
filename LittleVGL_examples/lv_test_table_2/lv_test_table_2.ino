/**
 * Create tables to test their functionalities
 */

#include "lv_xg.h"
#include "lvgl.h"

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
    
  lv_obj_t * page = lv_page_create(lv_scr_act(), NULL);
  lv_page_set_style(page, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
  lv_page_set_style(page, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);
  lv_page_set_scrl_fit(page, true, true);
  lv_obj_set_size(page, 200, 200);

  static lv_style_t cell_style;
  lv_style_copy(&cell_style, &lv_style_plain);
  cell_style.body.border.width = 1;

  /* Create a default object*/
  lv_obj_t * table1 = lv_table_create(page, NULL);
  lv_obj_set_pos(table1, 10, 10);
  lv_page_glue_obj(table1, true);
  lv_table_set_style(table1, LV_TABLE_STYLE_CELL1, &cell_style);
  lv_table_set_col_cnt(table1, 2);
  lv_table_set_row_cnt(table1, 8);
}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}
