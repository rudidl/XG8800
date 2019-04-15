#include <XG8800.h>
//#include "fonts/orbitron_16.c"

XGLCD tft = XGLCD();

void setup() 
{
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  tft.begin();
  //for (i=0; i<100; i++) pixels[i] = COLOR_GREEN;
  //tft.clearScreen(COLOR_BLACK);
  //tft.setCursor(CENTER, CENTER);
  //tft.setTextColor(COLOR_WHITE);
  //tft.setFontScale(1);
  //tft.setFont(&orbitron_16);
  //tft.setRotation(1);
  //tft.print("Blabla");
  //tft. setRotation(3);
  //tft.drawPixels(pixels, 99, 10, 50);
  //tft.setRotation(2);
  //tft.drawPixels(pixels, 79, 10, 50);
  //tft.setRotation(1);
  //tft.drawPixels(pixels, 59, 20, 60);
  //tft.setRotation(0);
  while(1) tft.drawPixel(100,100, COLOR_GREEN);
  //tft.drawPixels(pixels, 39, 20, 60);
  //tft.drawTriangle(100, 200, 150, 250, 180, 230, COLOR_GREEN);
  //tft.drawPixel(10,100, COLOR_GREEN);
  //tft.drawPixel(10,100, COLOR_GREEN);
  //tft.drawFastHLine(20, 50, 150, COLOR_GREEN);4
  //tft.drawFastVLine(20, 50, 100, COLOR_GREEN);
  //tft.drawArc(30,50,20,1,0,150,COLOR_GREEN);
  //while (1) tft.drawPixel(10,100, COLOR_GREEN);
  //while(1) {
  //tft.drawLine(100,200,150,250, COLOR_GREEN);
  //tft.drawLine(150,250,180,230, COLOR_GREEN);
  //}
}

void loop() 
{
  
}
