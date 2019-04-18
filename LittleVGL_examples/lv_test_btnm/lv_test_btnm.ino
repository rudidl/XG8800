/**
 * Create button matrixes to test their functionalities
 */
 
#include "lv_xg.h"
#include "lvgl.h"

#if LV_TXT_UTF8 != 0
static const char * btnm_map[] = {"One line", "\n", "\212", "\242Ina", "\204üŰöŐ", "\221éÉ", "\n", "\214", "\202Left", ""};
#else
static const char * btnm_map[] = {"One line", "\n", "\212", "\242Ina", "\204long", "\221short", "\n", "\214", "\202Left", ""};
#endif

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
    
  /* Default object
   * GOAL: A button matrix with default buttons */
  lv_obj_t * btnm1 = lv_btnm_create(lv_scr_act(), NULL);

  /* Test map, size and position. Also try some features.
   * GOAL: A button matrix with default buttons.  */
  static lv_style_t rel;
  lv_style_copy(&rel, &lv_style_btn_tgl_rel);
  rel.body.main_color = LV_COLOR_RED;
  rel.body.grad_color = LV_COLOR_BLACK;
  rel.text.color = LV_COLOR_YELLOW;

  static lv_style_t pr;
  lv_style_copy(&pr, &lv_style_btn_tgl_rel);
  pr.body.main_color = LV_COLOR_ORANGE;
  pr.body.grad_color = LV_COLOR_BLACK;
  pr.text.color = LV_COLOR_WHITE;


  lv_obj_t * btnm2 = lv_btnm_create(lv_scr_act(), NULL);
  lv_btnm_set_map(btnm2, btnm_map);
  lv_obj_set_size(btnm2, LV_HOR_RES / 2, LV_VER_RES / 3);
  lv_obj_align(btnm2, btnm1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
  lv_btnm_set_toggle(btnm2, true, 2);
  lv_btnm_set_action(btnm2, btnm_action);
  lv_btnm_set_style(btnm2, LV_BTNM_STYLE_BTN_REL, &rel);
  lv_btnm_set_style(btnm2, LV_BTNM_STYLE_BTN_PR, &pr);


  lv_obj_t * btnm3 = lv_btnm_create(lv_scr_act(), btnm2);
  lv_obj_align(btnm3, btnm1, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 20);
}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}

static lv_res_t btnm_action(lv_obj_t * btnm, const char * txt)
{
    (void) btnm; /*Unused*/

    printf("Key pressed: %s\n", txt);
    return LV_RES_OK;
}
