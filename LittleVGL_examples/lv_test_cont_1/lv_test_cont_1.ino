/**
 * Create containers to test their  basic functionalities
 */

#include "lv_xg.h"
#include "lvgl.h"

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
    
  /* Create a default object*/
  lv_obj_t * cont1 = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_pos(cont1, 10, 10);
  lv_cont_set_style(cont1, &lv_style_pretty);

  /*Test fit wit adding two labels*/
  lv_obj_t * cont2 = lv_cont_create(lv_scr_act(), cont1);
  lv_cont_set_fit(cont2, true, true);

  lv_obj_t * obj2_1 = lv_label_create(cont2, NULL);
  lv_label_set_text(obj2_1, "Short");

  lv_obj_t * obj2_2 = lv_label_create(cont2, NULL);
  lv_label_set_text(obj2_2, "A longer text");
  lv_obj_set_pos(obj2_2, 80, 30);

  lv_obj_align(cont2, cont1, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);

  /*Test layout and fit togother*/
  lv_obj_t * cont3 = lv_cont_create(lv_scr_act(), cont2);
  lv_label_create(cont3, obj2_1);
  lv_label_create(cont3, obj2_2);
  lv_cont_set_layout(cont3, LV_LAYOUT_COL_L);

  lv_obj_align(cont3, cont2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);


  /*Set a new style with new padding*/
  static lv_style_t cont4_style;
  lv_style_copy(&cont4_style, &lv_style_pretty_color);
  cont4_style.body.padding.hor = 20;
  cont4_style.body.padding.ver = 40;
  cont4_style.body.padding.inner = 1;

  lv_obj_t * cont4 = lv_cont_create(lv_scr_act(), cont3);
  lv_label_create(cont4, obj2_1);
  lv_label_create(cont4, obj2_2);
  lv_cont_set_style(cont4, &cont4_style);

  lv_obj_align(cont4, cont3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}
