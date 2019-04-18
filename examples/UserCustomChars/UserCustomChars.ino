/*
An example of how easy it's upload custom 8x16 symbols on the RA8875.
which has 255 locations reserved for user custom designed symbols.

 
Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>

XGLCD tft = XGLCD();


//define some custom char (8x16)
const uint8_t battery1[16] = {
  0b00111000,
  0b00111000,
  0b11111111,
  0b10000001,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10000001,
  0b11111111
};

const uint8_t battery2[16] = {
  0b00111000,
  0b00111000,
  0b11111111,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10000001,
  0b11111111
};

const uint8_t battery3[16] = {
  0b00111000,
  0b00111000,
  0b11111111,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10000001,
  0b11111111
};

const uint8_t battery4[16] = {
  0b00111000,
  0b00111000,
  0b11111111,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10000001,
  0b11111111
};

const uint8_t battery5[16] = {
  0b00111000,
  0b00111000,
  0b11111111,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b11111111
};

//next 3 are part of the same char
const uint8_t battery6a[16] = {
  0b00000111,
  0b00000111,
  0b11111111,
  0b11000000,
  0b11000000,
  0b11000000,
  0b11110000,
  0b11111100,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b00000000,
  0b00000000
};

const uint8_t battery6b[16] = {
  0b11100000,
  0b11100000,
  0b11111111,
  0b00000011,
  0b00000011,
  0b00000011,
  0b00000011,
  0b00000011,
  0b00000011,
  0b11000011,
  0b11110011,
  0b11111111,
  0b11111111,
  0b11111111,
  0b00000000,
  0b00000000
};

const uint8_t battery6c[16] = {
  0b00011111,
  0b00000011,
  0b00001111,
  0b00000011,
  0b00011111,
  0b00000000,
  0b00011111,
  0b00000011,
  0b00001111,
  0b00000011,
  0b00011111,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000
};

void setup()
{

  uint8_t i;
  tft.begin();
  //upload chars in address 0x00 to 0x04
  tft.uploadUserChar(battery1, 0); //0x00
  tft.uploadUserChar(battery2, 1);
  tft.uploadUserChar(battery3, 2);
  tft.uploadUserChar(battery4, 3);
  tft.uploadUserChar(battery5, 4); //0x04
  tft.uploadUserChar(battery6a, 5); //
  tft.uploadUserChar(battery6b, 6); //
  tft.uploadUserChar(battery6c, 7); //
  //now custom char are stored in CGRAM
  tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
  tft.setFontScale(0);
  for (i = 0; i < 5; i++) {
    tft.setCursor(tft.width() / 2, tft.height() / 2);
    tft.showUserChar(i);//retrieve from 0x00 to 0x04
    delay(200);
  }
  tft.setFontScale(0);
}

uint8_t rot;
void loop()
{
  uint8_t i;
  tft.clearScreen();//exact as tft.fillScreen();
  tft.setRotation(rot);
  tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
  //custom char are managed as text but need to be called
  //by a special function
  //the only text parameter that is not accepted is setFontScale!
  for (i = 0; i < 5; i++) {
    tft.setCursor(10, 10);
    tft.showUserChar(i);//retrieve from 0x00 to 0x04
    delay(200);
  }

  //now another feature, you can design a char wider
  //by using many char slot combined.
  //below an example, 3 chars wider
  tft.setTextColor(COLOR_LIGHT_ORANGE);
  tft.setCursor(30, 11);
  tft.showUserChar(5, 2);
  delay(1000);
  if (rot > 3) rot = 0;
  rot++;
}
