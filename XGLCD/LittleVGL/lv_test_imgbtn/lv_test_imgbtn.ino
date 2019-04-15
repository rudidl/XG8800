#include "lv_xg.h"
#include "lvgl.h"

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
    
  /* Create an image button and set images for it*/
  lv_obj_t * imgbtn1 = lv_imgbtn_create(lv_scr_act(), NULL);
  lv_obj_set_pos(imgbtn1, 10, 10);
  lv_imgbtn_set_toggle(imgbtn1, true);

  LV_IMG_DECLARE(imgbtn_img_1);
  LV_IMG_DECLARE(imgbtn_img_2);
  LV_IMG_DECLARE(imgbtn_img_3);
  LV_IMG_DECLARE(imgbtn_img_4);

  lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_REL, &imgbtn_img_1);
  lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_PR, &imgbtn_img_2);
  lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_TGL_REL, &imgbtn_img_3);
  lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_TGL_PR, &imgbtn_img_4);

  lv_imgbtn_set_action(imgbtn1, LV_BTN_ACTION_CLICK, imgbtn_clicked);

  /*Add a label*/
  lv_obj_t * label = lv_label_create(imgbtn1, NULL);
  lv_label_set_text(label, "Button 1");

  /*Copy the image button*/
  lv_obj_t * imgbtn2 = lv_imgbtn_create(lv_scr_act(), imgbtn1);
  lv_imgbtn_set_state(imgbtn2, LV_BTN_STATE_TGL_REL);
  lv_obj_align(imgbtn2, imgbtn1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

  label = lv_label_create(imgbtn2, NULL);
  lv_label_set_text(label, "Button 2");
}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}

lv_res_t imgbtn_clicked(lv_obj_t * imgbtn)
{
    (void) imgbtn; /*Unused*/

    printf("Clicked\n");

    return LV_RES_OK;
}
