/*
curve and ellipse test
Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>

XGLCD tft = XGLCD();

void setup()
{
   tft.begin();
}

uint8_t b = 0;
void loop()
{
  tft.setRotation(b);
  tft.drawEllipse(CENTER, CENTER, 100, 60, COLOR_PURPLE);
  tft.fillCurve(CENTER, CENTER, 80, 30, 0, COLOR_CYAN);
  tft.fillCurve(CENTER, CENTER, 80, 30, 1, COLOR_MAGENTA);
  tft.fillCurve(CENTER, CENTER, 80, 30, 2, COLOR_YELLOW);
  tft.fillCurve(CENTER, CENTER, 80, 30, 3, COLOR_RED);

  tft.drawCurve(CENTER, CENTER, 90, 50, 0, COLOR_CYAN);
  tft.drawCurve(CENTER, CENTER, 90, 50, 1, COLOR_MAGENTA);
  tft.drawCurve(CENTER, CENTER, 90, 50, 2, COLOR_YELLOW);
  tft.drawCurve(CENTER, CENTER, 90, 50, 3, COLOR_RED);
  tft.fillCircle(CENTER, CENTER, 30, COLOR_BLUE);
  delay(100);
  if (b > 2) {
    b = 0;
    tft.fillWindow();
  } else {
    b++;
  }
}
