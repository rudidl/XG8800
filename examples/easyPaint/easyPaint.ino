/* 
Touch screen super-easy paint!
This version uses the new touch screen functions, much easier!
Did you already calibrate your screen? Better do that as soon as you can
Open the TouchScreenCalibration example and follow the instructions.
  
Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>

XGLCD tft = XGLCD();

uint32_t tx, ty;


void interface(){
  tft.fillRect(10,10,40,40,COLOR_WHITE);
  tft.fillRect(10+(40*1)+(10*1),10,40,40,COLOR_BLUE);
  tft.fillRect(10+(40*2)+(10*2),10,40,40,COLOR_RED);
  tft.fillRect(10+(40*3)+(10*3),10,40,40,COLOR_GREEN);
  tft.fillRect(10+(40*4)+(10*4),10,40,40,COLOR_CYAN);
  tft.fillRect(10+(40*5)+(10*5),10,40,40,COLOR_MAGENTA);
  tft.fillRect(10+(40*6)+(10*6),10,40,40,COLOR_YELLOW);
  tft.drawRect(10+(40*7)+(10*7),10,40,40,COLOR_WHITE);
}

void setup() 
{
  Sprintln("XGLCD start");

  Sbegin(38400);
  delay(2000);
  tft.begin();//initialize library
  tft.touchBegin();//enable Touch support!
  interface();
}

uint16_t choosenColor = 0;

void loop() 
{
  if (tft.touchReadPixel(&tx, &ty)) {
      Sprint(tx);
      Sprint("-");
      Sprintln(ty);
      if (ty >= 0 && ty <= 55){ //interface area
        if ((tx > 10 && tx < (10+40))){
          choosenColor = COLOR_WHITE;
          interface();
          tft.fillRect(10,10,40,40,COLOR_BLACK);
          tft.fillCircle(tft.width()-10,10,5,choosenColor);
        } 
        else if ((tx > 10+(40*1)+(10*1) && tx < 10+(40*2)+(10*1))){
          choosenColor = COLOR_BLUE;
          interface();
          tft.fillRect(10+(40*1)+(10*1),10,40,40,COLOR_BLACK);
          tft.fillCircle(tft.width()-10,10,5,choosenColor);
        } 
        else if ((tx > 10+(40*2)+(10*2) && tx < 10+(40*3)+(10*2))){
          choosenColor = COLOR_RED;  
          interface();
          tft.fillRect(10+(40*2)+(10*2),10,40,40,COLOR_BLACK);
          tft.fillCircle(tft.width()-10,10,5,choosenColor);
        } 
        else if ((tx > 10+(40*3)+(10*3) && tx < 10+(40*4)+(10*3))){
          choosenColor = COLOR_GREEN;  
          interface();
          tft.fillRect(10+(40*3)+(10*3),10,40,40,COLOR_BLACK);
          tft.fillCircle(tft.width()-10,10,5,choosenColor);
        } 
        else if ((tx > 10+(40*4)+(10*4) && tx < 10+(40*5)+(10*4))){
          choosenColor = COLOR_CYAN;  
          interface();
          tft.fillRect(10+(40*4)+(10*4),10,40,40,COLOR_BLACK);
          tft.fillCircle(tft.width()-10,10,5,choosenColor);
        } 
        else if ((tx > 10+(40*5)+(10*5) && tx < 10+(40*6)+(10*5))){
          choosenColor = COLOR_MAGENTA;  
          interface();
          tft.fillRect(10+(40*5)+(10*5),10,40,40,COLOR_BLACK);
          tft.fillCircle(tft.width()-10,10,5,choosenColor);
        } 
        else if ((tx > 10+(40*6)+(10*6) && tx < 10+(40*7)+(10*6))){
          choosenColor = COLOR_YELLOW;  
          interface();
          tft.fillRect(10+(40*6)+(10*6),10,40,40,COLOR_BLACK);
          tft.fillCircle(tft.width()-10,10,5,choosenColor);
        } 
        else if ((tx > 10+(40*7)+(10*7) && tx < 10+(40*8)+(10*7))){
          choosenColor = 0;  
          interface();
          tft.fillRect(0,52,tft.width()-1,tft.height()-53,COLOR_BLACK);
          tft.fillCircle(tft.width()-10,10,5,COLOR_BLACK);
        }
      } else { //paint
        //if (choosenColor != 0) tft.fillCircle(tx,ty,1,choosenColor);
        if (choosenColor != 0) tft.drawPixel(tx,ty,choosenColor);
      }
  }
}
