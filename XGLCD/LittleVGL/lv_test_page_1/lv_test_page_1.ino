/**
 * Create pages to test their basic functionalities
 */

#include "lv_xg.h"
#include "lvgl.h"

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
    
  /*Create a page which should look well*/
  lv_obj_t * page1 = lv_page_create(lv_scr_act(), NULL);

  /*Resize the page*/
  lv_obj_t * page2 = lv_page_create(lv_scr_act(), NULL);
  lv_obj_set_size(page2, LV_DPI, LV_DPI * 2);
  lv_obj_align(page2, page1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

  /*Add some text to text the scrolling*/
  lv_obj_t * page3 = lv_page_create(lv_scr_act(), page2);
  lv_obj_set_size(page3, LV_DPI, LV_DPI * 2);
  lv_obj_align(page3, page2, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);

  lv_obj_t * label = lv_label_create(page3, NULL);
  lv_label_set_text(label, "First line of a text\n"
                    "Second line of a text\n"
                    "Third line of a text\n"
                    "Forth line of a text\n"
                    "Fifth line of a text\n"
                    "Sixth line of a text\n"
                    "Seventh line of a text\n"
                    "Eight line of a text\n"
                    "Ninth line of a text\n"
                    "Tenth line of a text\n");

  /*Enable horizontal fit to set scrolling in both directions*/
  lv_obj_t * page4 = lv_page_create(lv_scr_act(), page3);
  lv_obj_align(page4, page3, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
  lv_page_set_scrl_fit(page4, true, true);
  label = lv_label_create(page4, label);
}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}
