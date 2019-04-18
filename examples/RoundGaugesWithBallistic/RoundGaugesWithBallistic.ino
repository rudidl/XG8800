/*
ROUND GAUGE EXAMPLE with ballistic!
This example show how to create 3 round gauge that react like the real one with (almost) correct ballistic
The 3 gauges read analog values from XG_A0, XG_A1 & XG_A2 (check your CPU module for availability)
Created by S.U.M.O.T.O.Y - Max MC Costa
If you modify or get better result please let me know

Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>

XGLCD tft = XGLCD();

volatile int16_t curVal1 = 0;
volatile int16_t oldVal1 = 0;
volatile int16_t curVal2 = 0;
volatile int16_t oldVal2 = 0;
volatile int16_t curVal3 = 0;
volatile int16_t oldVal3 = 0;

void setup() {
  tft.begin();
  drawGauge(63, 63, 63);
  drawGauge(63 * 3 + 4, 63, 63);
  drawGauge(63 * 5 + 8, 63, 63);
}

void loop(void) {

#ifdef XG_A0
  curVal1 = map(analogRead(XG_A0), 0, 1023, 1, 254);
#else
  curVal1 = random(254); 
#endif
#ifdef XG_A1
  curVal2 = map(analogRead(XG_A1), 0, 1023, 1, 254);
#else
  curVal2 = random(254); 
#endif
#ifdef XG_A2
  curVal3 = map(analogRead(XG_A2), 0, 1023, 1, 254);
#else
  curVal3 = random(254); 
#endif

  if (oldVal1 != curVal1) {
    drawNeedle(curVal1, oldVal1, 63, 63, 63, COLOR_GREEN, COLOR_BLACK);
    oldVal1 = curVal1;
  }
  if (oldVal2 != curVal2) {
    drawNeedle(curVal2, oldVal2, 63 * 3 + 4, 63, 63, COLOR_CYAN, COLOR_BLACK);
    oldVal2 = curVal2;
  }
  if (oldVal3 != curVal3) {
    drawNeedle(curVal3, oldVal3, 63 * 5 + 8, 63, 63, COLOR_MAGENTA, COLOR_BLACK);
    oldVal3 = curVal3;
  }
}

void drawGauge(uint16_t x, uint16_t y, uint16_t r) {
  tft.drawCircle(x, y, r, COLOR_WHITE); //draw instrument container
  tft.roundGaugeTicker(x, y, r, 150, 390, 1.3, COLOR_WHITE); //draw major ticks
  if (r > 15) tft.roundGaugeTicker(x, y, r, 165, 375, 1.1, COLOR_WHITE); //draw minor ticks

}



void drawNeedle(int16_t val, int16_t oval, uint16_t x, uint16_t y, uint16_t r, uint16_t color, uint16_t bcolor) {
  uint16_t i;
  if (val > oval) {
    for (i = oval; i <= val; i++) {
      drawPointerHelper(i - 1, x, y, r, bcolor);
      drawPointerHelper(i, x, y, r, color);
      if ((val - oval) < (128)) delay(1);//ballistic
    }
  }
  else {
    for (i = oval; i > val; i--) {
      drawPointerHelper(i + 1, x, y, r, bcolor);
      drawPointerHelper(i, x, y, r, color);
      //ballistic
      if ((oval - val) >= 128) {
        delay(1);
      } else {
        delay(3);
      }
    }
  }
}

void drawPointerHelper(int16_t val, uint16_t x, uint16_t y, uint16_t r, uint16_t color) {
  float dsec, toSecX, toSecY;
  int16_t minValue = 0;
  int16_t maxValue = 255;
  float fromDegree = 150.0;//start
  float toDegree = 240.0;//end
  if (val > maxValue) val = maxValue;
  if (val < minValue) val = minValue;
  dsec = (((float)(val - minValue) / (float)(maxValue - minValue) * toDegree) + fromDegree) * (PI / 180.0);
  toSecX = cos(dsec) * (r / 1.35);
  toSecY = sin(dsec) * (r / 1.35);
  tft.drawLine(x, y, 1 + x + (int16_t)toSecX, 1 + y + (int16_t)toSecY, color);
  tft.fillCircle(x, y, 2, color);
}
