/*
Grab bmp image from an sd card.
It reads column by column and send each RA instead of pixels
Look inside the folder XGLCD/examples/SDTest
there's a folder, copy the content in a formatted FAT32 SD card

IMPORTANT: Not all XG8800 compatible CPU modules work with the standard
Arduino SD library. This example uses the SDFat library of Bill Greyman 
(on which the Arduino standard SD Library is based).
Installation instructions:
- Close the Arduino IDE
- Visit https://github.com/greiman/SdFat and download the ZIP repository
- Unzip the downloaded file
- Copy the SDFat folder in the unzipped directory to the Arduino libraries folder
- Re-open the Arduino IDE

Make sure to select the correct LCD type in XGLCDProjectSettings.h

*/

/* How hardware dependancy works
 *  
- SdFat class uses a fast custom SPI hardware SPI implementation (SDFatLibSPI = Arduino SPI / SDFatSoftSPI = Soft SPI)
- SD_SPI_CONFIGURATION = 0 (default) -> the SdFat / SdSpi class is enabled.
  This uses begin/beginTransaction/endTransaction/receive/end functions which are hardware dependant:
- src/SDSpicard: several hardware dependant files which are condition compiled (only the correct one is compiled)
- SdSpiCard.cpp is used for all hardware versions:
  - begin = *spi bus / CS pin / Clock Divisor (which are used for configuring the selected hardware)
    (divisor = 2 = default in sdfat.h)
- Default SPI values are different from RA8875: MSBFIRST / SPI_MODE0 / Divisor = 2 (30MHZ !!!!) 

*/

// BUG in SDFAT libraries when used with Particle/Redbear Duo
//1. Open SdFat.cpp
//2. Find #if defined(ARDUINO) || defined(DOXYGEN)
//3. Change to #if (defined(ARDUINO) || defined(DOXYGEN)) && !defined(PLATFORM_ID)
//4. Save SdFat.cpp
//5. Open Syscall.cpp
//6. Find:
//#if defined(ARDUINO)
//#include <Arduino.h>
//#include <SPI.h>
//#elif defined(PLATFORM_ID)  // Only defined if a Particle device
//#include "application.h"
//7. Change:
//#if defined(PLATFORM_ID)  // Only defined if a Particle device
//#include "application.h"
//#elif defined(ARDUINO)
//#include <Arduino.h>
//#include <SPI.h>
//8: Find:
//#elif defined(ARDUINO)
//inline void SysCall::yield() {
//  // Use the external Arduino yield() function.
//  ::yield();
//
//#elif defined(PLATFORM_ID)  // Only defined if a Particle device
//inline void SysCall::yield() {
//    Particle.process();
//}
//9: Change:
//#elif defined(ARDUINO)
//inline void SysCall::yield() {
//  // Use the external Arduino yield() function.
//  ::yield();
//}

/* SPI library changes for BLE Nano
- SdFatConfig.h: SD_HAS_CUSTOM_SPI: module added
- new file: SdSpinRF51822.cpp
- SdSpi.h: SDSpiLib: disabled
- Syscall.h: yield() for nRF51822 added + extra ARDUINO check (on top)
- arduinostream.h: add ARDUINO_FILE_USES_STREAM iso ENABLE_ARDUINO_FEATURES
- sdfatconfig.h: ARDUINO_FILE_USES_STREAM between #ifndef so it can be disable on project base
- sdfatconfig.h: top file: a bunch of defines/undefs only for the BLE Nano
 */
#include <XG8800.h>
#include <SdFat.h>

#define BUFFPIXEL 20//I've experiment a little with this but not so many differences

//instances
XGLCD tft = XGLCD();
SdFat SD;
File bmpFile;

void setup()
{
  Sbegin(38400);
  delay(2000);
  
  tft.begin();        // Must be before SD.begin because it inits SD_CS
  tft.setRotation(0);
  
  if (!SD.begin(XG_SD_CS)) {
    Sprintln("SD failed!");
    return;
  }
  Sprintln("OK!");
}

void loop()
{
  bmpDraw("UVee.bmp", 0, 0);//copy the enclosed image in a SD card (check the folder!!!)
  delay(5000);
  tft.clearScreen();
  if (tft.width() >= 272) bmpDraw("VSTOPB.BMP", 0, 0);
  else bmpDraw("VSTOPBS.BMP", 0, 0);//copy the enclosed image in a SD card (check the folder!!!)
  delay(5000);
  tft.clearScreen();
}



void bmpDraw(const char *filename, uint16_t x, uint16_t y) {

  uint16_t i;
  uint16_t bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint16_t buffidx = 0; 
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int16_t  w, h, row, col;
  uint32_t pos = 0, startTime = millis();
  buffidx = sizeof(sdbuffer);// Current position in sdbuffer
  if ((x >= tft.width()) || (y >= tft.height())) return;

  Sprintln();
  Sprint("Loading image '");
  Sprint(filename);
  Sprintln('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == 0) {
    //Sprint("File not found");
    return;
  }

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) { // BMP signature
    Sprint("File size: "); 
    w = read32(bmpFile);
    Sprintln(w);
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Sprint("Image Offset: "); Sprintln2(bmpImageoffset, DEC);
    // Read DIB header
    Sprint("Header size: ");
    w = read32(bmpFile);
    Sprintln(w);
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Sprint("Bit Depth: "); Sprintln(bmpDepth);
      if (((bmpDepth == 24) || (bmpDepth == 32)) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Sprint("Image size: ");
        Sprint(bmpWidth);
        Sprint('x');
        Sprintln(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        if (bmpDepth == 24) rowSize = (bmpWidth * 3 + 3) & ~3;
        else rowSize = (bmpWidth * 4);

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        uint16_t rowBuffer[w];
        if (((w - 1)+x) >= tft.width())  w = tft.width()  - x;
        if (((h - 1)+y) >= tft.height()) h = tft.height() - y;

        for (row = 0; row < h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if (bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col = 0; col < w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }
            if (bmpDepth == 24) i = tft.Color565(sdbuffer[buffidx+2], sdbuffer[buffidx+1], sdbuffer[buffidx]);
            else i = tft.Color565(sdbuffer[buffidx+2], sdbuffer[buffidx+1], sdbuffer[buffidx]);
            rowBuffer[col] = i>>8;
            rowBuffer[col] += (i & 0xff) <<8;
            //rowBuffer[col] = (sdbuffer[buffidx+2] & 0xF8) | ((sdbuffer[buffidx+1] & 0xE0) >> 5) | ((sdbuffer[buffidx+1] & 0x1C) << 11) | ((sdbuffer[buffidx] & 0xf8) << 5);
            if (bmpDepth == 24) buffidx+=3; else buffidx+=4;
          } // end pixel
          tft.setY(y + row);
          tft.drawPixels(rowBuffer, w, x, y + row);
        } // end scanline
        Sprint("Loaded in ");
        Sprint(millis() - startTime);
        Sprintln(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if (!goodBmp) {
    Sprintln("BMP format not recognized.");
  } else {
    Sprintln("end...");
  }
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

void writePixb(int16_t x, uint16_t color) {
  tft.setX(x);
  tft.writeCommand(RA8875_MRWC);
  tft.writeData16(color);
}

uint16_t read16(File &f) {
  uint16_t result = 0;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result = 0;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

