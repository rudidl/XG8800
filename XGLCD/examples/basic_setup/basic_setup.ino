/*
Explain the minimal setup and how to use instances...

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
  tft.print("Hello World!");
}

void loop() 
{

}
