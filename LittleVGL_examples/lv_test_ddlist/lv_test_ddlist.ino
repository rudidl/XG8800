/**
 * Create drop down lists to test their functionalities
 */

#include "lv_xg.h"
#include "lvgl.h"

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
    
  /* Create a default object*/
  lv_obj_t * ddlist1 = lv_ddlist_create(lv_scr_act(), NULL);
  lv_obj_set_pos(ddlist1, 10, 10);

  /* Create a drop down list with a lot of options, fix height and anim time.
   * Open it by default without animation and assign an action*/
  lv_obj_t * ddlist2 = lv_ddlist_create(lv_scr_act(), NULL);
  lv_obj_align(ddlist2, ddlist1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
  lv_ddlist_set_options(ddlist2, "First\nSecond\nThird\nForth\nFifth\nSixth");
  lv_ddlist_set_fix_height(ddlist2, LV_DPI);
  lv_ddlist_set_selected(ddlist2, 2);
  lv_ddlist_set_anim_time(ddlist2, 100);
  lv_ddlist_open(ddlist2, false);
  lv_ddlist_set_hor_fit(ddlist2, false);
  lv_ddlist_set_action(ddlist2, ddlist_action);
  lv_obj_set_width(ddlist2, LV_DPI * 2);

  /*Copy the previous drop down list and modify its style*/
  static lv_style_t ddlist3_style;
  lv_style_copy(&ddlist3_style, &lv_style_pretty);
  ddlist3_style.body.main_color = LV_COLOR_GRAY;
  ddlist3_style.body.grad_color = LV_COLOR_BLACK;
  ddlist3_style.body.padding.hor = 20;
  ddlist3_style.body.padding.ver = 30;

  ddlist3_style.text.color = LV_COLOR_RED;
  ddlist3_style.text.letter_space = 5;
  ddlist3_style.text.line_space = 15;

  lv_obj_t * ddlist3 = lv_ddlist_create(lv_scr_act(), ddlist2);
  lv_obj_align(ddlist3, ddlist2, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
  lv_ddlist_set_style(ddlist3, LV_DDLIST_STYLE_BG, &ddlist3_style);
  lv_ddlist_set_style(ddlist3, LV_DDLIST_STYLE_SEL, &lv_style_plain_color);
}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}

static lv_res_t ddlist_action(lv_obj_t * ddlist)
{
    char buf[64];
    lv_ddlist_get_selected_str(ddlist, buf);
    printf("New option selected on a drop down list: %s\n", buf);

    return LV_RES_OK;
}
