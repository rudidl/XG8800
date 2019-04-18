/* Simply Touch screen library calibration V2:

This will help you to calibrate your touch screen by modify
4 parameters inside theXGLCD/XGLCDProjectSettings. file:
  TOUCSRCAL_XLOW  //min value of x you can get
  TOUCSRCAL_XHIGH  //max value of x you can get
  TOUCSRCAL_YLOW //min value of y you can get
  TOUCSRCAL_YHIGH  //max value of y you can get
Normally those parameters are set as 0.

It's not a bullet-proof scientist alghorithm but calibrate
using this method will be fast and enough accurate for basic
touch screen operations like buttons, etc.

Now run this program and open Serial Monitor or follow screen instrunctions.
*/
#include <XG8800.h>

XGLCD tft = XGLCD();

const uint8_t samples = 20;
uint16_t tempData[samples][2];
volatile int acount = 0;
uint16_t tx, ty;//used as temp
uint16_t _XLOW_VAR;
uint16_t _YLOW_VAR;
uint16_t _XHIGH_VAR;
uint16_t _YHIGH_VAR;
bool proceed;
int scount = 0;

void setup() {
  Sbegin(38400);
  delay(2000);
  tft.begin();
  
  //tft.useINT(COLOR_INT);//We use generic int helper for Internal Resistive Touch
  tft.touchBegin();//enable touch support
  if (tft.touchCalibrated()) {//already calibrated?
    if (tft.width() > 480)  tft.setFontScale(1);
    Sprintln("You have old calibration data present!\n");
    Sprintln("\nPlease open XGLCD/XGLCDProjectSettings.h and put zero on the following:\n");
    Sprintln("#define TOUCSRCAL_XLOW  0");
    Sprintln("#define TOUCSRCAL_YLOW  0");
    Sprintln("#define TOUCSRCAL_XHIGH 0");
    Sprintln("#define TOUCSRCAL_YHIGH 0\n");
    Sprintln("Then save and try to run this again!");
    tft.setCursor(0, 0);
    tft.setTextColor(COLOR_RED);
    tft.println("---> You have old calibration data present! <---");
    tft.setTextColor(COLOR_WHITE);
    tft.println("Please open XGLCD/XGLCDProjectSettings.h");
    tft.println("and put zero on the following:");
    tft.println(" ");
    tft.println("   #define TOUCSRCAL_XLOW   0");
    tft.println("   #define TOUCSRCAL_YLOW   0");
    tft.println("   #define TOUCSRCAL_XHIGH  0");
    tft.println("   #define TOUCSRCAL_YHIGH  0");
    tft.println(" ");
    tft.print("Then save and try to run this again!");
    proceed = false;
  } else {
    Sprintln("Start calibration, please follow indications...\n");
    Sprintln("\nPlease press FIRMLY the TOP/LEFT corner of your screen now!\n");
    if (tft.width() > 480)  tft.setFontScale(1);
    tft.setCursor(CENTER, CENTER);
    tft.setTextColor(COLOR_WHITE);
    tft.print("Please press the TOP/LEFT corner now!");
    tft.fillCircle(5, 5, 5, COLOR_RED);
    proceed = true;
    //tft.enableISR(true);
    //You can avoid ISR by simple ignore the line above
    //it will use the slower digitalRead(pin) alternative internally
  }
}

