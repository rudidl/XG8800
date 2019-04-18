/*
An example that uses the really useful colorInterpolation functions.
During library development I need a solid color interpolation function that
it's able to mix 2 different colors.
The first problem it's to unpack a 565 16bit color in 3x8bit colors,
not easy since the resulting colors missed some information so I first created Color565ToRGB
that it's not perfect (it's almost impossible reconstruct the exact 8 bit components) but a good approach.
Then it became easy to build an interpolation function.
If you want the maximum precision use the rgb one that it's also faster.

Make sure to select the correct LCD type in XGLCDProjectSettings.h
*/

#include <XG8800.h>

XGLCD tft = XGLCD();

void setup()
{
  tft.begin();
/*
The colorInterpolation it's quite simple and has 2 variants:
colorInterpolation(color1,color2,pos,divisions);
divisions:this is the amount of color division between the 2 colors
pos:this is the position between the mix, can go from 0 (color1) to divisions (color2)
There's another version that uses 8bit components for color1 and color2:
colorInterpolation(color1R,color1G,color1B,color2R,color2G,color2B,pos,divisions);
Has the same functionality but better precision and bit faster
 */
  for (int i = 0; i < tft.width(); i++) {
    tft.drawFastVLine(i, 0, 50, tft.colorInterpolation(COLOR_GREEN, COLOR_RED, i, tft.width()));
    tft.drawFastVLine(i, 50, 50, tft.colorInterpolation(COLOR_RED, COLOR_GREEN, i, tft.width()));
    tft.drawFastVLine(i, 100, 50, tft.colorInterpolation(COLOR_GRAYSCALE, COLOR_PINK, i, tft.width()));
    tft.drawFastVLine(i, 150, 50, tft.colorInterpolation(COLOR_YELLOW, COLOR_CYAN, i, tft.width()));
    tft.drawFastVLine(i, 200, 50, tft.colorInterpolation(23,0,127,64,127,0, i, tft.width()));//this one uses the rgb one
  }
}


void loop()
{


}
