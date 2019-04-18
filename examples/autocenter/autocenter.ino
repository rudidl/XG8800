/*
autocenter example:
setCursor has an optional parameter that let you precisely center
text regardless th text length.
The autocenter flag, automatically reset after each use!!!

Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>

XGLCD tft = XGLCD();

void setup()
{
  tft.begin();
}

void loop()
{
  tft.setRotation(0);
  
  tft.clearScreen();
  //normal method
  tft.drawRect(100, 100, 100, 100, COLOR_RED);//draw a rect
  tft.setCursor(150, 150);//set text cursor inside rect center
  tft.setFontScale(2);
  tft.print("AB");//print the text which will not appear in the center because the text has his length
  tft.setFontScale(0);
  tft.setCursor(CENTER, CENTER);
  tft.print("normal");
  delay(3000);
  
  tft.clearScreen();
  //autocenter method
  tft.drawRect(100, 100, 100, 100, COLOR_RED);//draw a rect
  tft.setCursor(150, 150, true);//set text cursor inside rect center and activate autocenter
  tft.setFontScale(2);
  tft.print("AB");//now the text is centered regardless of its length!
  tft.setFontScale(0);
  tft.setCursor(CENTER, CENTER);
  tft.print("centered");
  //autocenter it's now automatically disabled
  delay(3000);
}
