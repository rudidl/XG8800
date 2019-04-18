/*
This shows the capabilities of the library text engine that is
extremely flexible and simple! All commands are the same for all type of text
(INT,EXT or Render ones) and things like println works too.
You can use all the print() options (like print(var,HEX), etc. Take a look the print command 
in the arduino guide!) 

Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>
#include "fonts/akashi_36.c"//minipixel

XGLCD tft = XGLCD();

void setup()
{
  tft.begin();
  tft.setRotation(0);//works at any rotation as well
  tft.setFont(&akashi_36);
  tft.setFontScale(3);
  tft.setCursor(CENTER,0);
  tft.setTextColor(COLOR_GREEN);//notice that! After gradient text will be this color!
  //gradient it's one shot, text color will be reset as the one chosen by setTextColor
  tft.setTextGradient(COLOR_RED,COLOR_CYAN);//works also with rendered text!
  tft.println("CD 0123");
  tft.println("ABCD");//notice that correctly goes in another line
  tft.setFont();
  tft.setTextColor(COLOR_WHITE);//Force white
  tft.println("this is the internal font.");//notice that correctly goes in another line too!
}

void loop()
{

}
