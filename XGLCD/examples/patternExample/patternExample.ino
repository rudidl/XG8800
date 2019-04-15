/*
Just a basic example, an hello world example.

Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>
#include "pattern16x16.h"

XGLCD tft = XGLCD();

void setup()
{
  tft.begin();
}

void loop()
{
  tft.clearScreen(COLOR_BLUE);
  tft.writePattern(100, 100, _pattern16x16, 16, false); //this write a pattern 16x16 at 100,100
  tft.BTE_move(100, 100, 16, 16, 200, 200, 1, 1, true, 0xC5);
  delay(1000);
  tft.clearScreen(COLOR_BLUE);
  tft.writePattern(200, 200, _pattern16x16, 16, false);
  tft.BTE_move(200, 200, 16, 16, 300, 300, 1, 1, true, 0xC4);
  delay(1000);
}