void loop() {
  if (proceed) {
    if (tft.touched()) {
      if (acount >= samples) {
        tft.touchEnable(false);
        acount = 0;//reset counts
        uint32_t valx = 0;
        uint32_t valy = 0;

        if (scount < 1) {
          for (uint8_t i = 0; i < samples; i++) {
            valx += tempData[i][0];
            valy += tempData[i][1];
          }
          valx = valx / samples;
          valy = valy / samples;
          tft.fillWindow();
          if (tft.width() > 480)  tft.setFontScale(1);
          tft.setCursor(CENTER, CENTER);
          tft.setTextColor(COLOR_WHITE);
          tft.println("Top/Left done. Please do not touch screen...");
          tft.setTextColor(COLOR_RED);
          tft.setFontScale(1);
          tft.print("Please do not touch screen!");
          tft.setFontScale(0);
          Sprint("Top/Left done...");
          Sprint("Please do not touch screen...");
          _XLOW_VAR = valx;
          _YLOW_VAR = valy;
          delay(3000);
          tft.fillWindow();
          tft.fillCircle(tft.width() - 5, tft.height() - 5, 5, COLOR_RED);
          if (tft.width() > 480)  tft.setFontScale(1);
          tft.setCursor(CENTER, CENTER);
          tft.setTextColor(COLOR_WHITE);
          tft.print("ok, Now Touch Bottom/Right corner!");
          Sprintln("\n...done, Now Touch Bottom/Right corner!");
          delay(2000);
          tft.touchEnable(true);
          scount++;
        } else if (scount >= 1) {
          for (uint8_t i = 0; i < samples; i++) {
            valx += tempData[i][0];
            valy += tempData[i][1];
          }
          valx = valx / samples;
          valy = valy / samples;
          tft.fillWindow();
          tft.setCursor(0, 20);
          tft.setTextColor(COLOR_WHITE);
          tft.println("Calibration done...watch results!");
          Sprintln("\nCalibration done...watch results");
          _XHIGH_VAR = valx;
          _YHIGH_VAR = valy;
          tft.println("Now open file:");
          tft.setTextColor(COLOR_YELLOW);
          tft.println("XGLCD/XGLCDProjectSettings.h");
          tft.setTextColor(COLOR_WHITE);
          Sprintln("\nNow open file XGLCD/XGLCDProjectSettings.h\n");
          Sprintln("Change the following:\n");
          tft.println("Change the following:");
          tft.println(" ");
          tft.setTextColor(COLOR_GREEN);
          tft.print("#define TOUCSRCAL_XLOW   ");
          tft.setTextColor(COLOR_YELLOW);
          tft.println(_XLOW_VAR, DEC);
          tft.setTextColor(COLOR_GREEN);
          tft.print("#define TOUCSRCAL_YLOW   ");
          tft.setTextColor(COLOR_YELLOW);
          tft.println(_YLOW_VAR, DEC);
          tft.setTextColor(COLOR_GREEN);
          tft.print("#define TOUCSRCAL_XHIGH  ");
          tft.setTextColor(COLOR_YELLOW);
          tft.println(_XHIGH_VAR, DEC);
          tft.setTextColor(COLOR_GREEN);
          tft.print("#define TOUCSRCAL_YHIGH  ");
          tft.setTextColor(COLOR_YELLOW);
          tft.println(_YHIGH_VAR, DEC);
          tft.setTextColor(COLOR_WHITE);
          tft.println(" ");
          tft.println("...then save file and you are calibrated!");
          Sprintln("#define TOUCSRCAL_XLOW  0");
          Sprintln("#define TOUCSRCAL_YLOW  0");
          Sprintln("#define TOUCSRCAL_XHIGH 0");
          Sprintln("#define TOUCSRCAL_YHIGH 0");
          Sprintln("\nInto:\n");
          Sprint("#define TOUCSRCAL_XLOW  ");
          Sprintln2(_XLOW_VAR, DEC);
          Sprint("#define TOUCSRCAL_YLOW  ");
          Sprintln2(_YLOW_VAR, DEC);
          Sprint("#define TOUCSRCAL_XHIGH ");
          Sprintln2(_XHIGH_VAR, DEC);
          Sprint("#define TOUCSRCAL_YHIGH ");
          Sprintln2(_YHIGH_VAR, DEC);
          Sprintln("\nSave and Have a nice day!");
          proceed = false;
          tft.touchEnable(false);
        }
      } else {//continue get samples
        delay(1);
        tft.touchReadAdc(&tx, &ty);//we using 10bit adc data here
        if (acount >= 0) {
          tempData[acount][0] = tx;
          tempData[acount][1] = ty;
        }
        acount++;
      }
    }
  }//proceed
}
