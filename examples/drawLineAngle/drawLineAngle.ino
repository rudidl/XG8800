/*
example of drawLineAngle

Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>

XGLCD tft = XGLCD();

void setup()
{

  tft.begin();


  tft.drawCircle(tft.width() / 2, tft.height() / 2, 101, 0xFFFF);//draw round gauge
  tft.drawCircle(tft.width() / 2, tft.height() / 2, 103, 0xFFFF);//draw round gauge
}

void loop()
{
  for (int i = 0; i <= 360; i++) {
    if (i > 0) tft.drawLineAngle(tft.width() / 2, tft.height() / 2, i - 1, 100, 0x0000);//erase previous needle
    tft.drawLineAngle(tft.width() / 2, tft.height() / 2, i, 100, 0xFFFF);//draw needle
    delay(10);
  }
}
