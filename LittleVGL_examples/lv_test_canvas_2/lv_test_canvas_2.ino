/**
 * Create canvas to test its functionalities
 */

#include "lv_xg.h"
#include "lvgl.h"

void setup() {
  Serial.begin(38400);                                                        // Debugging info via Arduino Terminal = Serial debugging port

  lv_xg_init();                                                               // Init X-Graph LCD, LittleVGL library and the LittleVGL to X-Graph driver
    
  lv_color_t bg_color = LV_COLOR_MAKE(0xec, 0xf0, 0xf1);
  lv_color_t fill_color = LV_COLOR_MAKE(0xbc, 0xa0, 0xa1);

  /*Create a buffer for the canvas set its pixels to light blue*/
  static lv_color_t cbuf[sizeof(lv_color_t) * 100 * 80];
  uint32_t i;
  for(i = 0; i < sizeof(cbuf) / sizeof(cbuf[0]); i++) {
      cbuf[i] = bg_color;
  }

  /*Create the canvas object*/
  lv_obj_t * canvas = lv_canvas_create(lv_scr_act(), NULL);

  /*Assign the buffer to the canvas*/
  lv_canvas_set_buffer(canvas, cbuf, 100, 80, LV_IMG_CF_TRUE_COLOR);

  // a. Draw image
  LV_IMG_DECLARE(img_flower_icon);
  lv_canvas_copy_buf(canvas, img_flower_icon.data, img_flower_icon.header.w, img_flower_icon.header.h, 0, 0);

  // b. Draw complex objects
  lv_point_t points5[] = {
    {.x=10, .y=10},
    {.x=40, .y=10},
    {.x=30, .y=40},
    {.x=20, .y=40},
  };

  lv_canvas_draw_polygon(canvas, points5, 4, LV_COLOR_RED);
  lv_canvas_fill_polygon(canvas, points5, 4, LV_COLOR_RED, fill_color);

  lv_canvas_draw_circle(canvas, 25, 54, 10, LV_COLOR_BLACK);
  lv_canvas_flood_fill(canvas, 25, 54, bg_color, LV_COLOR_RED);

  lv_canvas_draw_circle(canvas, 25, 54, 5, LV_COLOR_BLACK);
  lv_canvas_draw_circle(canvas, 25, 54, 15, LV_COLOR_BLACK);

  // c. Draw polygon/fill
  lv_point_t points4[] = {
    {.x=10, .y=10},
    {.x=40, .y=10},
    {.x=30, .y=40},
    {.x=20, .y=40},
  };

  lv_canvas_draw_polygon(canvas, points4, 4, LV_COLOR_RED);
  lv_canvas_fill_polygon(canvas, points4, 4, LV_COLOR_RED, fill_color);

  // d. Draw rect/fill
  lv_point_t points3[4] = {
    {.x=10, .y=10},
    {.x=50, .y=10},
    {.x=50, .y=50},
    {.x=10, .y=50}
  };

  lv_canvas_draw_rect(canvas, points3, LV_COLOR_RED);
  lv_canvas_fill_polygon(canvas, points3, 4, LV_COLOR_RED, fill_color);

  // e. Draw triangle/fill
  lv_point_t points1[3] = {
    {.x=50, .y=10},
    {.x=30, .y=50},
    {.x=70, .y=50}
  };
  lv_canvas_draw_triangle(canvas, points1, LV_COLOR_RED);
  lv_canvas_fill_polygon(canvas, points1, 3, LV_COLOR_RED, fill_color);

  lv_point_t points2[3] = {
    {.x=80, .y=5},
    {.x=50, .y=30},
    {.x=99, .y=79}
  };
  lv_canvas_draw_triangle(canvas, points2, LV_COLOR_BLUE);
  lv_canvas_fill_polygon(canvas, points2, 3, LV_COLOR_BLUE, fill_color);

  // f. Draw lines
  lv_point_t point1 = {.x=50, .y=10}, point2 = {.x=30, .y=50}, point3 = {.x=70, .y=50};
  lv_canvas_draw_line(canvas, point1, point2, LV_COLOR_RED);
  lv_canvas_draw_line(canvas, point1, point3, LV_COLOR_RED);
  lv_canvas_draw_line(canvas, point3, point2, LV_COLOR_RED);
  lv_canvas_flood_fill(canvas, 50, 30, LV_COLOR_RED, bg_color);

  // h. Draw circle
  lv_canvas_draw_circle(canvas, 50, 40, 15, LV_COLOR_BLACK);
  lv_canvas_flood_fill(canvas, 50, 40, LV_COLOR_RED, bg_color);

  lv_canvas_draw_circle(canvas, 50, 40,  5, LV_COLOR_BLACK);
  lv_canvas_draw_circle(canvas, 50, 40, 10, LV_COLOR_BLACK);
  lv_canvas_draw_circle(canvas, 50, 40, 20, LV_COLOR_BLACK);
  lv_canvas_draw_circle(canvas, 50, 40, 25, LV_COLOR_BLACK);
}

void loop() {
  lv_task_handler();                                                          // Handle the GUI
  delay(5);                                                                   // Just wait a little
}
