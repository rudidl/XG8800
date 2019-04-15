/*
This sketch demonstrate the difference between fillWindow and clearScreen
fillWindow depends directly on the activeWindow settings, after startup
these settings are automatically set as the entire window visible, so using
clearScreen instead fillWindow doesn't make sense (clearScreen is slower).
BUT if you set different parameters for the active window (setActiveWindow)
you will notice that fillWindow clears the image INSIDE the activeWindow only!
In that case it's useful to have a command that clear the entire window in a command.

Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>

XGLCD tft = XGLCD();

void setup() 
{
  tft.begin();
}


uint8_t alt = 0;

void loop() 
{
  tft.setActiveWindow(0,tft.width()-1,0,tft.height()-1);//this set the active window
  testdrawrects(COLOR_GREEN);
  delay(500);
  testfillrects(COLOR_YELLOW, COLOR_MAGENTA);
  delay(500);
  tft.setActiveWindow(80,tft.width()-80,80,tft.height()-80);//this set the active window
    tft.setCursor(CENTER,CENTER);
    tft.setTextColor(COLOR_WHITE,COLOR_BLUE);
  if (alt > 0){
    tft.print("[clearScreen]");
    delay(1000);
    tft.clearScreen();
  } else {
    tft.print("[fillScreen]");
    delay(1000);
    tft.fillWindow();
  }
  alt++;
  if (alt > 1) alt = 0;
  
}

void testdrawrects(uint16_t color) {
  for (int16_t x=0; x < tft.width(); x+=4) {
    tft.drawRect((tft.width()/2) - (x/2), (tft.height()/2) - (x/2) , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  for (int16_t x=tft.width()-1; x > 4; x-=4) {
    tft.fillRect((tft.width()/2) - (x/2), (tft.height()/2) - (x/2) , x, x, color1);
    tft.drawRect((tft.width()/2) - (x/2), (tft.height()/2) - (x/2) , x, x, color2);
  }
}

