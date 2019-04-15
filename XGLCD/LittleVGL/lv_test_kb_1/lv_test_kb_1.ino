/**
 * Create a default object and test the basic functions
 */

#include "lv_xg.h"
#include "lvgl.h"

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver

  lv_obj_t * ta = lv_ta_create(lv_scr_act(), NULL);
  lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_MID, 0, 30);

  /* Default object*/
  lv_obj_t * kb1 = lv_kb_create(lv_scr_act(), NULL);
  lv_obj_align(kb1, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  lv_kb_set_ta(kb1, ta);
}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}
