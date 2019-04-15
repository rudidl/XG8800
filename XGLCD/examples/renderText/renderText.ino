/*
Another small example for render text.
This time shows the ability to use transparency.

Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>
#include "fonts/akashi_36.c"

XGLCD tft = XGLCD();

void setup()
{
  tft.begin();

  for (int i = 0; i < tft.width(); i++) {
    tft.drawFastVLine(i, 0, 110, tft.colorInterpolation(COLOR_RED, COLOR_PINK, i, tft.width()));
  }
  tft.setFont(&akashi_36);
  tft.setTextColor(COLOR_WHITE);//background transparent!!!
  tft.setCursor(CENTER, 0);
  if (tft.width(true) > 400){
    tft.setFontScale(2);
  } else {
    tft.setFontScale(1);
  }
  tft.println("XGRAPH");
}

void loop()
{

}
