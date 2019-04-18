/**
 * Open a terminal
 * @return pointer to the terminal window
 */

#include "lv_xg.h"
#include "lvgl.h"

#define TERMINAL_ANIM_TIME   100 /*[ms]*/
#define TERMINAL_NO_INPUT    0   /*Do not create Text area and Keyboard*/
#define TERMINAL_WIDTH       (LV_HOR_RES / 2)
#define TERMINAL_HEIGHT      (LV_VER_RES)
#define TERMINAL_LOG_LENGTH  512        /*Characters*/

void terminal_add(const char * txt_in);
static lv_res_t clr_click_action(lv_obj_t * btn);
static lv_res_t win_close_action(lv_obj_t * btn);

static lv_obj_t * win;
static char txt_log[TERMINAL_LOG_LENGTH + 1];
static lv_obj_t * label;
static lv_obj_t * clr_btn;

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
  
  static lv_style_t style_bg;
  lv_style_copy(&style_bg, &lv_style_pretty);
  style_bg.body.main_color = LV_COLOR_MAKE(0x30, 0x30, 0x30);
  style_bg.body.grad_color = LV_COLOR_MAKE(0x30, 0x30, 0x30);
  style_bg.body.border.color = LV_COLOR_WHITE;
  style_bg.text.color = LV_COLOR_MAKE(0xE0, 0xE0, 0xE0);

  win = lv_win_create(lv_scr_act(), NULL);
  lv_win_set_style(win, LV_WIN_STYLE_BG, &style_bg);
  lv_obj_set_size(win, TERMINAL_WIDTH, TERMINAL_HEIGHT);
  lv_win_set_sb_mode(win, LV_SB_MODE_AUTO);
  lv_win_add_btn(win, SYMBOL_CLOSE, win_close_action);

  /*Make the window's content responsive*/
  lv_win_set_layout(win, LV_LAYOUT_PRETTY);

  /*Create a label for the text of the terminal*/
  label = lv_label_create(win, NULL);
  lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(label, lv_win_get_width(win));
  lv_label_set_static_text(label, txt_log);               /*Use the text array directly*/

  /*Create a clear button*/
  clr_btn = lv_btn_create(win, NULL);
  lv_cont_set_fit(clr_btn, true, true);
  lv_btn_set_action(clr_btn, LV_BTN_ACTION_CLICK, clr_click_action);
  lv_obj_t * btn_label = lv_label_create(clr_btn, NULL);
  lv_label_set_text(btn_label, "Clear");

  //return win;
}

void loop() {
char c;
  lv_task_handler();                                                          // Handle the GUI
  if (Serial.available()) {
    c = Serial.read();
    terminal_add((const char*)&c);
  }
  delay(5);                                                                   // Just wait a little
}

/**
 * Add data to the terminal
 * @param txt_in character sting to add to the terminal
 */
void terminal_add(const char * txt_in)
{
    if(win == NULL) return;                 /*Check if the window is exists*/

    uint16_t txt_len = strlen(txt_in);
    uint16_t old_len = strlen(txt_log);

    /*If the data is longer then the terminal ax size show the last part of data*/
    if(txt_len > TERMINAL_LOG_LENGTH) {
        txt_in += (txt_len - TERMINAL_LOG_LENGTH);
        txt_len = TERMINAL_LOG_LENGTH;
        old_len = 0;
    }
    /*If the text become too long 'forget' the oldest lines*/
    else if(old_len + txt_len > TERMINAL_LOG_LENGTH) {
        uint16_t new_start;
        for(new_start = 0; new_start < old_len; new_start++) {
            if(txt_log[new_start] == '\n') {
                /*If there is enough space break*/
                if(new_start >= txt_len) {
                    /*Ignore line breaks*/
                    while(txt_log[new_start] == '\n' || txt_log[new_start] == '\r') new_start++;
                    break;
                }
            }
        }

        /* If it wasn't able to make enough space on line breaks
         * simply forget the oldest characters*/
        if(new_start == old_len) {
            new_start = old_len - (TERMINAL_LOG_LENGTH - txt_len);
        }
        /*Move the remaining text to the beginning*/
        uint16_t j;
        for(j = new_start; j < old_len; j++) {
            txt_log[j - new_start] = txt_log[j];
        }
        old_len = old_len - new_start;
        txt_log[old_len] = '\0';

    }

    memcpy(&txt_log[old_len], txt_in, txt_len);
    txt_log[old_len + txt_len] = '\0';

    lv_label_set_static_text(label, txt_log);
    lv_win_focus(win, clr_btn, TERMINAL_ANIM_TIME);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Called when the Clear button is click to clear the text of the terminal
 * @param btn pointer to the clear button
 * @return LV_ACTION_RES_OK because the button is not deleted
 */
static lv_res_t clr_click_action(lv_obj_t * btn)
{
    (void) btn;    /*Unused*/

    txt_log[0] = '\0';
    lv_label_set_static_text(label, txt_log);   /*Refresh the text*/

    return LV_RES_OK;
}

/**
 * Called when the window's close button is clicked
 * @param btn pointer to the close button
 * @return LV_ACTION_RES_INV because the button is deleted in the function
 */
static lv_res_t win_close_action(lv_obj_t * btn)
{
    (void) btn;    /*Unused*/

    lv_obj_del(win);
    win = NULL;
    return LV_RES_INV;
}
