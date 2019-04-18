/*
this sketch demostrate the ability to rotate the screen .

Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>

XGLCD tft = XGLCD();

void setup() 
{
  tft.begin();
}


uint8_t rot = 0;

void loop() 
{
  tft.fillWindow();
  tft.setRotation(rot);
  tft.setCursor(0,0);
  tft.print("[rotation ");
  tft.print(rot);
  tft.print("]");
  tft.setCursor(100,0);
  tft.print("txt shifted to X100");
  tft.setCursor(0,20);
  tft.print("txt shifted to Y20");
  tft.drawCircle(tft.width()/2,tft.height()/2,50,COLOR_GREEN);
  tft.drawRect((tft.width()/2)-50,(tft.height()/2)-25,100,50,COLOR_YELLOW);
  tft.drawEllipse(tft.width()/2,tft.height()/2,50,25,COLOR_CYAN);
  tft.drawTriangle((tft.width()/2)-50,(tft.height()/2)-25,tft.width()/2,(tft.height()/2)+25,(tft.width()/2)+50,(tft.height()/2)-25,COLOR_RED);
  tft.drawPixel(tft.width()/2,tft.height()/2,COLOR_WHITE);
  tft.drawFastVLine(tft.width()/2,(tft.height()/2)-60,60,COLOR_RED);
  tft.drawFastHLine((tft.width()/2)-60,tft.height()/2,60,COLOR_RED);
  delay(3000);
  if (rot > 3) rot = 0;
  rot++;
}


