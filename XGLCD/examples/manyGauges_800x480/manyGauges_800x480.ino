/*
ROUND GAUGE EXAMPLE with ballistic! only 800x480
This example showx how to create 3 round gaugex that react like the real one with (almost) correct ballistic
The 3 gauges read analog values from A0,A1 & A2
It's slow since the ballistic uses a delay, a better example will be posted soon
Created by S.U.M.O.T.O.Y - Max MC Costa
If you modify or get better result please let me know

Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>
#include <math.h>

XGLCD tft = XGLCD();

volatile int16_t curVal[6] = { -1, -1, -1, -1, -1, -1};
volatile int16_t oldVal[6] = {0, 0, 0, 0, 0, 0};
const int16_t posx[6] = {63, 193, 323, 453, 583, 713};
const int16_t posy[6] = {63, 63, 63, 63, 63, 63};
const int16_t radius[6] = {63, 63, 63, 63, 63, 63};
//const uint8_t analogIn[6] = {A0, A1, A2, A3, A8, A9};
const uint16_t needleColors[6] = {COLOR_GREEN, COLOR_CYAN, COLOR_MAGENTA, COLOR_YELLOW, COLOR_LIGHT_ORANGE, COLOR_RED};
const uint8_t degreesVal[6][2] = {
  {150, 240},
  {150, 240},
  {150, 240},
  {150, 240},
  {150, 240},
  {150, 240},
};

void setup() {
  tft.begin();
  for (uint8_t i = 0; i < 6; i++) {
    drawGauge(posx[i], posy[i], radius[i]);
  }
}

void loop(void) {

  for (uint8_t i = 0; i < 6; i++) {
    curVal[i] = random(255);
    drawNeedle(i, COLOR_BLACK);
  }
  delay(10);
}


void drawGauge(uint16_t x, uint16_t y, uint16_t r) {
  tft.drawCircle(x, y, r, COLOR_WHITE); //draw instrument container
  tft.roundGaugeTicker(x, y, r, 150, 390, 1.3, COLOR_WHITE); //draw major ticks
  if (r > 15) tft.roundGaugeTicker(x, y, r, 165, 375, 1.1, COLOR_WHITE); //draw minor ticks
}



void drawNeedle(uint8_t index, uint16_t bcolor) {
  uint16_t i;
  if (curVal[index] > 255) return;//curVal[index] = 255;
  if (curVal[index] <   0) return;//curVal[index] = 0;
  if (oldVal[index] != curVal[index]) {
    if (curVal[index] > oldVal[index]) {
      for (i = oldVal[index]; i <= curVal[index]; i++) {
        if (i > 0) drawPointerHelper(index, i - 1, posx[index], posy[index], radius[index], bcolor);
        drawPointerHelper(index, i, posx[index], posy[index], radius[index], needleColors[index]);
        if ((curVal[index] - oldVal[index]) < (128)) delay(1);//ballistic
      }
    }
    else {
      for (i = oldVal[index]; i > curVal[index]; i--) {
        drawPointerHelper(index, i + 1, posx[index], posy[index], radius[index], bcolor);
        drawPointerHelper(index, i, posx[index], posy[index], radius[index], needleColors[index]);
        //ballistic
        if ((oldVal[index] - curVal[index]) >= 128) {
          delay(1);
        } else {
          delay(2);
        }
      }
    }
    oldVal[index] = curVal[index];
  }//val != oldval
}

void drawPointerHelper(uint8_t index, int16_t val, uint16_t x, uint16_t y, uint16_t r, uint16_t color) {
  float dsec;
  const int16_t minValue = 0;
  const int16_t maxValue = 255;
  dsec = (((float)(uint16_t)(val - minValue) / (float)(uint16_t)(maxValue - minValue) * degreesVal[index][1]) + degreesVal[index][0]) * (PI / 180);
  uint16_t w = (uint16_t)(1 + x + (cos(dsec) * (r / 1.35)));
  uint16_t h = (uint16_t)(1 + y + (sin(dsec) * (r / 1.35)));
  tft.drawLine(x, y, w, h, color);
  tft.fillCircle(x, y, 2, color);
}

