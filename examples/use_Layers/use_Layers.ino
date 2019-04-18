/*
An ultra-simple sketch to test Layers.

Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>

XGLCD tft = XGLCD();

void setup() {
  Sbegin(38400);
  delay(2000);
  tft.begin();
  tft.useLayers(true);
  Sprintln(F("Turn ON layers (automatically)"));
  tft.writeTo(L1);//If layers off it will turn on
  //remember to turn off or layers remain active forever!
  Sprintln(F("Writing on Layer1 a magenta rect"));
  tft.fillRect(0, 0 , 100, 100, COLOR_MAGENTA);
  Sprintln(F("Tells that magenta it's a Transparent color"));
  tft.setTransparentColor(COLOR_MAGENTA);
  Sprintln(F("Writing on Layer2 a circle..."));
  tft.writeTo(L2);
  tft.drawCircle(100, 100, 100, COLOR_GREEN);
  Sprintln(F("Show LAYER1"));
  tft.layerEffect(LAYER1);
  delay(1000);
  Sprintln(F("Show LAYER2"));
  tft.layerEffect(LAYER2);
  delay(1000);
  Sprintln(F("Apply Lighten"));
  tft.layerEffect(LIGHTEN);
  delay(1000);
  Sprintln(F("Apply OR"));
  tft.layerEffect(OR);
  delay(1000);
  Sprintln(F("Apply Transparent"));
  tft.layerEffect(TRANSPARENT);
  delay(1000);
  Sprintln(F("Clear current layer"));
  tft.clearMemory();
  delay(1000);
  Sprintln(F("Turn OFF layers"));
  tft.useLayers(false);//turn off layers
  delay(1000);
  Sprintln(F("Clear the remaining memory"));
  tft.clearMemory();
}

void loop() {
}
