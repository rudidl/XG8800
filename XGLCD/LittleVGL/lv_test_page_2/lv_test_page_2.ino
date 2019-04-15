/**
 * Test styling, scrollbar modes, layout and action
 */

#include "lv_xg.h"
#include "lvgl.h"

static lv_res_t page_rel_action(lv_obj_t * page);

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
    
  static lv_style_t bg;
  static lv_style_t scrl;
  static lv_style_t sb;

  lv_style_copy(&bg, &lv_style_pretty);
  lv_style_copy(&scrl, &lv_style_pretty);
  lv_style_copy(&sb, &lv_style_pretty);

  bg.body.main_color = LV_COLOR_SILVER;
  bg.body.grad_color = LV_COLOR_GRAY;
  bg.body.padding.hor = 5;
  bg.body.padding.ver = 20;

  scrl.body.main_color = LV_COLOR_BLUE;
  scrl.body.grad_color = LV_COLOR_NAVY;
  scrl.body.padding.hor = 3;
  scrl.body.padding.ver = 3;
  scrl.body.shadow.width = 15;
  scrl.text.color = LV_COLOR_SILVER;

  sb.body.padding.hor = -10;    /*Out of the page*/
  sb.body.padding.ver = 10;
  sb.body.padding.inner = 10;
  sb.body.main_color = LV_COLOR_WHITE;
  sb.body.grad_color = LV_COLOR_WHITE;
  sb.body.opa = LV_OPA_70;

  /* Create a page with new style, layout, fit, action and scrollbar OFF*/
  lv_obj_t * page1 = lv_page_create(lv_scr_act(), NULL);
  lv_obj_set_size(page1, LV_DPI, LV_DPI * 2);
  lv_page_set_scrl_fit(page1, true, true);
  lv_page_set_scrl_layout(page1, LV_LAYOUT_COL_L);
  lv_page_set_sb_mode(page1, LV_SB_MODE_OFF);
  lv_page_set_rel_action(page1, page_rel_action);
  lv_page_set_style(page1, LV_PAGE_STYLE_BG, &bg);
  lv_page_set_style(page1, LV_PAGE_STYLE_SCRL, &scrl);
  lv_page_set_style(page1, LV_PAGE_STYLE_SB, &sb);

  lv_obj_t * label = lv_label_create(page1, NULL);
  lv_label_set_text(label, "First line of a text\n"
                    "Second line of a text\n"
                    "Third line of a text\n"
                    "Forth line of a text\n"
                    "Fifth line of a text\n");

  /*Copy 'page1' and set scrollbar ON*/
  lv_obj_t * page2 = lv_page_create(lv_scr_act(), page1);
  lv_obj_align(page2, page1, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
  label = lv_label_create(page2, label);
  lv_page_set_sb_mode(page2, LV_SB_MODE_ON);

  /*Copy 'page1' and set scrollbar AUTO*/
  lv_obj_t * page3 = lv_page_create(lv_scr_act(), page1);
  lv_obj_align(page3, page2, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
  label = lv_label_create(page3, label);
  lv_page_set_sb_mode(page3, LV_SB_MODE_AUTO);

  /*Copy 'page1' and set scrollbar DRAG*/
  lv_obj_t * page4 = lv_page_create(lv_scr_act(), page1);
  lv_obj_align(page4, page3, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
  label = lv_label_create(page4, label);
  lv_page_set_sb_mode(page4, LV_SB_MODE_DRAG);
}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}

static lv_res_t page_rel_action(lv_obj_t * page) {
  lv_obj_t * label = lv_label_create(page, NULL);
  lv_label_set_text(label, "First line of a text\n"
                    "Second line of a text\n"
                    "Third line of a text\n"
                    "Forth line of a text\n"
                    "Fifth line of a text\n");

  return LV_RES_OK;
}
