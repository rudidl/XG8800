/*
Simple test of BTE block move

Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>

XGLCD tft = XGLCD();

void setup() 
{
  tft.begin();
  
  //fill a gradient, so we can test BTEing stuff around the screen
  for(int i=0;i<tft.height();i++) tft.drawLine(0,i,tft.width()-1,i,tft.Color565(map(i,0,tft.height(),128,40), map(i,0,tft.height(),255,40), map(i,0,tft.height(),40,128)));

  tft.setTextColor(COLOR_PINK,COLOR_BLUE);
  tft.setCursor(30,30);
  tft.print("HELLO WORLD!");
  //The transparent-move option uses the foreground colour as the transparent colour.
  tft.setTextColor(COLOR_BLUE);
  tft.BTE_move(20,20,120,30,200,200,0,0,true);
}

void loop() 
{

}
