/**
 * Test label long modes
 */

#include "lv_xg.h"
#include "lvgl.h"

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
    
  /* Test LV_LABEL_LONG_EXPAND (default)
   * GOAL: A label with a long line*/
  lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style(label1, &lv_style_plain_color);    /*Set a background to clearly see the label size*/
  lv_label_set_body_draw(label1, true);
  lv_label_set_text(label1, "This is a very long line which is not broken.");
  lv_label_set_long_mode(label1, LV_LABEL_LONG_EXPAND);

  /* LV_LABEL_LONG_BERAK (set width and test line break)
   * GOAL: the words are wrapped into multiple lines */
  lv_obj_t * label2 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_body_draw(label2, true);
  lv_obj_set_style(label2, &lv_style_plain_color);
  lv_label_set_text(label2, "This is a long line and a VeryVeryLongWordToWrap.\n"
                    "A new line and a lot of spaces:                        . Can you see them?");
  lv_label_set_long_mode(label2, LV_LABEL_LONG_BREAK);
  lv_obj_align(label2, label1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
  lv_obj_set_width(label2, 100);

  /* LV_LABEL_LONG_ROLL (set size and test rolling)
   * GOAL: the text is rolled in both directions*/
  lv_obj_t * label3 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_body_draw(label3, true);
  lv_obj_set_style(label3, &lv_style_plain_color);
  lv_label_set_text(label3, "Long line to roll!");
  lv_label_set_long_mode(label3, LV_LABEL_LONG_ROLL);
  lv_obj_align(label3, label2, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
  lv_obj_set_size(label3, 100, 50);

  /* LV_LABEL_LONG_SCROLL (create a parent and label on it)
   * GOAL: the text is scrolled in both directions */
  lv_obj_t * bg1 = lv_obj_create(lv_scr_act(), NULL);
  lv_obj_set_style(bg1, &lv_style_pretty);
  lv_obj_align(bg1, label3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

  lv_obj_t * label4 = lv_label_create(bg1, NULL);
  lv_obj_set_style(label4, &lv_style_plain_color);
  lv_label_set_body_draw(label4, true);
  lv_label_set_text(label4, "Long line to scroll!");
  lv_label_set_long_mode(label4, LV_LABEL_LONG_SCROLL);

  /* LV_LABEL_LONG_DOTS (set size and a long text)
   * GOAL: see dots at the end of the size */
  lv_obj_t * label5 = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style(label5, &lv_style_plain_color);
  lv_label_set_body_draw(label5, true);
  lv_label_set_long_mode(label5, LV_LABEL_LONG_DOT);
  lv_obj_set_size(label5, 100, 60);
#if LV_TXT_UTF8 == 0
  lv_label_set_text(label5, "Dots: abcdefghijklmnopqrs");
#else
  lv_label_set_text(label5, "Dots: aáeéiíoóuúAÁEÉIÍOÓUÚ");
#endif
  lv_obj_align(label5, bg1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

  /*Restore dots*/
  lv_obj_t * label6 = lv_label_create(lv_scr_act(), label5);
  lv_label_set_long_mode(label6, LV_LABEL_LONG_EXPAND);
  lv_obj_align(label6, label5, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}
