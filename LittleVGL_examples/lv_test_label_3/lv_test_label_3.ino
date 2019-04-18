/**
 * Test text insert and cut
 */

#include "lv_xg.h"
#include "lvgl.h"

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
    
  /*Test inserting*/
  lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_pos(label1, 10, 10);
  lv_label_set_text(label1, "Test insert");
  lv_label_ins_text(label1, 4, " the");
  lv_label_ins_text(label1, 0, "I will ");
  lv_label_ins_text(label1, LV_LABEL_POS_LAST, " feature");

#if LV_TXT_UTF8
  lv_label_ins_text(label1, 7, "(UTF-8: aÁoÓ) ");
#endif

  lv_obj_t * label2 = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(label2, label1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
#if LV_TXT_UTF8 == 0
  lv_label_set_text(label2, "Characters to delete: abcd ABCD");
#else
  lv_label_set_text(label2, "Characters to delete: abcd aÁ uÚ üŰ");

#endif
  lv_label_cut_text(label2, 4, 5);
  lv_label_cut_text(label2, 21, 3);
}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}
