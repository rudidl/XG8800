/**
 * Create labels with dynamic, static and array texts
 */
 
#include "lv_xg.h"
#include "lvgl.h"

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
    
  /* Default object*/
  lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);

  /* Set label text to "I'm testing\nthe labels" */
  lv_obj_t * label2 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label2, "I'm testing\nthe labels");
  lv_obj_align(label2, label1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

  /* Set a static array as text and modify a letter later (Goal is "STATic text")*/
  static char label_static_text[] =  {"static text"};
  lv_obj_t * label3 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_static_text(label3, label_static_text);
  lv_obj_align(label3, label2, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
  label_static_text[0] = 'S';         /*Randomly modify letters*/
  label_static_text[1] = 'T';
  label_static_text[2] = 'A';
  label_static_text[3] = 'T';
  lv_label_set_text(label3, NULL);    /*Refresh after modification*/

  /* Set text from array*/
  char array_text[3]; /*Not static to see the text will remain after this variable is destroyed*/
  array_text[0] = 'a';
  array_text[1] = 'b';
  array_text[2] = 'c';    /*Not need to be '\0' terminated*/
  lv_obj_t * label4 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_array_text(label4, array_text, sizeof(array_text));
  lv_obj_align(label4, label3, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);

  /* Copy 'label2' (dynamic) and set style and background*/
  lv_obj_t * label5 = lv_label_create(lv_scr_act(), label2);
  lv_obj_set_style(label5, &lv_style_pretty_color);
  lv_obj_align(label5, label2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
  lv_label_set_body_draw(label5, true);

  /* Copy 'label3' (static) and set style and background*/
  lv_obj_t * label6 = lv_label_create(lv_scr_act(), label3);
  lv_obj_set_style(label6, &lv_style_pretty_color);
  lv_obj_align(label6, label3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
  lv_label_set_body_draw(label6, true);

  /* Copy 'label4' (array) and set style and background*/
  lv_obj_t * label7 = lv_label_create(lv_scr_act(), label4);
  lv_obj_set_style(label7, &lv_style_pretty_color);
  lv_obj_align(label7, label4, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
  lv_label_set_body_draw(label7, true);
}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}
