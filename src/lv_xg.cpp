/*
 LittleVGL to XGLCD library for www.x-graph.be
 Interface between LittleVGL C library and XGLCD C++ library
 Copyright (c) 2019 DELCOMp bvba / UVee bvba
 2sd (a t) delcomp (d o t) com
 
 License:GNU General Public License v3.0
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "lvgl.h"
#include "XG8800.h"

XGLCD tft = XGLCD();

void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_array) {
  tft.drawBitmap(x1, y1, x2, y2, (uint16_t *)color_array);                    // copy 'color_array' to the specifed coordinates
  lv_flush_ready();                                                           // Tell the flushing is ready
}

void my_disp_map(int32_t x1,int32_t y1,int32_t x2,int32_t y2, const lv_color_t* color_p) {
  tft.drawBitmap(x1, y1, x2, y2, (uint16_t *)color_p);                        // Copy 'color_p' to the specified area
}

void my_disp_fill(int32_t x1,int32_t y1,int32_t x2,int32_t y2, lv_color_t color) {
  tft.fillRect(x1, y1, x2-x1+1, y2-y1+1, color.full);                         // Fill the specified area with 'color'
}

bool my_tp_read(lv_indev_data_t *data) {
  
  bool tp_is_pressed;
  uint32_t tx, ty;

  tp_is_pressed = tft.touchReadPixel(&tx, &ty);
    
  data->point.x = (uint16_t)tx;
  data->point.y = (uint16_t)ty;
  data->state = tp_is_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    
  return false;                                                               // Return false because no more to be read
}

void lv_xg_init() {

  lv_init();                                                                  // Init the LittleVGL library (not C++ library)
  
  tft.begin();                                                                // Start the LCD

  lv_disp_drv_t disp_drv;                                                     // Link the LCD to the LittleVGL library
  lv_disp_drv_init(&disp_drv);
  disp_drv.disp_flush = my_disp_flush;
  disp_drv.disp_fill = my_disp_fill;
  disp_drv.disp_map = my_disp_map;
  lv_disp_drv_register(&disp_drv);

  lv_indev_drv_t indev_drv;                                                   // Link the touchscreen to the LittleVGL library
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read = my_tp_read;
  lv_indev_drv_register(&indev_drv);
}
