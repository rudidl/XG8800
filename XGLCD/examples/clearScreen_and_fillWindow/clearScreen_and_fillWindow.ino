/*
fillWindow will fill the active window (set by setActiveWindow) with a chosen color. 
When you first start up, the active window is the whole screen so it acts as clearSceen.
This is the fastest way to clear the whole screen or a part of it!
The clearScreen command is a combination of setActiveWindow(whole)->fillScreen->setActiveWindow
so it's much slower, use it when you are using setActiveWindow.

Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>

XGLCD tft = XGLCD();

void setup()
{
  Sbegin(38400);
  delay(2000);
  Sprintln("XGLCD start");

  tft.begin();
  tft.clearScreen(COLOR_RED);
  tft.setActiveWindow(100,tft.width()-100,100,tft.height()-100);
  tft.fillWindow();
}

void loop()
{

}
