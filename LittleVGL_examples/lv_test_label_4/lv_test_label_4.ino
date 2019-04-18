/**
 * Test mixed features
 */

#include "lv_xg.h"
#include "lvgl.h"

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
    
  /* Create a label with '\r', '\n', '\r\n' and '\n\r' line breaks
   * GOAL: The text in 5 lines without empty lines*/
  lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label1, "Line1\n"
                    "Line2\r"
                    "Line3\r\n"
                    "Line4");

  /* Test recoloring
   * GOAL: the word "red" is red*/
  lv_obj_t * label3 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label3, "This is a #ff0000 red# word");
  lv_label_set_recolor(label3, true);
  lv_obj_align(label3, label1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

#if LV_TXT_UTF8 != 0
  /* Test UTF-8 support with LV_LABEL_LONG_BREAK, new lines and recolor
   * GOAL: the word "red" is red*/
  lv_obj_t * label4 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label4, "Normal ASCII\n"
                    "UTF-8 letters:áÁééőŐöÖúÚűŰ\n"
                    "Recolor UTF-8: #ff0000 öŐ##00ff00 üŰ##0000ff éÉ#");
  lv_label_set_recolor(label4, true);
  lv_label_set_long_mode(label4, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(label4, 100);
  lv_obj_align(label4, label3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

#endif
}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}
