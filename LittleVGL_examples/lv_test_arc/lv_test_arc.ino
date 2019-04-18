 /**
 * Create arcs to test their functionalities
 */

#include "lv_xg.h"
#include "lvgl.h"

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
    
  /* Create a default object*/
  lv_obj_t * arc1 = lv_arc_create(lv_scr_act(), NULL);
  lv_obj_set_pos(arc1, 10, 10);

  /* Modify size, position and angles*/
  lv_obj_t * arc2 = lv_arc_create(lv_scr_act(), NULL);
  lv_obj_set_size(arc2, 100, 100);
  lv_obj_align(arc2, arc1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
  lv_arc_set_angles(arc2, 0, 250);

  /* Copy 'arc2' and set a new style for it */
  static lv_style_t style1;
  lv_style_copy(&style1, &lv_style_plain);
  style1.line.color = LV_COLOR_RED;
  style1.line.width = 8;
  lv_obj_t * arc3 = lv_arc_create(lv_scr_act(), arc2);
  lv_obj_set_style(arc3, &style1);
  lv_obj_align(arc3, arc2, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);

}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}
