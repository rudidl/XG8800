/*
A benchmark test - measure the speed of many drawing functions of XGLCD library at any rotation
Open your serial monitor for results

Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>
#include <SdFat.h>
#ifdef LCD_DRIVER_ILI9341
#include "fonts/orbitron_16.c"
#endif

XGLCD tft = XGLCD();
SdFat SD;

#define DELAY_BETWEEN 500

void setup() {
  Sbegin(38400);
  delay(2000);
  tft.begin();
  SD.begin(XG_SD_CS);
#ifdef LCD_DRIVER_ILI9341
  tft.setFont(&orbitron_16);
#endif
}

uint8_t rot = 0;
unsigned long timecnt;
  void loop(void) {

  timecnt = millis();
  test(rot);
  tft.clearScreen();
  tft.setCursor(CENTER,CENTER);
  tft.setTextColor(COLOR_WHITE);
  tft.setFontScale(1);
  timecnt = millis() - timecnt;
  tft.print(timecnt);
  //tft.print(" msec");
  delay(2000);
  rot++;
  if (rot > 3) rot = 0;
}

