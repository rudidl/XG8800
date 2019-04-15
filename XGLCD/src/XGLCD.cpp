/*
 XGLCD library for www.x-graph.be
 Universal LCD library for X-Graph LCD modules with RA8875 driver
 Copyright (c) 2016-2019 DELCOMp bvba / UVee bvba
 2sd (a t) delcomp (d o t) com
 
 Targets: DELCOMp XG8800 & XG7300 LCD SHIELDS (https://www.x-graph.be)
 
 Optimized code is used to control the RA8875 chip with the hardware SPI bus of the CPU boards:
 - Arduino MKR family
 - ESP32
 - Pycom
 (- Particle Proton/Electron)
 - Particle Argon/Boron
 - nRF528xx
 - Arduino headers (note that the library is 32-bit optimised, it will run slow on 8-bit cpu's)
 
 This library contains functions:
 - generic for all RA8875 features using RA8875 hardware acceleration and SPI-bus optimalisations
 - low-level driver for Adafruit GFX library
 - low-level driver for TFT_eSPI/eFEX libraries
 - low-level driver for GUIslice
 - low-level driver for Littlevgl
 - compatibility layer for UTFT functions (where possible) (http://www.rinkydinkelectronics.com)
 (To maintain this compatibility there is NO code copied from the UTFT library as this is not open source)
 
 This library is based on the RA8875 library from https://github.com/sumotoy/RA8875
 
 License:GNU General Public License v3.0
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 SPI BUS WARNINGS:
 The LCD driver and SDCard socket are routed to the same SPI bus. Interrupts are NOT disabled during SPI bus
 transmissions because this results in hang ups on some CPU modules due to the prolonged no-interrupt
 period this causes.
 Make sure your app does NOT interrupt routines to use the SPI bus as this will cause problems with the LCD & SDCard functions
 
 WARNING: SPI_HAS_TRANSACTIONS is not used. This is because of the embedded SPI code to maximalise the
 SPI bus speed and thus the LCD drawing speed. Just make sure the LCD SPI bus is not used in interrupt routines.
 
 WARNING: SD.begin(XG_SD_CS) MUST be added AFTER tft.begin()
 If not included you might get weird behavior depending on the brand of the inserted SDCard.
*/

/* To make the compiled code smaller the following code was removed from the sumotoy RA8875 library:
 - support for all not X-Graph LCD types
 - support for all not X-Graph CPU modules support
 - the analog touchscreen is always enabled
 - the capacitive touchscreen code is removed
 - the _RA8875_TXTRNDOPTIMIZER optimized code is always used
 - all non X-Graph hardware features are removed (external font rom, external flash)
 - USE_SEPARATE_TXT_COLOR is always on
 - _DEFAULTTXTFRGRND & _DEFAULTTXTBKGRND are fixed to white & black. 
   If other colors are needed these should be set by function calls, not by a pre-compile user setting.
   Tranparency is default disabled. If needed this can be changed with a function call.
 - DEFAULTBACKLIGHT is removed, always black. Function needed to change this.
 - FORCE_RA8875_TXTREND_FOLLOW_CURS is default enabled, can not be disabled
UTFT remarks:
 - UTFT fonts are not supported
 - UTFT bitmaps are not supported
 
 
 TODO:
 - only use 32-bit variables
 - test Particle / Fanstel / Arduino Headers
*/

#include "XGLCD.h"

/******************************************************************************/
/*!
	Contructors
*/
/******************************************************************************/
XGLCD::XGLCD(void) {
}

/******************************************************************************/
/*!
	Initialize library, SPI, hardware and RA8875
*/
/******************************************************************************/
void XGLCD::begin(void) {
	
    // Init global variables
    _rotation = 0;
	_sleep = false;
    _portrait = false;
	_TXTForeColor = COLOR_WHITE;
	_TXTBackColor = COLOR_BLACK;
	_TXTrecoverColor = false;
    _brightness = 255;
	_cursorX = 0; _cursorY = 0; _scrollXL = 0; _scrollXR = 0; _scrollYT = 0; _scrollYB = 0;
	_scaleX = 1; _scaleY = 1;
	_scaling = false;
	_FNTspacing = 0;
	_TXTrender = false;
    _TXTAlignXToCenter = false;
    _TXTAlignYToCenter = false;
    _TXTwrap = true;
	_relativeCenter = false;
	_absoluteCenter = false;
    _FNTinterline = 0;
	_FNTcursorType = NOCURSOR;
	_FNTgradient = false;
	_arcAngle_max = ARC_ANGLE_MAX;
	_arcAngle_offset = ARC_ANGLE_OFFSET;
	_angle_offset = ANGLE_OFFSET;
	_tsAdcMinX = TOUCSRCAL_XLOW; _tsAdcMinY = TOUCSRCAL_YLOW; _tsAdcMaxX = TOUCSRCAL_XHIGH; _tsAdcMaxY = TOUCSRCAL_YHIGH;
    _maxLayers = 1;
    _currentLayer = 0;
    _useMultiLayers = false;                                                    //starts with one layer only
    _activeWindowXL = 0;
    _activeWindowYT = 0;
    _textMode = false;
    _color_bpp = 0;
    _lcdtype = 0;
    _backTransparent = false;

	// Start SPI initialization
    pinMode(XG_PIN_LCD_CS, OUTPUT);
    digitalWrite(XG_PIN_LCD_CS, HIGH);
    _spibegin();
    setSPI();
    _spisetSpeed(SPI_SPEED_SLOW);
    
    // I/O lines initialization
    pinMode(XG_PIN_LCD_RESET, OUTPUT);
	digitalWrite(XG_PIN_LCD_RESET, HIGH);
	delay(10);
	digitalWrite(XG_PIN_LCD_RESET, LOW);
	delay(220);
	digitalWrite(XG_PIN_LCD_RESET, HIGH);
	delay(300);

    pinMode(XG_PIN_SD_CS, OUTPUT);
    digitalWrite(XG_PIN_SD_CS, HIGH);
    
    // RA8875 initialization
    
    // Fill shadow registers
    _DPCR_Reg = RA8875_DPCR_ONE_LAYER + RA8875_DPCR_HDIR_NORMAL + RA8875_DPCR_VDIR_NORMAL;
    _MWCR0_Reg = RA8875_MWCR0_GFXMODE + RA8875_MWCR0_NO_CURSOR + RA8875_MWCR0_CURSOR_NORMAL + RA8875_MWCR0_MEMWRDIR_LT + RA8875_MWCR0_MEMWR_CUR_INC + RA8875_MWCR0_MEMRD_NO_INC;
    _FNCR0_Reg = RA8875_FNCR0_CGROM + RA8875_FNCR0_INTERNAL_CGROM + RA8857_FNCR0_8859_1;
    _FNCR1_Reg = RA8875_FNCR1_ALIGNMENT_OFF + RA8875_FNCR1_TRANSPARENT_OFF + RA8875_FNCR1_NORMAL + RA8875_FNCR1_SCALE_HOR_1 + RA8875_FNCR1_SCALE_VER_1;
    _FWTSET_Reg = RA8875_FWTSET_16X16;
    _SFRSET_Reg = 0b00000000;
    _INTC1_Reg = 0b00000000;

    // Software Reset
    writeCommand(RA8875_PWRR);
    _writeData(RA8875_PWRR_SOFTRESET);
    delay(20);
    _writeData(RA8875_PWRR_NORMAL);
    delay(200);
    // Set slow clock speed with default pixclk
    _setSysClock(0x07, RA8875_PLLC2_DIV8, RA8875_PCSR_PDATL | RA8875_PCSR_2CLK);

    // Color space setup (RA8875_SYSR)
    setColorBpp(16);
    
    // Get lcd type and set timing values accordingly
    // PIN1-5 = GPI0-GPI4
    // PIN6-9 = GPO0-GPO3
    // PIN10 = PMW2
    #ifdef XG_LCD_AUTO
        _writeRegister(RA8875_KSCR1, 0x00);                                     // Disable keyboard scanning
        /*_writeRegister(RA8875_GPO, 0x00);
        delay(10);
        if (!(_readRegister(RA8875_GPI) & 0x01)) {
            _writeRegister(RA8875_GPO, 0x01);
            delay(10);
            if (_readRegister(RA8875_GPI) & 0x01) _lcdtype = 1;
        }
        _writeRegister(RA8875_GPO, 0x00);
        delay(10);
        if (!(_readRegister(RA8875_GPI) & 0x02)) {
            _writeRegister(RA8875_GPO, 0x02);
            delay(10);
            if (_readRegister(RA8875_GPI) & 0x02) _lcdtype += 2;
        }
        _writeRegister(RA8875_GPO, 0x00);
        delay(10);
        if (!(_readRegister(RA8875_GPI) & 0x04)) {
            _writeRegister(RA8875_GPO, 0x04);
            delay(10);
            if (_readRegister(RA8875_GPI) & 0x04) _lcdtype += 4;
        }*/
        //uint8_t i = _readRegister(RA8875_GPI);
        uint8_t i;
        writeCommand(RA8875_GPI);
        _spiCSLow;
#ifdef _spixread
        _spixread(RA8875_DATAREAD, i);
#else
        _spiwrite(RA8875_DATAREAD);
        delayMicroseconds(50);                                                  // Stabilize time, else first bit is read wrong
        _spiread(i);
#endif
        _spiCSHigh;

        if (i == 0x00) _lcdtype = 0;                                            // Default is lcdtype 0 = 5.0" 800x480
        if (i == 0x01) _lcdtype = 2;                                            // lcdtype 2 = 9.0" 800x480
    #else
        #if defined(XG_LCD_56)
            _lcdtype = 1;
        #elif defined(XG_LCD_90)
            _lcdtype = 2;
        #endif
    #endif

    switch (_lcdtype) {
        case 0:
        case 2:                                                                 // 800x480
            _activeWindowXR  = LCD_WIDTH = _width = 800;
            _activeWindowYB  = LCD_HEIGHT = _height = 480;
            _pixclk          = RA8875_PCSR_PDATL | RA8875_PCSR_2CLK;
            _hsync_finetune  = 0;
            _hsync_nondisp   = 26;
            _hsync_start     = 32;
            _hsync_pw        = 96;
            _vsync_nondisp   = 32;
            _vsync_start     = 23;
            _vsync_pw        = 2;
            _pll_div         = RA8875_PLLC2_DIV4;
            break;
        case 1:                                                                 // 640x480
            _activeWindowXR  = LCD_WIDTH = _width = 640;
            _activeWindowYB  = LCD_HEIGHT = _height = 480;
            _pixclk          = RA8875_PCSR_2CLK;
            _hsync_finetune  = 5;
            _hsync_nondisp   = 127;
            _hsync_start     = 16;
            _hsync_pw        = 8;
            _vsync_nondisp   = 11;
            _vsync_start     = 15;
            _vsync_pw        = 2;
            _pll_div         = RA8875_PLLC2_DIV4;
            break;
        /*
         case A:                                                                // 320240
            _pixclk          = RA8875_PCSR_8CLK;
            _hsync_finetune  = 0;
            _hsync_nondisp   = 42;
            _hsync_start     = 40;
            _hsync_pw        = 32;
            _vsync_nondisp   = 6;
            _vsync_start     = 15;
            _vsync_pw        = 3;
            _pll_div         = RA8875_PLLC2_DIV2;
            break;
         case B:                                                                // 480272
            _pixclk          = RA8875_PCSR_PDATL | RA8875_PCSR_4CLK;
            _hsync_finetune  = 0;
            _hsync_nondisp   = 10;
            _hsync_start     = 8;
            _hsync_pw        = 48;
            _vsync_nondisp   = 3;
            _vsync_start     = 8;
            _vsync_pw        = 10;
            _pll_div         = RA8875_PLLC2_DIV4;
            break;
        */
    }
    
    _writeRegister(RA8875_HDWR, ((LCD_WIDTH)/8) - 1);		                    // LCD Horizontal Display Width Register
    _writeRegister(RA8875_HNDFTR, RA8875_HNDFTR_DE_HIGH + _hsync_finetune);     // Horizontal Non-Display Period Fine Tuning Option Register
    _writeRegister(RA8875_HNDR, (_hsync_nondisp-_hsync_finetune - 2) / 8);		// LCD Horizontal Non-Display Period Register
    _writeRegister(RA8875_HSTR, _hsync_start/8 - 1);		                    // HSYNC Start Position Register
    _writeRegister(RA8875_HPWR, RA8875_HPWR_LOW + (_hsync_pw/8 - 1));		    // HSYNC Pulse Width Register
    _writeRegister(RA8875_VDHR0, (uint16_t)(((LCD_HEIGHT) - 1) & 0xFF));         // LCD Vertical Display Height Register0
    _writeRegister(RA8875_VDHR0+1, (uint16_t)((LCD_HEIGHT) - 1) >> 8);	        // LCD Vertical Display Height Register1
    _writeRegister(RA8875_VNDR0, _vsync_nondisp-1);                             // LCD Vertical Non-Display Period Register 0
    _writeRegister(RA8875_VNDR0+1, _vsync_nondisp >> 8);	                    // LCD Vertical Non-Display Period Register 1
    _writeRegister(RA8875_VSTR0, _vsync_start-1);                               // VSYNC Start Position Register 0
    _writeRegister(RA8875_VSTR0+1, _vsync_start >> 8);	                        // VSYNC Start Position Register 1
    _writeRegister(RA8875_VPWR, RA8875_VPWR_LOW + _vsync_pw - 1);               // VSYNC Pulse Width Register
    
    // Cursor & Font
    setCursorBlinkRate(10);                                                     // set default blink rate
    setIntFontCoding(ISO_IEC_8859_1);                                           // set default internal font encoding
    setFont();	                                                                // set internal font use
    
    // Set clock speed to normal
    _setSysClock(0x0b, _pll_div, _pixclk);
    _updateActiveWindow(true);									                // set the whole screen as active
    delay(10);

    _spisetSpeed(SPI_SPEED_WRITE);
    delay(1);
    
    // Clear the display and switch it on
    clearMemory();
    delay(1);
	displayOn(true);
    delay(1);
	fillWindow(COLOR_BLACK);
    backlight(true);
	setRotation(0);
	setForegroundColor(_TXTForeColor);
	setBackgroundColor(_TXTBackColor);
    
    // Set text & Cursor to defaults
    _setTextMode(false);
    setActiveWindow();
    _writeRegister(RA8875_FNCR1,_FNCR1_Reg);
    setCursor(0,0);
    
    // Start touchscreen
    touchBegin();
}

/******************************************************************************/
/*!
 This function set the sysClock accordingly datasheet
 Parameters:
 pll1: PLL Control Register 1
 pll2: PLL Control Register 2
 pixclk: Pixel Clock Setting Register
 [private]
 */
/******************************************************************************/
void XGLCD::_setSysClock(uint8_t pll1,uint8_t pll2,uint8_t pixclk)
{
    _writeRegister(RA8875_PLLC1,pll1);
    delay(1);
    _writeRegister(RA8875_PLLC1+1,pll2);
    delay(1);
    _writeRegister(RA8875_PCSR,pixclk);
    delay(1);
}

/******************************************************************************/
/*!
 turn display on/off
 */
/******************************************************************************/
void XGLCD::displayOn(boolean on)
{
    on == true ? _writeRegister(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON) : _writeRegister(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
}


/******************************************************************************/
/*!
 Set SPI bus to defaults for LCD Driver
 */
/******************************************************************************/
// NO interrupts are disabled: Spark & MKR1000 do not allow this for a 'long' period which results in hang-ups
// do NOT allow interrupt routines to use the SPI bus as this will cause problems with the LCD & SDCard functions

// Every LCD SPI bus use MUST be preceded with an SPI bus configuration as other SPI device using the same bus
// might have reconfigured the SPI hardware
//
// Before calling a LCD library function, first call the setSPI() function to set the SPI bus in the correct mode.
// This is only required if other devices use the same SPI bus (for example the SDCard)

void XGLCD::setSPI(void) {
    _spisetDataMode(SPI_MODE_LCD);
    _spisetBitOrder(MSBFIRST);
    _spisetSpeed(SPI_SPEED_WRITE);
#ifdef _spisetBitLen
    _spisetBitLen;
#endif
}

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                RA8875 INTERNAL                               +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Stuff you will probably never need in your UI, but it is important for the
internals of the RA8875.
*/

/******************************************************************************/
/*!
	return true when register has done the job, otherwise false.
*/
/******************************************************************************/
boolean XGLCD::_waitPoll(uint8_t regname, uint8_t waitflag)
{
	uint8_t temp;
	unsigned long timeout = millis();
	
	while (1) {
		temp = _readRegister(regname);
		if (!(temp & waitflag)) return true;
		if ((millis() - timeout) > 20) return false;                            //emergency exit! Should never occur.
	}  
	return false;
}

/******************************************************************************/
/*!
	Just another specified wait routine until job it's done
	Parameters:
	res:
	0x80(for most operations),
	0x40(BTE wait), 
	0x01(DMA wait) (X-Graph:not allowed anymore)
 
Note:
Does a continuous RA8875_CMDREAD read (slow SPI bus, one byte read).
A CMDREAD is not possible, it always reads the 'Status Register'.
 - SR.7 = 1 = memory read/write busy (which includes font writes)
 - SR.6 = 1 = BTE busy
 - other bits are related to touch panel detection / sleep modus / external flash/rom busy

Used in the following library function calls:
 - clearMemory(): SR.7 memory clear is completed
 - _charWrite(): SR.7 -> internal font character is completely written
 - BTE_Move(): SR.6 is checked before AND after the BTE commands are given
 - BTE_Enable(): SR.6 -> wait for BTE done/idle
 - uselayers() when switching to 8-bit colors: _waitBusy() call = no parameters: ???? -> this results in a 100msec wait -> changed to SR.7 check
    (but why is this needed in this situation ?)
 - WritePattern(): SR.7 -> checks after each word written (needed for pattern ??)
 
 */
/******************************************************************************/
void XGLCD::_waitBusy(uint8_t res)
{
	uint8_t temp; 	
	unsigned long start = millis();
    
	do {
		temp = readStatus();
        if ((millis() - start) > 10) return;
    } while ((temp & res) == res);
}

/******************************************************************************/
/*!
 Performs a SW-based reset of the RA8875
 */
/******************************************************************************/
void XGLCD::softReset(void) {
    writeCommand(RA8875_PWRR);
    _writeData(RA8875_PWRR_SOFTRESET);
    _writeData(RA8875_PWRR_NORMAL);
    delay(1);
}

/******************************************************************************/
/*!
 Sleep mode on/off (complete sequence)
 The sleep on/off sequence it's quite tricky on RA8875 when in SPI mode!
 */
/******************************************************************************/
void XGLCD::sleep(boolean sleep)
{
    if (_sleep != sleep){                                                       // only when needed
        _sleep = sleep;
        if (_sleep == true){
            //1)turn off backlight
            //2)decelerate SPI clock
            _spisetSpeed(SPI_SPEED_SLOW);
            //3)set PLL to default
            _setSysClock(0x07,RA8875_PLLC2_DIV8,RA8875_PCSR_PDATR | RA8875_PCSR_4CLK);
            //4)display off & sleep
            _writeRegister(RA8875_PWRR, RA8875_PWRR_DISPOFF | RA8875_PWRR_SLEEP);
            delay(100);
        } else {
            //1)wake up with display off(100ms)
            _writeRegister(RA8875_PWRR, RA8875_PWRR_DISPOFF);
            delay(100);
            //2)bring back the pll
            _setSysClock(0x07, RA8875_PLLC2_DIV8, _pixclk);
            //_writeRegister(RA8875_PCSR,initStrings[_initIndex][2]);//Pixel Clock Setting Register
            delay(20);
            _writeRegister(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);//disp on
            delay(20);
            //4)resume SPI speed
            _spisetSpeed(SPI_SPEED_WRITE);
            //5)PLL afterburn!
            _setSysClock(0x0b, _pll_div, _pixclk);
            //_writeRegister(RA8875_PWRR, RA8875_PWRR_NORMAL);
        }
    }
}

/******************************************************************************/
/*!
 Sets or resets the GPIOX line
 Switches the power on/off of the LCD
 */
/******************************************************************************/
void XGLCD::GPIOX(boolean on) {
    if (on)
        _writeRegister(RA8875_GPIOX, 1);
    else
        _writeRegister(RA8875_GPIOX, 0);
}


/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                WINDOWS                                       +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
The RA8875 has 768kByte internal memory.
For a 800x480 LCD (16-bit color) the full memory is needed.
By only using 8-bit color 384kByte is needed, and 2 layers are available.
 
Many RA8875 hardware accelerated drawing functions can limit the drawing area
to the 'active window'. If this is not defined the 'full screen' is used.
Depending on the number of layers 1 or 2 'full screens' are available. The
active window can be positioned anywhere in the fulls screen area.
 
Here you will also find functions to rotate the display.
*/

/******************************************************************************/
/*!
 Set the Active Window as FULL SCREEN
 */
/******************************************************************************/
void XGLCD::setActiveWindow(void)
{
    _activeWindowXL = 0; _activeWindowXR = LCD_WIDTH;
    _activeWindowYT = 0; _activeWindowYB = LCD_HEIGHT;
    if (_portrait){swapvals(_activeWindowXL,_activeWindowYT); swapvals(_activeWindowXR,_activeWindowYB);}
    _updateActiveWindow(true);
}

/******************************************************************************/
/*!
 Set the Active Window
 Parameters:
 XL: Horizontal Left
 XR: Horizontal Right
 YT: Vertical TOP
 YB: Vertical Bottom
 */
/******************************************************************************/
void XGLCD::setActiveWindow(int16_t XL,int16_t XR ,int16_t YT ,int16_t YB)
{
    if (_portrait) {swapvals(XL,YT); swapvals(XR,YB);}
    
    if (XR >= LCD_WIDTH) XR = LCD_WIDTH;
    if (YB >= LCD_HEIGHT) YB = LCD_HEIGHT;
    
    _activeWindowXL = XL; _activeWindowXR = XR;
    _activeWindowYT = YT; _activeWindowYB = YB;
    _updateActiveWindow(false);
}

/******************************************************************************/
/*!
 this updates the RA8875 Active Window registers
 [private]
 */
/******************************************************************************/
void XGLCD::_updateActiveWindow(bool full)
{
    if (full){
        // X
        _writeRegister(RA8875_HSAW0,    0x00);
        _writeRegister(RA8875_HSAW0 + 1,0x00);
        _writeRegister(RA8875_HEAW0,    (LCD_WIDTH) & 0xFF);
        _writeRegister(RA8875_HEAW0 + 1,(LCD_WIDTH) >> 8);
        // Y
        _writeRegister(RA8875_VSAW0,    0x00);
        _writeRegister(RA8875_VSAW0 + 1,0x00);
        _writeRegister(RA8875_VEAW0,    (LCD_HEIGHT) & 0xFF);
        _writeRegister(RA8875_VEAW0 + 1,(LCD_HEIGHT) >> 8);
    } else {
        // X
        _writeRegister(RA8875_HSAW0,    _activeWindowXL & 0xFF);
        _writeRegister(RA8875_HSAW0 + 1,_activeWindowXL >> 8);
        _writeRegister(RA8875_HEAW0,    _activeWindowXR & 0xFF);
        _writeRegister(RA8875_HEAW0 + 1,_activeWindowXR >> 8);
        // Y
        _writeRegister(RA8875_VSAW0,     _activeWindowYT & 0xFF);
        _writeRegister(RA8875_VSAW0 + 1,_activeWindowYT >> 8);
        _writeRegister(RA8875_VEAW0,    _activeWindowYB & 0xFF);
        _writeRegister(RA8875_VEAW0 + 1,_activeWindowYB >> 8);
    }
}

/******************************************************************************/
/*!
 Get the Active Window
 Parameters:
 XL: Horizontal Left
 XR: Horizontal Right
 YT: Vertical TOP
 YB: Vertical Bottom
 */
/******************************************************************************/
void XGLCD::getActiveWindow(int16_t &XL,int16_t &XR ,int16_t &YT ,int16_t &YB)//0.69b24
{
    XL = _activeWindowXL; XR = _activeWindowXR;
    YT = _activeWindowYT; YB = _activeWindowYB;
}

/******************************************************************************/
/*!		
		Clear memory = fill window with 0's
        (different from fillWindow/clearScreen which fill the window with a
        color)
 	    Parameters:
            1: start the clear memory operation
            0: stop (or interrupt) the clear memory operation
 
 This function will:
 1. clear the full window (1 layer) when bit 6 is '0' (clearActiveWindow())
 2. clear the current active window when bit 6 is '1'

 This function uses the MCLR register.
 By setting bit 7 '1' the memory clearing starts (clearMemory(false)).
 It can be stopped by writing a '0' to bit7 (clearMemory(true)).
 The RA8875 will auto-clear bit7 once the memory clearing is finished
 (hence the _waitBusy(0x80) call).
 The function works in co-operation with clearActiveWindow(). Based on 
 the contents of bit 6 of MCLR (set or reset with clearActiveWindow()),
 the clearMemory() function will clear all memory of the active windows
 memory only.
 
 This function is claimed to be slow, and it should be better to use
 fillWindow which is claimed to work faster and have the same result.
/******************************************************************************/
void XGLCD::clearMemory(bool stop)
{
    uint8_t temp;
    temp = _readRegister(RA8875_MCLR);
    stop == true ? temp &= ~RA8875_MCLR_START : temp |= RA8875_MCLR_START;
    _writeData(temp);
    if (!stop) _waitBusy(0x80);
}


/******************************************************************************/
/*!
 Clear the active window
 Parameters:
 full: false(clear current window), true clear full window
 */
/******************************************************************************/
void XGLCD::clearActiveWindow(bool full)
{
    uint8_t temp;
    temp = _readRegister(RA8875_MCLR);
    full == true ? temp &= ~RA8875_MCLR_ACTIVE : temp |= RA8875_MCLR_ACTIVE;
    _writeData(temp);
}

/******************************************************************************/
/*!		
		Return the max tft width.
		Parameters: 
		absolute:
        - true: physical width of LCD
        - false: visible width (depends on portrait or landscape mode)
*/
/******************************************************************************/
uint16_t XGLCD::width(bool absolute) const
{ 
	if (absolute) return LCD_WIDTH;
	return _width; 
}

/******************************************************************************/
/*!		
		Return the max tft height.
		Parameters: 
		absolute:
        - true: physical height of LCD
        - false: visible height (depends on portrait or landscape mode)
*/
/******************************************************************************/
uint16_t XGLCD::height(bool absolute) const
{ 
	if (absolute) return LCD_HEIGHT;
	return _height; 
}

/******************************************************************************/
/*!
      Change the beam scan direction on display
	  Parameters:
	  invertH: true(inverted),false(normal) horizontal
	  invertV: true(inverted),false(normal) vertical
*/
/******************************************************************************/
void XGLCD::_scanDirection(boolean invertH,boolean invertV)
{
	invertH == true ? _DPCR_Reg |= RA8875_DPCR_HDIR_REVERSE : _DPCR_Reg &= ~RA8875_DPCR_HDIR_REVERSE;
	invertV == true ? _DPCR_Reg |= RA8875_DPCR_VDIR_REVERSE : _DPCR_Reg &= ~RA8875_DPCR_VDIR_REVERSE;
	_writeRegister(RA8875_DPCR,_DPCR_Reg);
}

/******************************************************************************/
/*!
      Change the rotation of the screen
	  Parameters:
	  rotation:
	    0 = default
		1 = 90 clockwise
		2 = 180 = upside down
		3 = 270 clockwise
*/
/******************************************************************************/
void XGLCD::setRotation(uint8_t rotation)//0.69b32 - less code
{
	_rotation = rotation % 4;                                                   //limit to the range 0-3
	switch (_rotation) {
	case 0:
		//default
		_portrait = false;
		_scanDirection(0,0);
		_tsAdcMinX = TOUCSRCAL_XLOW; _tsAdcMinY = TOUCSRCAL_YLOW; _tsAdcMaxX = TOUCSRCAL_XHIGH; _tsAdcMaxY = TOUCSRCAL_YHIGH;
    break;
	case 1:
		//90
		_portrait = true;
		_scanDirection(1,0);
		_tsAdcMinX = TOUCSRCAL_XHIGH; _tsAdcMinY = TOUCSRCAL_YLOW; _tsAdcMaxX = TOUCSRCAL_XLOW; _tsAdcMaxY = TOUCSRCAL_YHIGH;
    break;
	case 2:
		//180
		_portrait = false;
		_scanDirection(1,1);
		_tsAdcMinX = TOUCSRCAL_XHIGH; _tsAdcMinY = TOUCSRCAL_YHIGH; _tsAdcMaxX = TOUCSRCAL_XLOW; _tsAdcMaxY = TOUCSRCAL_YLOW;
    break;
	case 3:
		//270
		_portrait = true;
		_scanDirection(0,1);
		_tsAdcMinX = TOUCSRCAL_XLOW; _tsAdcMinY = TOUCSRCAL_YHIGH; _tsAdcMaxX = TOUCSRCAL_XHIGH; _tsAdcMaxY = TOUCSRCAL_YLOW;
    break;
	}
    
	if (_portrait){
		_width = LCD_HEIGHT;
		_height = LCD_WIDTH;
	} else {
		_width = LCD_WIDTH;
		_height = LCD_HEIGHT;
	}
    
    if (_portrait) _FNCR1_Reg |= RA8875_FNCR1_90DEGREES; else _FNCR1_Reg &= ~RA8875_FNCR1_90DEGREES;
    _writeRegister(RA8875_FNCR1,_FNCR1_Reg);                                    // Rotates fonts 90 degrees (for build-in fonts that is)
    setActiveWindow();
}

/******************************************************************************/
/*!
      Get rotation setting
*/
/******************************************************************************/
uint8_t XGLCD::getRotation()
{
	return _rotation;
}

/******************************************************************************/
/*!
      true if rotation 1 or 3
*/
/******************************************************************************/
boolean XGLCD::isPortrait(void)
{
	return _portrait;
}


/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +                        GRAPHIC PRIMITIVES                                   +
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Probably the most interesting part of this library.
  Here you'll find all the functions to draw graphic primitives on the screen
  using the RA8875 hardware accelaration.
*/

/******************************************************************************/
/*!
 Write a single pixel
 Parameters:
 x: horizontal pos
 y: vertical pos
 color: RGB565 color
 NOTE:
 In 8bit bpp RA8875 needs a 8bit color(332) and NOT a 16bit(565),
 the routine deal with this...
 */
/******************************************************************************/
void XGLCD::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if (_textMode) _setTextMode(false);                                         //we are in text mode?
    setXY(x,y);
    
    writeCommand(RA8875_MRWC);
    if (_color_bpp > 8){
        _writeData16(color);
    } else {                                                                    //TOTEST:layer bug workaround for 8bit color!
        _writeData(_color16To8bpp(color));
    }
}

void XGLCD::drawPixel(int16_t x, int16_t y) {
    drawPixel(x,y,_foreColor);
}

/******************************************************************************/
/*!
 Draw a series of pixels
 Parameters:
 p: an array of 16bit colors (pixels)
 count: how many pixels
 x: horizontal pos
 y: vertical pos
 NOTE:
 In 8bit bpp RA8875 needs a 8bit color(332) and NOT a 16bit(565),
 the routine deals with this...
 */
/******************************************************************************/
void XGLCD::drawPixels(uint16_t p[], uint32_t count, int16_t x, int16_t y)
{
    uint32_t i,j;
    uint16_t temp = 0;
    
    if (_textMode) _setTextMode(false);                                         //we are in text mode?
    
    setXY(x,y);
    writeCommand(RA8875_MRWC);
#ifdef _spiwrite24
    if (_color_bpp < 16) {                                                      // 8-bit panel, convert the array (it will be overwritten)
        j = 0;
        for (i=0;i<count;i+=2) p[j++] = (_color16To8bpp(p[i])<<8) + _color16To8bpp(p[i+1]);
        count /= 2;                                                             // For odd number of pixels, the last pixel will not be shown, always use even number of pixels
    }
    _spiCSLow;
    if (_portrait) {
        for (i=0;i<count;i++){
            _spiwrite24(RA8875_DATAWRITE, temp);
            _spiCSHigh;
            x++;
            setXY(x,y);
            writeCommand(RA8875_MRWC);
            _spiCSLow;
        }
    } else {
#ifdef _spixwritedma
        _spixwritedma(RA8875_DATAWRITE, p, count);
#else
        _spiwrite(RA8875_DATAWRITE);
        _spiwritedma(p, count);
#endif
    }
    _spiCSHigh;
#else
    _spiCSLow;
    _spiwrite(RA8875_DATAWRITE);
    for (i=0;i<count;i++){
        if (_color_bpp < 16) {
            temp = _color16To8bpp(p[i]);                                        //TOTEST:layer bug workaround for 8bit color!
        } else {
            temp = p[i];
        }
        if (_color_bpp > 8){
            _spiwrite16(temp);
        } else {                                                                //TOTEST:layer bug workaround for 8bit color!
            _spiwrite(temp & 0xFF);
        }
        if (_portrait) {                                                        // X-GRAPH: solve bug in drawPixels in portrait mode = a lot slower to draw !!//
            _spiCSHigh;
            x++;
            setXY(x,y);
            writeCommand(RA8875_MRWC);
            _spiCSLow;
            _spiwrite(RA8875_DATAWRITE);
        }
    }
    _spiCSHigh;
#endif
}

/******************************************************************************/
/*!
 Get a pixel color from screen
 Parameters:
 x: horizontal pos
 y: vertical pos
 */
/******************************************************************************/
uint16_t XGLCD::getPixel(int16_t x, int16_t y)
{
    uint32_t color, color2;
    setXY(x,y);
    if (_textMode) _setTextMode(false);                                         //we are in text mode?
    writeCommand(RA8875_MRWC);
    _spisetSpeed(SPI_SPEED_READ);
    _spiCSLow;
#ifdef _spixbread
    if (_color_bpp > 8) {
        _spixbread(RA8875_DATAREAD, color, 32);
        color >>= 16;
    } else {
        _spixbread(RA8875_DATAREAD, color, 16);
        color >>= 8;
    }
#else
    _spiwrite(RA8875_DATAREAD);
    if (_color_bpp > 8) {
        _spiwrite(0x00);                                                            //first word it's dummy
        _spiwrite(0x00);
        _spiread(color2);
        _spiread(color);
        color |= (color2 << 8);
    } else {
        _spiwrite(0x00);                                                            //first byte it's dummy
        _spiread(color);
    }
#endif
    _spiCSHigh;
    _spisetSpeed(SPI_SPEED_WRITE);
    return (uint16_t)color;
}

/******************************************************************************/
/*!
 HW accelerated function to push a chunk of raw pixel data
 Parameters:
 num: the number of pixels to push
 p: the pixel color to use
 */
/******************************************************************************/
void XGLCD::pushPixels(uint32_t num, uint16_t p) {
    if (_textMode) _setTextMode(false);                                         //we are in text mode?

    //setXY(x,y);
    _spiCSLow;
#ifdef _spiwrite24
    while (num--) {
        if (_color_bpp > 8) {
            _spiwrite24(RA8875_DATAWRITE, p);
        } else {
            _writeData16((RA8875_DATAWRITE<<8) + _color16To8bpp(p));
        }
    }
#else
    _spiwrite(RA8875_DATAWRITE);
    while (num--) {
        if (_color_bpp > 8) {
            _spiwrite16(p);
        } else {
            _writeData(_color16To8bpp(p));
        }
    }
#endif
    _spiCSHigh;
}


/******************************************************************************/
/*!
 Fill the ActiveWindow by using a specified RGB565 color
 Parameters:
 color: RGB565 color (default=BLACK)
 */
/******************************************************************************/
void XGLCD::fillWindow(uint16_t color)
{
    _line_addressing(0,0,LCD_WIDTH-1, LCD_HEIGHT-1);
    setForegroundColor(color);
    writeCommand(RA8875_DCR);
    _writeData(0xB0);
    _waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
    _TXTrecoverColor = true;
}

/******************************************************************************/
/*!
 clearScreen it's different from fillWindow because it doesn't depends
 from the active window settings so it will clear all the screen.
 It should be used only when needed since it's slower than fillWindow.
 parameter:
 color: 16bit color (default=BLACK)
 */
/******************************************************************************/
void XGLCD::clearScreen(uint16_t color)
{
    setActiveWindow();
    fillWindow(color);
}

/******************************************************************************/
/*!
 draws a dots filled area
 Parameters:
 x: horizontal origin
 y: vertical origin
 w: width
 h: height
 spacing: space between dots in pixels (min 2pix)
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::drawMesh(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t spacing, uint16_t color)
{
    if (spacing < 2) spacing = 2;
    if (((x + w) - 1) >= _width)  w = _width  - x;
    if (((y + h) - 1) >= _height) h = _height - y;
    
    int16_t n, m;
    
    if (w < x) {n = w; w = x; x = n;}
    if (h < y) {n = h; h = y; y = n;}
    for (m = y; m <= h; m += spacing) {
        for (n = x; n <= w; n += spacing) {
            drawPixel(n, m, color);
        }
    }
}

/******************************************************************************/
/*!
 Basic line draw
 Parameters:
 x0: horizontal start pos
 y0: vertical start
 x1: horizontal end pos
 y1: vertical end pos
 color: RGB565 color
 NOTE:
 Remember that this write from->to so: drawLine(0,0,2,0,RA8875_RED);
 result a 3 pixel long! (0..1..2)
 */
/******************************************************************************/
void XGLCD::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    if ((x0 == x1 && y0 == y1)) {
        drawPixel(x0,y0,color);
        return;
    }
    
    _TXTrecoverColor = true;
    if (color != _foreColor) setForegroundColor(color);
    
    if (_portrait) { swapvals(x0,y0); swapvals(x1,y1);}
    if (_textMode) _setTextMode(false);                                         //we are in text mode?
    _line_addressing(x0,y0,x1,y1);
    _writeRegister(RA8875_DCR,0x80);
    _waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

void XGLCD::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    drawLine(x0, y0, x1, y1, _foreColor);
}

/******************************************************************************/
/*!
 Basic line by using Angle as parameter
 Parameters:
 x: horizontal start pos
 y: vertical start
 angle: the angle of the line
 length: lenght of the line
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::drawLineAngle(int16_t x, int16_t y, int16_t angle, uint16_t length, uint16_t color,int offset)
{
    
    if (length < 2) {
        drawPixel(x,y,color);
    } else {
        length--;
        drawLine(
                 x,
                 y,
                 x + round((length) * _cosDeg_helper(angle + offset)),
                 y + round((length) * _sinDeg_helper(angle + offset)),
                 color);
    }
}

/******************************************************************************/
/*!
 Basic line by using Angle as parameter
 Parameters:
 x: horizontal start pos
 y: vertical start
 angle: the angle of the line
 start: where line start
 length: lenght of the line
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::drawLineAngle(int16_t x, int16_t y, int16_t angle, uint16_t start, uint16_t length, uint16_t color,int offset)
{
    if (start - length < 2) {
        drawPixel(x,y,color);
    } else {
        length--;
        drawLine(
                 x + round(start * _cosDeg_helper(angle + offset)),
                 y + round(start * _sinDeg_helper(angle + offset)),
                 x + round((start + length) * _cosDeg_helper(angle + offset)),
                 y + round((start + length) * _sinDeg_helper(angle + offset)),
                 color);
    }
}

/******************************************************************************/
/*!
 draws a single vertical line
 Parameters:
 x: horizontal start
 y: vertical start
 h: height
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    
    if (h < 1) h = 1;
    h < 2 ? drawPixel(x,y,color) : drawLine(x, y, x, (y+h)-1, color);
}

/******************************************************************************/
/*!
 draws a single horizontal line
 Parameters:
 x: horizontal start
 y: vertical start
 w: width
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    if (w < 1) w = 1;
    w < 2 ? drawPixel(x,y,color) : drawLine(x, y, (w+x)-1, y, color);
}

/******************************************************************************/
/*!
 draws a rectangle
 Parameters:
 x: horizontal start
 y: vertical start
 w: width
 h: height
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    //RA8875 is not out-of-range tolerant so this is a workaround
    if (w < 1 || h < 1) return;                                                 //it cannot be!
    if (w < 2 && h < 2){                                                        //render as pixel
        drawPixel(x,y,color);
    } else {                                                                    //render as rect
        _rect_helper(x,y,(w+x)-1,(h+y)-1,color,false);
    }
}

void XGLCD::drawRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    drawRect(x0, y0, x1-x0+1, y1-y0+1, _foreColor);
}

/******************************************************************************/
/*!
 draws a FILLED rectangle
 Parameters:
 x: horizontal start
 y: vertical start
 w: width
 h: height
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    //RA8875 it's not out-of-range tolerant so this is a workaround
    if (w < 1 || h < 1) return;                                                 //it cannot be!
    if (w < 2 && h < 2){                                                        //render as pixel
        drawPixel(x,y,color);
    } else {                                                                    //render as rect
        _rect_helper(x,y,(x+w)-1,(y+h)-1,color,true);
    }
}

void XGLCD::fillRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    fillRect(x0, y0, x1-x0+1, y1-y0+1, _foreColor);
}

/******************************************************************************/
/*!
 Draw a rounded rectangle
 Parameters:
 x:   x location of the rectangle
 y:   y location of the rectangle
 w:  the width in pix
 h:  the height in pix
 r:  the radius of the rounded corner
 color: RGB565 color
 _roundRect_helper it's not tolerant to improper values
 so there's some value check here
 */
/******************************************************************************/
void XGLCD::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    if (r == 0) {drawRect(x,y,w,h,color); return;}                              // X-GRAPH: return added
    if (w < 1 || h < 1) return;                                                 //it cannot be!
    if (w < 2 && h < 2){                                                        //render as pixel
        drawPixel(x,y,color);
    } else {                                                                    //render as rect
        if (w < h && (r * 2) >= w) r = (w / 2) - 1;
        if (w > h && (r * 2) >= h) r = (h / 2) - 1;
        if (r == w || r == h) drawRect(x,y,w,h,color);
        _roundRect_helper(x, y, (x + w) - 1, (y + h) - 1, r, color, false);
    }
}

void XGLCD::drawRoundRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    drawRoundRect(x0, y0, x1-x0+1, y1-y0+1, 2, _foreColor);
}

/******************************************************************************/
/*!
 Draw a filled rounded rectangle
 Parameters:
 x:   x location of the rectangle
 y:   y location of the rectangle
 w:  the width in pix
 h:  the height in pix
 r:  the radius of the rounded corner
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
    if (r == 0) {fillRect(x,y,w,h,color); return;}                              // X-GRAPH: return added
    if (w < 1 || h < 1) return;                                                 //it cannot be!
    if (w < 2 && h < 2){                                                        //render as pixel
        drawPixel(x,y,color);
    } else {                                                                    //render as rect
        if (w < h && (r * 2) >= w) r = (w / 2) - 1;
        if (w > h && (r  *2) >= h) r = (h / 2) - 1;
        if (r == w || r == h) drawRect(x,y,w,h,color);
        _roundRect_helper(x, y, (x + w) - 1, (y + h) - 1, r, color, true);
    }
}

void XGLCD::fillRoundRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    fillRoundRect(x0, y0, x1-x0+1, y1-y0+1, 2, _foreColor);
}

/******************************************************************************/
/*!
 Draw circle
 Parameters:
 x0: The 0-based x location of the center of the circle
 y0: The 0-based y location of the center of the circle
 r: radius
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    _center_helper(x0,y0);                                                      // check for x or y CENTER -> width/height / 2
    //if (r < 1) return;                                                        // X-GRAPH removed cause code is also in _circle_helper
    //if (r < 2) {
    //    drawPixel(x0,y0,color);
    //    return;
    //}
    _circle_helper(x0, y0, r, color, false);
}

void XGLCD::drawCircle(int16_t x0, int16_t y0, int16_t r) {
    drawCircle(x0, y0, r, _foreColor);
}

/******************************************************************************/
/*!
 Draw filled circle
 Parameters:
 x0: The 0-based x location of the center of the circle
 y0: The 0-based y location of the center of the circle
 r: radius
 color: RGB565 color
 */
/******************************************************************************/

void XGLCD::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    _center_helper(x0,y0);
    //if (r < 1) return;                                                        // X-GRAPH removed cause code is also in _circle_helper
    //if (r == 1) {
    //    drawPixel(x0,y0,color);
    //    return;
    //}
    _circle_helper(x0, y0, r, color, true);
}

void XGLCD::fillCircle(int16_t x0, int16_t y0, int16_t r) {
    fillCircle(x0, y0, r, _foreColor);
}

/******************************************************************************/
/*!
 Draw arc
 */
/******************************************************************************/

void XGLCD::drawArc(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color) {
    if (start == 0 && end == _arcAngle_max) {
        _drawArc_helper(cx, cy, radius, thickness, 0, _arcAngle_max, color);
    } else {
        _drawArc_helper(cx, cy, radius, thickness, start + (_arcAngle_offset / (float)360)*_arcAngle_max, end + (_arcAngle_offset / (float)360)*_arcAngle_max, color);
    }
}

/******************************************************************************/
/*!
 change the arc default parameters
 */
/******************************************************************************/
void XGLCD::setArcParams(float arcAngleMax, int arcAngleOffset)
{
    _arcAngle_max = arcAngleMax;
    _arcAngle_offset = arcAngleOffset;
}

/******************************************************************************/
/*!
 change the angle offset parameter from default one
 */
/******************************************************************************/
void XGLCD::setAngleOffset(int16_t angleOffset)
{
    _angle_offset = ANGLE_OFFSET + angleOffset;
}

/******************************************************************************/
/*!
 Draw a quadrilater by connecting 4 points
 Parameters:
 x0:
 y0:
 x1:
 y1:
 x2:
 y2:
 x3:
 y3:
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::drawQuad(int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2,int16_t x3, int16_t y3, uint16_t color)
{
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x3, y3, color);
    drawLine(x3, y3, x0, y0, color);
}


/******************************************************************************/
/*!
 Draw a filled quadrilater by connecting 4 points
 Parameters:
 x0:
 y0:
 x1:
 y1:
 x2:
 y2:
 x3:
 y3:
 color: RGB565 color
 triangled: if true a full quad will be generated, false generate a low res quad (faster)
 *NOTE: a bug in _triangle_helper create some problem, still fixing....
 */
/******************************************************************************/
void XGLCD::fillQuad(int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color, bool triangled)
{
    _triangle_helper(x0, y0, x1, y1, x2, y2, color,true);
    if (triangled) _triangle_helper(x2, y2, x3, y3, x0, y0, color,true);
    _triangle_helper(x1, y1, x2, y2, x3, y3, color,true);
}

/******************************************************************************/
/*!
 Draw a polygon from a center
 Parameters:
 cx: x center of the polygon
 cy: y center of the polygon
 sides: how many sides (min 3)
 diameter: diameter of the polygon
 rot: angle rotation of the polygon
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::drawPolygon(int16_t cx, int16_t cy, uint8_t sides, int16_t diameter, float rot, uint16_t color)
{
    _center_helper(cx,cy);
    sides = (sides > 2? sides : 3);
    float dtr = (PI / 180.0) + PI;
    float rads = 360.0 / sides;                                                 //points spacd equally
    uint8_t i;
    for (i = 0; i < sides; i++) {
        drawLine(
                 cx + (sin((i*rads + rot) * dtr) * diameter),
                 cy + (cos((i*rads + rot) * dtr) * diameter),
                 cx + (sin(((i+1)*rads + rot) * dtr) * diameter),
                 cy + (cos(((i+1)*rads + rot) * dtr) * diameter),
                 color);
    }
}

/******************************************************************************/
/*!
 Draw Triangle
 Parameters:
 x0: The 0-based x location of the point 0 of the triangle bottom LEFT
 y0: The 0-based y location of the point 0 of the triangle bottom LEFT
 x1: The 0-based x location of the point 1 of the triangle middle TOP
 y1: The 0-based y location of the point 1 of the triangle middle TOP
 x2: The 0-based x location of the point 2 of the triangle bottom RIGHT
 y2: The 0-based y location of the point 2 of the triangle bottom RIGHT
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    _triangle_helper(x0, y0, x1, y1, x2, y2, color, false);
}

/******************************************************************************/
/*!
 Draw filled Triangle
 Parameters:
 x0: The 0-based x location of the point 0 of the triangle
 y0: The 0-based y location of the point 0 of the triangle
 x1: The 0-based x location of the point 1 of the triangle
 y1: The 0-based y location of the point 1 of the triangle
 x2: The 0-based x location of the point 2 of the triangle
 y2: The 0-based y location of the point 2 of the triangle
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    _triangle_helper(x0, y0, x1, y1, x2, y2, color, true);
}

/******************************************************************************/
/*!
 Draw an ellipse
 Parameters:
 xCenter:   x location of the center of the ellipse
 yCenter:   y location of the center of the ellipse
 longAxis:  Size in pixels of the long axis
 shortAxis: Size in pixels of the short axis
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::drawEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color)
{
    _ellipseCurve_helper(xCenter, yCenter, longAxis, shortAxis, 255, color, false);
}

/******************************************************************************/
/*!
 Draw a filled ellipse
 Parameters:
 xCenter:   x location of the center of the ellipse
 yCenter:   y location of the center of the ellipse
 longAxis:  Size in pixels of the long axis
 shortAxis: Size in pixels of the short axis
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::fillEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color)
{
    _ellipseCurve_helper(xCenter, yCenter, longAxis, shortAxis, 255, color, true);
}

/******************************************************************************/
/*!
 Draw a curve
 Parameters:
 xCenter:]   x location of the ellipse center
 yCenter:   y location of the ellipse center
 longAxis:  Size in pixels of the long axis
 shortAxis: Size in pixels of the short axis
 curvePart: Curve to draw in clock-wise dir: 0[180-270°],1[270-0°],2[0-90°],3[90-180°]
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::drawCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color)
{
    curvePart = curvePart % 4;
    if (_portrait) {
        if (curvePart == 0) {
            curvePart = 2;
        } else if (curvePart == 2) {
            curvePart = 0;
        }
    }
    _ellipseCurve_helper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, false);
}

/******************************************************************************/
/*!
 Draw a filled curve
 Parameters:
 xCenter:]   x location of the ellipse center
 yCenter:   y location of the ellipse center
 longAxis:  Size in pixels of the long axis
 shortAxis: Size in pixels of the short axis
 curvePart: Curve to draw in clock-wise dir: 0[180-270°],1[270-0°],2[0-90°],3[90-180°]
 color: RGB565 color
 */
/******************************************************************************/
void XGLCD::fillCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color)
{
    curvePart = curvePart % 4;
    if (_portrait) {
        if (curvePart == 0) {
            curvePart = 2;
        } else if (curvePart == 2) {
            curvePart = 0;
        }
    }
    _ellipseCurve_helper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, true);
}

/******************************************************************************/
/*!
 Set the position for Graphic Write
 Parameters:
 x: horizontal position
 y: vertical position
 
 Note: X-GRAPH: these are actually internal functions. There is no
 purpose for normal drawing as X,Y are included in all drawing
 functions.
 */
/******************************************************************************/

void XGLCD::setXY(int16_t x, int16_t y)
{
    setX(x);
    setY(y);
}

/******************************************************************************/
/*!
 Set the x position for Graphic Write
 Parameters:
 x: horizontal position
 */
/******************************************************************************/
void XGLCD::setX(int16_t x)
{
    if (x < 0) x = 0;
    if (_portrait){
        if (x >= LCD_HEIGHT) x = LCD_HEIGHT-1;
        _writeRegister(RA8875_CURV0, x & 0xFF);
        _writeRegister(RA8875_CURV0+1, x >> 8);
    } else {
        if (x >= LCD_WIDTH) x = LCD_WIDTH-1;
        _writeRegister(RA8875_CURH0, x & 0xFF);
        _writeRegister(RA8875_CURH0+1, (x >> 8));
    }
}

/******************************************************************************/
/*!
 Set the y position for Graphic Write
 Parameters:
 y: vertical position
 */
/******************************************************************************/
void XGLCD::setY(int16_t y)
{
    if (y < 0) y = 0;
    if (_portrait){
        if (y >= LCD_WIDTH) y = LCD_WIDTH-1;
        _writeRegister(RA8875_CURH0, y & 0xFF);
        _writeRegister(RA8875_CURH0+1, (y >> 8));
    } else {
        if (y >= LCD_HEIGHT) y = LCD_HEIGHT-1;
        _writeRegister(RA8875_CURV0, y & 0xFF);
        _writeRegister(RA8875_CURV0+1, y >> 8);
    }
}


/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                    GEOMETRIC PRIMITIVE HELPERS STUFF                         +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/******************************************************************************/
/*!
 check area of a triangle
 [private]
 Thanks MrTom
 */
/******************************************************************************/
float XGLCD::_check_area(int16_t Ax, int16_t Ay, int16_t Bx, int16_t By, int16_t Cx, int16_t Cy) {
    float area = abs(Ax * (By - Cy) + Bx * (Cy - Ay) + Cx * (Ay - By));         // Calc area
    float mag1 = sqrt((Bx - Ax) * (Bx - Ax) + (By - Ay) * (By - Ay));           // Calc side lengths
    float mag2 = sqrt((Cx - Ax) * (Cx - Ax) + (Cy - Ay) * (Cy - Ay));
    float mag3 = sqrt((Cx - Bx) * (Cx - Bx) + (Cy - By) * (Cy - By));
    float magmax = (mag1>mag2?mag1:mag2)>mag3?(mag1>mag2?mag1:mag2):mag3;       // Find largest length
    return area/magmax;                                                         // Return area
}

/******************************************************************************/
/*!
 helper function for circles
 [private]
 */
/******************************************************************************/
void XGLCD::_circle_helper(int16_t x0, int16_t y0, int16_t r, uint16_t color, bool filled) // fixed an undocumented hardware limit
{
    
    if (r < 1) r = 1;
    if (r < 2) {
        drawPixel(x0,y0,color);
        return;
    }
    
    if (_portrait) swapvals(x0,y0);                                             // X-GRAPH: moved after drawPixel which has its local swaps
    
    if (r > LCD_HEIGHT / 2) r = (LCD_HEIGHT / 2) - 1;                           //this is the (undocumented) hardware limit of RA8875
    
    if (_textMode) _setTextMode(false);                                         //we are in text mode?
    _TXTrecoverColor = true;
    if (color != _foreColor) setForegroundColor(color);                         // avoid several SPI calls
    
    _writeRegister(RA8875_DCHR0,    x0 & 0xFF);
    _writeRegister(RA8875_DCHR0 + 1,x0 >> 8);
    
    _writeRegister(RA8875_DCVR0,    y0 & 0xFF);
    _writeRegister(RA8875_DCVR0 + 1,y0 >> 8);
    _writeRegister(RA8875_DCRR,r);
    
    writeCommand(RA8875_DCR);
    _spisetSpeed(SPI_SPEED_READ);                                               // Note: the DCR_CIRCLE_START command can only be send at SPI speeds < 10MHz -> X-Graph: 7.5MHz is too high, reduced to 3.75MHz
    filled == true ? _writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_FILL) : _writeData(RA8875_DCR_CIRCLE_START | RA8875_DCR_NOFILL);
    _spisetSpeed(SPI_SPEED_WRITE);
    _waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);                            //ZzZzz
    _waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);                            //ZzZzz: X-Graph: do this twice and clock2 sample draws circle completely, why ?
    _waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);                            //ZzZzz: X-Graph: do this three times and drawingTests Example works perfectly
}


/******************************************************************************/
/*!
 helper function for rects (filled or not)
 [private]
 */
/******************************************************************************/

void XGLCD::_rect_helper(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled)
{
    if (_portrait) {swapvals(x1,y1); swapvals(x2,y2);}
    
    if ((x1 < 0 && x2 < 0) || (x1 >= LCD_WIDTH && x2 >= LCD_WIDTH) ||
        (y1 < 0 && y2 < 0) || (y1 >= LCD_HEIGHT && y2 >= LCD_HEIGHT))
        return;                                                                    // All points are out of bounds, don't draw anything
    
    _checkLimits_helper(x1,y1);                                                    // Truncate rectangle that is off screen, still draw remaining rectangle
    _checkLimits_helper(x2,y2);
    
    if (_textMode) _setTextMode(false);                                            //we are in text mode?
    _TXTrecoverColor = true;
    if (color != _foreColor) setForegroundColor(color);
    _line_addressing(x1,y1,x2,y2);
    writeCommand(RA8875_DCR);
    filled == true ? _writeData(0xB0) : _writeData(0x90);
    _waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}


/******************************************************************************/
/*!
 helper function for triangles
 [private]
 */
/******************************************************************************/

void XGLCD::_triangle_helper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled)
{
    if (x0 >= _width || x1 >= _width || x2 >= _width) return;
    if (y0 >= _height || y1 >= _height || y2 >= _height) return;
    
    if (x0 == x1 && y0 == y1 && x0 == x2 && y0 == y2) {                            // X-GRAPH: moved above portrait swap / All points are same
        drawPixel(x0,y0, color);
        return;
    }
    
    if (_portrait) {swapvals(x0,y0); swapvals(x1,y1); swapvals(x2,y2);}
    
    // Avoid drawing lines here due to hardware bug in certain circumstances when a
    // specific shape triangle is drawn after a line. This bug can still happen, but
    // at least the user has control over fixing it.
    // Not drawing a line here is slower, but drawing a non-filled "triangle" is
    // slightly faster than a filled "triangle".
    //
    // bug example: tft.drawLine(799,479, 750,50, RA8875_BLUE)
    //              tft.fillTriangle(480,152, 456,212, 215,410, RA8875_GREEN)
    // MrTom
    //
    if ((x0 == x1 && y0 == y1) || (x0 == x2 && y0 == y2) || (x1 == x2 && y1 == y2)){
        filled = false;                                                            // Two points are same
    } else if (x0 == x1 && x0 == x2){
        filled = false;                                                            // Vertical line
    } else if (y0 == y1 && y0 == y2){
        filled = false;                                                            // Horizontal line
    }
    if (filled){
        if (_check_area(x0,y0, x1,y1, x2,y2) < 0.9) {
            filled = false;                                                        // Draw non-filled triangle to avoid filled triangle bug when two vertices are close together.
        }
    }
    
    if (_textMode) _setTextMode(false);                                         //we are in text mode?
    
    _TXTrecoverColor = true;
    if (color != _foreColor) setForegroundColor(color);                         // avoid several SPI calls
    
    //_checkLimits_helper(x0,y0);
    //_checkLimits_helper(x1,y1);
    
    _line_addressing(x0,y0,x1,y1);
    
    _writeRegister(RA8875_DTPH0,    x2 & 0xFF);
    _writeRegister(RA8875_DTPH0 + 1,x2 >> 8);
    _writeRegister(RA8875_DTPV0,    y2 & 0xFF);
    _writeRegister(RA8875_DTPV0 + 1,y2 >> 8);
    
    writeCommand(RA8875_DCR);
    filled == true ? _writeData(0xA1) : _writeData(0x81);
    
    _waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

/******************************************************************************/
/*!
 helper function for ellipse and curve
 [private]
 curvePart:
 0: bottom-right
 1: top-left
 2: top-right
 3: bottom-left
 */
/******************************************************************************/
void XGLCD::_ellipseCurve_helper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart,uint16_t color, bool filled)
{
    _center_helper(xCenter,yCenter);                                            //use CENTER?
    
    if (longAxis == 1 && shortAxis == 1) {                                      // X-GRAPH: moved above _portrait check
        drawPixel(xCenter,yCenter,color);
        return;
    }
    
    if (_portrait) {
        swapvals(xCenter,yCenter);
        swapvals(longAxis,shortAxis);
        if (longAxis > _height/2) longAxis = (_height / 2) - 1;
        if (shortAxis > _width/2) shortAxis = (_width / 2) - 1;
    } else {
        if (longAxis > _width/2) longAxis = (_width / 2) - 1;
        if (shortAxis > _height/2) shortAxis = (_height / 2) - 1;
    }
    _checkLimits_helper(xCenter,yCenter);
    
    if (_textMode) _setTextMode(false);                                         //we are in text mode?
    
    _TXTrecoverColor = true;
    if (color != _foreColor) setForegroundColor(color);
    
    _curve_addressing(xCenter,yCenter,longAxis,shortAxis);
    writeCommand(RA8875_ELLIPSE);
    
    if (curvePart != 255){
        curvePart = curvePart % 4;
        filled == true ? _writeData(0xD0 | (curvePart & 0x03)) : _writeData(0x90 | (curvePart & 0x03));
    } else {
        filled == true ? _writeData(0xC0) : _writeData(0x80);
    }
    _waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

/******************************************************************************/
/*!
 helper function for rounded Rects
 PARAMETERS
 x:
 y:
 w:
 h:
 r:
 color:
 filled:
 [private]
 */
/******************************************************************************/
void XGLCD::_roundRect_helper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t r, uint16_t color, bool filled)
{
    
    if (_portrait) {swapvals(x0,y0); swapvals(x1,y1);}
    if (_textMode) _setTextMode(false);
    
    _TXTrecoverColor = true;
    if (color != _foreColor) setForegroundColor(color);                         // avoid several SPI calls
    
    
    _line_addressing(x0,y0,x1,y1);
    
    _writeRegister(RA8875_ELL_A0,    r & 0xFF);
    _writeRegister(RA8875_ELL_A0 + 1,r >> 8);
    _writeRegister(RA8875_ELL_B0,    r & 0xFF);
    _writeRegister(RA8875_ELL_B0 + 1,r >> 8);
    
    writeCommand(RA8875_ELLIPSE);
    filled == true ? _writeData(0xE0) : _writeData(0xA0);
    _waitPoll(RA8875_ELLIPSE, RA8875_DCR_LINESQUTRI_STATUS);
}

/******************************************************************************/
/*!
 helper function for draw arcs in degrees
 DrawArc function thanks to Jnmattern and his Arc_2.0 (https://github.com/Jnmattern)
 Adapted for DUE by Marek Buriak https://github.com/marekburiak/ILI9341_Due
 Re-Adapted for this library by sumotoy
 PARAMETERS
 cx: center x
 cy: center y
 radius: the radius of the arc
 thickness:
 start: where arc start in degrees: 0 = North
 end:     where arc end in degrees: rotates clockwise
 color:
 [private]
 */
/******************************************************************************/
void XGLCD::_drawArc_helper(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color) {
    
    //_center_helper(cx,cy);//use CENTER?
    int16_t xmin = 65535, xmax = -32767, ymin = 32767, ymax = -32767;
    float cosStart, sinStart, cosEnd, sinEnd;
    float r, t;
    float startAngle, endAngle;
    
    startAngle = (start / _arcAngle_max) * 360;                                    // 252
    endAngle = (end / _arcAngle_max) * 360;                                        // 807
    
    while (startAngle < 0)   startAngle += 360;
    while (endAngle < 0)     endAngle += 360;
    while (startAngle > 360) startAngle -= 360;
    while (endAngle > 360)   endAngle -= 360;
    
    
    if (startAngle > endAngle) {
        _drawArc_helper(cx, cy, radius, thickness, ((startAngle) / (float)360) * _arcAngle_max, _arcAngle_max, color);
        _drawArc_helper(cx, cy, radius, thickness, 0, ((endAngle) / (float)360) * _arcAngle_max, color);
    } else {
        //if (_textMode) _setTextMode(false);//we are in text mode?
        cosStart = _cosDeg_helper(startAngle);                                  // Calculate bounding box for the arc to be drawn
        sinStart = _sinDeg_helper(startAngle);
        cosEnd = _cosDeg_helper(endAngle);
        sinEnd = _sinDeg_helper(endAngle);
        
        r = radius;
        t = r * cosStart;                                                       // Point 1: radius & startAngle
        if (t < xmin) xmin = t;
        if (t > xmax) xmax = t;
        t = r * sinStart;
        if (t < ymin) ymin = t;
        if (t > ymax) ymax = t;
        
        t = r * cosEnd;                                                         // Point 2: radius & endAngle
        if (t < xmin) xmin = t;
        if (t > xmax) xmax = t;
        t = r * sinEnd;
        if (t < ymin) ymin = t;
        if (t > ymax) ymax = t;
        
        r = radius - thickness;                                                 // Point 3: radius-thickness & startAngle
        t = r * cosStart;
        if (t < xmin) xmin = t;
        if (t > xmax) xmax = t;
        t = r * sinStart;
        if (t < ymin) ymin = t;
        if (t > ymax) ymax = t;
        
        t = r * cosEnd;                                                         // Point 4: radius-thickness & endAngle
        if (t < xmin) xmin = t;
        if (t > xmax) xmax = t;
        t = r * sinEnd;
        if (t < ymin) ymin = t;
        if (t > ymax) ymax = t;
        
        if ((startAngle < 90) && (endAngle > 90)) ymax = radius;                // Corrections if arc crosses X or Y axis
        if ((startAngle < 180) && (endAngle > 180)) xmin = -radius;
        if ((startAngle < 270) && (endAngle > 270)) ymin = -radius;
        
        float sslope = (float)cosStart / (float)sinStart;                       // Slopes for the two sides of the arc
        float eslope = (float)cosEnd / (float)sinEnd;
        if (endAngle == 360) eslope = -1000000;
        int ir2 = (radius - thickness) * (radius - thickness);
        int or2 = radius * radius;
        for (int x = xmin; x <= xmax; x++) {
            bool y1StartFound = false, y2StartFound = false;
            bool y1EndFound = false, y2EndSearching = false;
            int y1s = 0, y1e = 0, y2s = 0;//, y2e = 0;
            for (int y = ymin; y <= ymax; y++) {
                int x2 = x * x;
                int y2 = y * y;
                
                if (
                    (x2 + y2 < or2 && x2 + y2 >= ir2) && (
                                                          (y > 0 && startAngle < 180 && x <= y * sslope) ||
                                                          (y < 0 && startAngle > 180 && x >= y * sslope) ||
                                                          (y < 0 && startAngle <= 180) ||
                                                          (y == 0 && startAngle <= 180 && x < 0) ||
                                                          (y == 0 && startAngle == 0 && x > 0)
                                                          ) && (
                                                                (y > 0 && endAngle < 180 && x >= y * eslope) ||
                                                                (y < 0 && endAngle > 180 && x <= y * eslope) ||
                                                                (y > 0 && endAngle >= 180) ||
                                                                (y == 0 && endAngle >= 180 && x < 0) ||
                                                                (y == 0 && startAngle == 0 && x > 0)))
                {
                    if (!y1StartFound) {                                        //start of the higher line found
                        y1StartFound = true;
                        y1s = y;
                    } else if (y1EndFound && !y2StartFound) {                   //start of the lower line found
                        y2StartFound = true;
                        y2s = y;
                        y += y1e - y1s - 1;                                        // calculate the most probable end of the lower line (in most cases the length of lower line is equal to length of upper line), in the next loop we will validate if the end of line is really there
                        if (y > ymax - 1) {                                     // the most probable end of line 2 is beyond ymax so line 2 must be shorter, thus continue with pixel by pixel search
                            y = y2s;                                            // reset y and continue with pixel by pixel search
                            y2EndSearching = true;
                        }
                    } else if (y2StartFound && !y2EndSearching) {
                        // we validated that the probable end of the lower line has a pixel, continue with pixel by pixel search, in most cases next loop with confirm the end of lower line as it will not find a valid pixel
                        y2EndSearching = true;
                    }
                } else {
                    if (y1StartFound && !y1EndFound) {                          //higher line end found
                        y1EndFound = true;
                        y1e = y - 1;
                        drawFastVLine(cx + x, cy + y1s, y - y1s, color);
                        if (y < 0) {
                            y = abs(y);                                         // skip the empty middle
                        }
                        else
                            break;
                    } else if (y2StartFound) {
                        if (y2EndSearching) {
                            drawFastVLine(cx + x, cy + y2s, y - y2s, color);    // we found the end of the lower line after pixel by pixel search
                            y2EndSearching = false;
                            break;
                        } else {                                                // the expected end of the lower line is not there so the lower line must be shorter
                            y = y2s;                                            // put the y back to the lower line start and go pixel by pixel to find the end
                            y2EndSearching = true;
                        }
                    }
                }
            }
            if (y1StartFound && !y1EndFound){
                y1e = ymax;
                drawFastVLine(cx + x, cy + y1s, y1e - y1s + 1, color);
            } else if (y2StartFound && y2EndSearching)    {                       // we found start of lower line but we are still searching for the end
                // which we haven't found in the loop so the last pixel in a column must be the end
                drawFastVLine(cx + x, cy + y2s, ymax - y2s + 1, color);
            }
        }
    }
}

/******************************************************************************/
/*!PRIVATE
 helper sub-functions
 */
/******************************************************************************/

void XGLCD::_checkLimits_helper(int16_t &x,int16_t &y)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= LCD_WIDTH) x = LCD_WIDTH - 1;
    if (y >= LCD_HEIGHT) y = LCD_HEIGHT -1;
    x = x;
    y = y;
}

void XGLCD::_center_helper(int16_t &x, int16_t &y)
{
    if (x == CENTER) x = _width/2;
    if (y == CENTER) y = _height/2;
}

/******************************************************************************/
/*!
 Graphic line addressing helper
 [private]
 */
/******************************************************************************/
void XGLCD::_line_addressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    //X0
    _writeRegister(RA8875_DLHSR0,    x0 & 0xFF);
    _writeRegister(RA8875_DLHSR0 + 1,x0 >> 8);
    //Y0
    _writeRegister(RA8875_DLVSR0,    y0 & 0xFF);
    _writeRegister(RA8875_DLVSR0 + 1,y0 >> 8);
    //X1
    _writeRegister(RA8875_DLHER0,    x1 & 0xFF);
    _writeRegister(RA8875_DLHER0 + 1,x1 >> 8);
    //Y1
    _writeRegister(RA8875_DLVER0,    y1 & 0xFF);
    _writeRegister(RA8875_DLVER0 + 1,y1 >> 8);
}

/******************************************************************************/
/*!
 curve addressing helper
 [private]
 */
/******************************************************************************/
void XGLCD::_curve_addressing(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    //center
    _writeRegister(RA8875_DEHR0,    x0 & 0xFF);
    _writeRegister(RA8875_DEHR0 + 1,x0 >> 8);
    _writeRegister(RA8875_DEVR0,    y0 & 0xFF);
    _writeRegister(RA8875_DEVR0 + 1,y0 >> 8);
    //long,short ax
    _writeRegister(RA8875_ELL_A0,    x1 & 0xFF);
    _writeRegister(RA8875_ELL_A0 + 1,x1 >> 8);
    _writeRegister(RA8875_ELL_B0,    y1 & 0xFF);
    _writeRegister(RA8875_ELL_B0 + 1,y1 >> 8);
}

/******************************************************************************/
/*!
 sin e cos helpers
 [private]
 */
/******************************************************************************/
float XGLCD::_cosDeg_helper(float angle)
{
    float radians = angle / (float)360 * 2 * PI;
    return cos(radians);
}

float XGLCD::_sinDeg_helper(float angle)
{
    float radians = angle / (float)360 * 2 * PI;
    return sin(radians);
}


/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                        GUI ELEMENTS                                         +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
A sample of some GUI elements.
In future versions of this library these functions will be moved to a
seperate GUI file.
*/

/******************************************************************************/
/*!
 ringMeter
 (adapted from Alan Senior (thanks man!))
 it create a ring meter with a lot of personalizations,
 it return the width of the gauge so you can use this value
 for positioning other gauges near the one just created easily
 Parameters:
 val:  your value
 minV: the minimum value possible
 maxV: the max value possible
 x:    the position on x axis
 y:    the position on y axis
 r:    the radius of the gauge (minimum 50)
 units: a text that shows the units, if "none" all text will be avoided
 scheme:0...7 or 16 bit color (not BLACK or WHITE)
 0:red
 1:green
 2:blue
 3:blue->red
 4:green->red
 5:red->green
 6:red->green->blue
 7:cyan->green->red
 8:black->white linear interpolation
 9:violet->yellow linear interpolation
 or
 RGB565 color (not BLACK or WHITE)
 backSegColor: the color of the segments not active (default BLACK)
 angle:        90 -> 180 (the shape of the meter, 90:halfway, 180:full round, 150:default)
 inc:             5...20 (5:solid, 20:sparse divisions, default:10)
 */
/******************************************************************************/
void XGLCD::ringMeter(int val, int minV, int maxV, int16_t x, int16_t y, uint16_t r, const char* units, uint16_t colorScheme,uint16_t backSegColor,int16_t angle,uint8_t inc)
{
    if (inc < 5) inc = 5;
    if (inc > 20) inc = 20;
    if (r < 50) r = 50;
    if (angle < 90) angle = 90;
    if (angle > 180) angle = 180;
    int curAngle = map(val, minV, maxV, -angle, angle);
    uint16_t colour;
    x += r;
    y += r;                                                                     // Calculate coords of centre of ring
    uint16_t w = r / 4;                                                         // Width of outer ring is 1/4 of radius
    const uint8_t seg = 5;                                                      // Segments are 5 degrees wide = 60 segments for 300 degrees
    for (int16_t i = -angle; i < angle; i += inc) {                             // Draw colour blocks every inc degrees
        colour = COLOR_BLACK;
        switch (colorScheme) {
            case 0:
                colour = COLOR_RED;
                break;
            case 1:
                colour = COLOR_GREEN;
                break;
            case 2:
                colour = COLOR_BLUE;
                break;
            case 3:
                colour = gradient(map(i, -angle, angle, 0, 127));               // Full spectrum blue to red
                break;
            case 4:
                colour = gradient(map(i, -angle, angle, 63, 127));              // Green to red (high temperature etc)
                break;
            case 5:
                colour = gradient(map(i, -angle, angle, 127, 63));              // Red to green (low battery etc)
                break;
            case 6:
                colour = gradient(map(i, -angle, angle, 127, 0));               // Red to blue (air cond reverse)
                break;
            case 7:
                colour = gradient(map(i, -angle, angle, 35, 127));              // cyan to red
                break;
            case 8:
                colour = colorInterpolation(0,0,0,255,255,255,map(i,-angle,angle,0,w),w); // black to white
                break;
            case 9:
                colour = colorInterpolation(0x80,0,0xC0,0xFF,0xFF,0,map(i,-angle,angle,0,w),w); // violet to yellow
                break;
            default:
                if (colorScheme > 9){
                    colour = colorScheme;
                } else {
                    colour = COLOR_BLUE;
                }
                break;
        }
        
        float xStart = cos((i - 90) * 0.0174532925);                            // Calculate pair of coordinates for segment start
        float yStart = sin((i - 90) * 0.0174532925);
        uint16_t x0 = xStart * (r - w) + x;
        uint16_t y0 = yStart * (r - w) + y;
        uint16_t x1 = xStart * r + x;
        uint16_t y1 = yStart * r + y;
        
        
        float xEnd = cos((i + seg - 90) * 0.0174532925);                        // Calculate pair of coordinates for segment end
        float yEnd = sin((i + seg - 90) * 0.0174532925);
        int16_t x2 = xEnd * (r - w) + x;
        int16_t y2 = yEnd * (r - w) + y;
        int16_t x3 = xEnd * r + x;
        int16_t y3 = yEnd * r + y;
        
        if (i < curAngle) {                                                     // Fill in coloured segments with 2 triangles
            fillQuad(x0, y0, x1, y1, x2, y2, x3, y3, colour, false);
        } else {                                                                // Fill in blank segments
            fillQuad(x0, y0, x1, y1, x2, y2, x3, y3, backSegColor, false);
        }
    }
    
    if (strcmp(units, "none") != 0){                                            // text
        if (angle > 90) {                                                       //erase internal background
            fillCircle(x, y, r - w, _backColor);
        } else {
            fillCurve(x, y + getFontHeight() / 2, r - w, r - w, 1, _backColor);
            fillCurve(x, y + getFontHeight() / 2, r - w, r - w, 2, _backColor);
        }
        if (r > 84) {                                                           //prepare for write text
            setFontScale(1);
        } else {
            setFontScale(0);
        }
        if (_portrait){
            setCursor(y, x - 15, true);
        } else {
            setCursor(x - 15, y, true);
        }
        //XG// print(val);
        //XG// print(" ");
        //XG// print(units);
    }
    
    //return x + r;                                                             // Calculate and return right hand side x coordinate
}

void XGLCD::roundGaugeTicker(uint16_t x, uint16_t y, uint16_t r, int from, int to, float dev,uint16_t color)
{
    float dsec;
    int i;
    for (i = from; i <= to; i += 30) {
        dsec = i * (PI / 180);
        drawLine(
                 x + (cos(dsec) * (r / dev)) + 1,
                 y + (sin(dsec) * (r / dev)) + 1,
                 x + (cos(dsec) * r) + 1,
                 y + (sin(dsec) * r) + 1,
                 color);
    }
}

void XGLCD::drawBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t * image, uint16_t scale) {
    // To be implemented
}
void XGLCD::drawBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t * image, uint16_t degree, uint16_t rx, uint16_t ry) {
    // To be implemented (rotation version)
}

// drawBitmap only supports 16-bit color and rotation=0 (for littlevgl support) to speed up things
// if (_textMode) _setTextMode(false); is not added, not supported for textmode is needed for LittleVGL anyhow

void XGLCD::drawBitmap(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t * image) {
    uint32_t count;

    uint32_t i;
    i = (uint32_t)image;
    
    setActiveWindow(x1,x2,y1,y2);
    setXY(x1,y1);
    count = (x2 - x1 + 1) * (y2 - y1 + 1);
    writeCommand(RA8875_MRWC);
    _spiCSLow;
#ifdef _spixwritedma
    _spixwritedma(RA8875_DATAWRITE, image, count);
#else
    _spiwrite(RA8875_DATAWRITE);
    _spiwritedma(image, count);
#endif
    _spiCSHigh;
}

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								TEXT STUFF							           +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Because drawing fonts requires moving a lot of pixels on the screen this is
one of the slowest drawing primitives. The RA8875 offers several solutions.
1. It has 4 build-in build-in fonts which can be used in 4 sizes.
2. It has a special ram area (CGRAM) where one can store one 'user' font. This
 can be used for example for pictograms.
3. External (hardware chips) fonts can be added, but this is not supported
 in the X-Graph modules.
4. You can also always use software based fonts ('rendered' fonts).
 This library includes a rather faster font drawing algorithm and includes
 several software fonts. New fonts can be created if needed on a personal
 computer, and then important. Note that every included font requires a
 substantial amount of flash storage.
 
The RA8875 also supports a hardware cursor, blinking or not. This is usefull
 for input boxes and doesn't require continuous read/writes to the video
 memory.
*/

/******************************************************************************/
/*!
 Change the mode between graphic and text
 Parameters:
 true = text mode / false = graphic mode
 [private]
 */
/******************************************************************************/
void XGLCD::_setTextMode(bool m)
{
    if (m == _textMode) return;
    writeCommand(RA8875_MWCR0);
    if (m){
        _MWCR0_Reg |= RA8875_MWCR0_TXTMODE;
        _textMode = true;
    } else {
        _MWCR0_Reg &= ~RA8875_MWCR0_TXTMODE;
        _textMode = false;
    }
    _writeData(_MWCR0_Reg);
}

/******************************************************************************/
/*!		Upload user custom char or symbol to CGRAM, max 255
		Parameters:
		symbol[]: an 8bit x 16 char in an array. Must be exact 16 bytes
		address: 0...255 the address of the CGRAM where to store the char
*/
/******************************************************************************/
void XGLCD::uploadUserChar(const uint8_t symbol[],uint8_t address)
{
	uint8_t tempMWCR1 = _readRegister(RA8875_MWCR1);
	uint8_t i;
	if (_textMode) _setTextMode(false);
	_writeRegister(RA8875_CGSR,address);
	writeTo(CGRAM);
	writeCommand(RA8875_MRWC);
	for (i=0;i<16;i++){
		_writeData(symbol[i]);
	}
	_writeRegister(RA8875_MWCR1, tempMWCR1);
}

/******************************************************************************/
/*!		Retrieve and print to screen the user custom char or symbol
		User have to store a custom char before use this function
		Parameters:
		address: 0...255 the address of the CGRAM where char it's stored
		wide:0 for single 8x16 char, if you have wider chars that use 
		more than a char slot they can be showed combined (see examples)
*/
/******************************************************************************/
void XGLCD::showUserChar(uint8_t symbolAddrs,uint8_t wide)
{
	if (!_textMode) _setTextMode(true);
	uint8_t oldReg1State = _FNCR0_Reg;
	uint8_t oldReg2State = 0;
	uint8_t i;
	oldReg1State |= RA8875_FNCR0_CGRAM + RA8857_FNCR0_EXTERNAL_CGROM;
	_writeRegister(RA8875_FNCR0,oldReg1State);
	if (_scaling){                                                              //reset scale (not compatible with this!)
		oldReg2State = _FNCR1_Reg;
		oldReg2State &= ~RA8875_FNCR1_SCALE_MASK;
		_writeRegister(RA8875_FNCR1,oldReg2State);
	}

    if (_useMultiLayers){
		if (_currentLayer == 0){
			writeTo(L1);
		} else {
			writeTo(L2);
		}
	} else {
		//writeTo(L1);
	}
    
	writeCommand(RA8875_MRWC);
	_writeData(symbolAddrs);
	if (wide > 0){
		for (i=1;i<=wide;i++){
			_writeData(symbolAddrs+i);
		}
	}
	if (oldReg2State != 0) _writeRegister(RA8875_FNCR1,_FNCR1_Reg);             //put back scale as it was
	if (oldReg1State != _FNCR0_Reg) _writeRegister(RA8875_FNCR0,_FNCR0_Reg);    //put back state
}

/******************************************************************************/
/*!   
		Set internal Font Encoding
        Parameters:
		f: ISO_IEC_8859_1, ISO_IEC_8859_2, ISO_IEC_8859_3, ISO_IEC_8859_4
		default: ISO_IEC_8859_1
*/
/******************************************************************************/
void XGLCD::setIntFontCoding(enum RA8875fontCoding f)
{
	uint8_t temp = _FNCR0_Reg;
	temp &= ~(RA8875_FNCR0_8859_MASK);
    switch (f){
		case ISO_IEC_8859_1:
			 //do nothing
		break;
		case ISO_IEC_8859_2:
			temp |= RA8857_FNCR0_8859_2;
		break;
		case ISO_IEC_8859_3:
			temp |= RA8857_FNCR0_8859_3;
		break;
		case ISO_IEC_8859_4:
			temp |= RA8857_FNCR0_8859_4;
        break;
		default:
		return;
	}
	_FNCR0_Reg = temp;
	_writeRegister(RA8875_FNCR0,_FNCR0_Reg);
}

/******************************************************************************/
/*!  
		choose internal Font Rom
*/
/******************************************************************************/
void XGLCD::setFont(void)
{
	if (!_textMode) _setTextMode(true);
	_TXTrender      = false;
    _FNTwidth 		= 	8;
    _FNTheight 		= 	16;
    _FNTbaselineLow  = 	2;
    _FNTbaselineTop  = 	4;
	_FNCR0_Reg &= ~(RA8875_FNCR0_CG_MASK);
	_writeRegister(RA8875_FNCR0,_FNCR0_Reg);
	delay(1);
	_spaceCharWidth = _FNTwidth;
	//setFontScale(0);
	_scaleX = 1; _scaleY = 1;                                                   //reset font scale
}

/******************************************************************************/
/*!  
		choose an external font that will be rendered
		Of course slower that internal fonts!
		Parameters:
		*font: &myfont
*/
/******************************************************************************/

void XGLCD::setFont(const tFont *font)
{
	_currentFont = font;
	_FNTheight = 		_currentFont->font_height;
	_FNTwidth = 		_currentFont->font_width;                               //if 0 it's variable width font
	_FNTcompression = 	_currentFont->rle;
    if (_FNTwidth > 0){                                                         //get all needed information
		_spaceCharWidth = _FNTwidth;
	} else {
		
		
		int temp = _getCharCode(0x20);                                          //_FNTwidth will be 0 to inform other functions that this it's a variable w font
		if (temp > -1){                                                         // We just get the space width now...
		_spaceCharWidth = (_currentFont->chars[temp].image->image_width);
		} else {
			setFont();                                                          //font malformed, doesn't have needed space parameter will return to system font
			return;
		}
	}
	_scaleX = 1; _scaleY = 1;                                                   //reset font scale
	//setFontScale(0);
    _TXTrender = true;
}

/******************************************************************************/
/*!  
		Enable/Disable the Font Full Alignment feature (default off)
		Parameters:
		align: true,false
		Note: not active with rendered fonts
*/
/******************************************************************************/
void XGLCD::setFontFullAlign(boolean align)
{
	if (!_TXTrender){
		align == true ? _FNCR1_Reg |= RA8875_FNCR1_ALIGNMENT_ON : _FNCR1_Reg &= ~RA8875_FNCR1_ALIGNMENT_ON;
		_writeRegister(RA8875_FNCR1,_FNCR1_Reg);
	}
}

/******************************************************************************/
/*!  
		Set distance between text lines (default off)
		Parameters:
		pix: 0...63 pixels
		Note: active with rendered fonts
*/
/******************************************************************************/
void XGLCD::setFontInterline(uint8_t pix)
{
	if (_TXTrender){
		_FNTinterline = pix;
	} else {
		if (pix > 0x3F) pix = 0x3F;
		_FNTinterline = pix;
		//_FWTSET_Reg &= 0xC0;
		//_FWTSET_Reg |= spc & 0x3F; 
		_writeRegister(RA8875_FLDR,_FNTinterline);
	}	
}

/******************************************************************************/
/*!   
		Set the Text position for write Text only.
		Parameters:
		x:horizontal in pixels or CENTER(of the screen)
		y:vertical in pixels or CENTER(of the screen)
		autocenter:center text to choosed x,y regardless text lenght
		false: |ABCD
		true:  AB|CD
		NOTE: works with any font
*/
/******************************************************************************/
void XGLCD::setCursor(int16_t x, int16_t y,bool autocenter)
{
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	
	_absoluteCenter = autocenter;
	
	if (_portrait) {                                                            //rotation 1,3
		swapvals(x,y);
		if (y == CENTER) {                                                      //swapped OK
			y = _width/2;
			if (!autocenter) {
				_relativeCenter = true;
				_TXTAlignYToCenter = true;
			}
		}
		if (x == CENTER) {                                                      //swapped
			x = _height/2;
			if (!autocenter) {
				_relativeCenter = true;
                _TXTAlignXToCenter = true;
			}
		}
	} else {                                                                    //rotation 0,2
		if (x == CENTER) {
			x = _width/2;
			if (!autocenter) {
				_relativeCenter = true;
                _TXTAlignXToCenter = true;
			}
		}
		if (y == CENTER) {
			y = _height/2;
			if (!autocenter) {
				_relativeCenter = true;
                _TXTAlignYToCenter = true;
			}
		}
	}
	
	if (!_TXTwrap){                                                             // Text wrap is currently always on, so this is useless for now
		if (x >= _width) x = _width-1;
		if (y >= _height) y = _height-1;
	}
	
	_cursorX = x;
	_cursorY = y;

	if (_relativeCenter || _absoluteCenter) return;                             // if _relativeCenter or _absoluteCenter do not apply to registers yet!
                                                                                // Have to go to _textWrite first to calculate the lenght of the entire string and recalculate the correct x,y
    if (!_TXTrender) _textPosition(x,y,false);
}

/******************************************************************************/
/*!   
		Set the x,y position for text only
		Parameters:
		x: horizontal pos in pixels
		y: vertical pos in pixels
		update: true track the actual text position internally
		note: not active with rendered fonts, just set x,y internal tracked param
		[private]
*/
/******************************************************************************/
void XGLCD::_textPosition(int16_t x, int16_t y,bool update)
{
    _writeRegister(RA8875_F_CURXL,(x & 0xFF));
	_writeRegister(RA8875_F_CURXH,(x >> 8));
	_writeRegister(RA8875_F_CURYL,(y & 0xFF));
	_writeRegister(RA8875_F_CURYH,(y >> 8));
    if (update){ _cursorX = x; _cursorY = y;}
}

/******************************************************************************/
/*!   
		Give you back the current text cursor position by reading inside RA8875
		Parameters:
		x: horizontal pos in pixels
		y: vertical pos in pixels
		note: works also with rendered fonts
		USE: xxx.getCursor(myX,myY);
*/
/******************************************************************************/
void XGLCD::getCursor(int16_t &x, int16_t &y)
{
	if (_TXTrender) {
		getCursorFast(x,y);
	} else {
		uint8_t t1,t2,t3,t4;
		t1 = _readRegister(RA8875_F_CURXL);
		t2 = _readRegister(RA8875_F_CURXH);
		t3 = _readRegister(RA8875_F_CURYL);
		t4 = _readRegister(RA8875_F_CURYH);
		x = (t2 << 8) | (t1 & 0xFF);
		y = (t4 << 8) | (t3 & 0xFF);
		if (_portrait) swapvals(x,y);
	}
}

/******************************************************************************/
/*!   
		Give you back the current text cursor position as tracked by library (fast)
		Parameters:
		x: horizontal pos in pixels
		y: vertical pos in pixels
		note: works also with rendered fonts
		USE: xxx.getCursor(myX,myY);
*/
/******************************************************************************/
void XGLCD::getCursorFast(int16_t &x, int16_t &y)
{
	x = _cursorX;
	y = _cursorY;
	if (_portrait) swapvals(x,y);
}

int16_t XGLCD::getCursorX(void)
{
	if (_portrait) return _cursorY;
	return _cursorX;
}

int16_t XGLCD::getCursorY(void)
{
	if (_portrait) return _cursorX;
	return _cursorY;
}

/******************************************************************************/
/*!     Show/Hide text cursor
		Parameters:
		c: cursor type (NOCURSOR,IBEAM,UNDER,BLOCK)
		note: not active with rendered fonts
		blink: true=blink cursor
*/
/******************************************************************************/
void XGLCD::showCursor(enum RA8875tcursor c,bool blink)
{
    //uint8_t MWCR1Reg = _readRegister(RA8875_MWCR1) & 0x01;(needed?)
    uint8_t cW = 0;
    uint8_t cH = 0;
	_FNTcursorType = c;
	c == NOCURSOR ? _MWCR0_Reg &= ~RA8875_MWCR0_CURSOR : _MWCR0_Reg |= RA8875_MWCR0_CURSOR;
    if (blink) _MWCR0_Reg |= RA8875_MWCR0_CURSOR_BLINK;                         //blink or not?
    _writeRegister(RA8875_MWCR0, _MWCR0_Reg);                                   //set cursor
    //_writeRegister(RA8875_MWCR1, MWCR1Reg);//close graphic cursor(needed?)
    switch (c) {
        case IBEAM:
            cW = 0x01;
            cH = 0x1F;
            break;
        case UNDER:
            cW = 0x07;
            cH = 0x01;
            break;
        case BLOCK:
            cW = 0x07;
            cH = 0x1F;
            break;
        case NOCURSOR:
        default:
            break;
    }
	_writeRegister(RA8875_CURHS, cW);                                           //set cursor size
    _writeRegister(RA8875_CURVS, cH);
}

/******************************************************************************/
/*!        Set graphic cursor beween 8 different ones.
 Graphic cursors has to be inserted before use!
 Parameters:
 cur: 0...7
 
 Note: X-GRAPH: graphic curosr write data function is lacking
 */
/******************************************************************************/
void XGLCD::setGraphicCursor(uint8_t cur)
{
    if (cur > 7) cur = 7;
    uint8_t temp = _readRegister(RA8875_MWCR1);
    temp &= ~(0x70);
    temp |= cur << 4;
    temp |= cur;
    if (_useMultiLayers){
        _currentLayer == 1 ? temp |= (1 << 0) : temp &= ~(1 << 0);
    } else {
        temp &= ~(1 << 0);
    }
    _writeData(temp);
}

/******************************************************************************/
/*!        Show the graphic cursor
 Graphic cursors has to be inserted before use!
 Parameters:
 cur: true,false
 */
/******************************************************************************/
void XGLCD::showGraphicCursor(boolean cur)
{
    uint8_t temp = _readRegister(RA8875_MWCR1);
    cur == true ? temp |= (1 << 7) : temp &= ~(1 << 7);
    if (_useMultiLayers){
        _currentLayer == 1 ? temp |= (1 << 0) : temp &= ~(1 << 0);
    } else {
        temp &= ~(1 << 0);
    }
    _writeData(temp);
}

/******************************************************************************/
/*!     Set cursor property blink and his rate
		Parameters:
		rate: blink speed (fast 0...255 slow)
		note: not active with rendered fonts
*/
/******************************************************************************/
void XGLCD::setCursorBlinkRate(uint8_t rate)
{
	_writeRegister(RA8875_BTCR,rate);
}

/******************************************************************************/
/*!
 Normally at every char the cursor advance by one
 You can stop/enable this by using this function
 Parameters:
 on: true(auto advance - default), false:(stop auto advance)
 Note: Inactive with rendered fonts
 */
/******************************************************************************/
void XGLCD::cursorIncrement(bool on)
{
    if (!_TXTrender){
        on == true ? _MWCR0_Reg &= ~RA8875_MWCR0_MEMWR_NO_INC : _MWCR0_Reg |= RA8875_MWCR0_MEMWR_NO_INC;
        //bitWrite(_TXTparameters,1,on);
        _writeRegister(RA8875_MWCR0,_MWCR0_Reg);
    }
}

/******************************************************************************/
/*!		
		set the text color and his background
		Parameters:
		fcolor: 16bit foreground color (text) RGB565
		bcolor: 16bit background color RGB565
		NOTE: will set background transparent OFF
		It also works with rendered fonts.
*/
/******************************************************************************/
void XGLCD::setTextColor(uint16_t fcolor, uint16_t bcolor)
{
	if (fcolor != _TXTForeColor) {
		_TXTForeColor = fcolor;
		setForegroundColor(fcolor);
	}
	if (bcolor != _TXTBackColor) {
		_TXTBackColor = bcolor;
		setBackgroundColor(bcolor);
	}
	_backTransparent = false;

	if (!_TXTrender) {
		_FNCR1_Reg &= ~RA8875_FNCR1_TRANSPARENT_ON;
		_writeRegister(RA8875_FNCR1,_FNCR1_Reg);
	}
}

/******************************************************************************/
/*!		
		set the text color with transparent background
		Parameters:
		fColor: 16bit foreground color (text) RGB565
		NOTE: will set background transparent ON
		It also works with rendered fonts.
*/
/******************************************************************************/

void XGLCD::setTextColor(uint16_t fcolor)
{
	if (fcolor != _TXTForeColor) {
		_TXTForeColor = fcolor;
		setForegroundColor(fcolor);
	}
	_backTransparent = true;

	if (!_TXTrender) {
		_FNCR1_Reg |= RA8875_FNCR1_TRANSPARENT_ON;//set
		_writeRegister(RA8875_FNCR1,_FNCR1_Reg);
	}
}

void XGLCD::setTextGradient(uint16_t fcolor1,uint16_t fcolor2)
{
	_FNTgradient = true;
	_FNTgradientColor1 = fcolor1;
	_FNTgradientColor2 = fcolor2;
}
/******************************************************************************/
/*!		
		Set the Text size by it's multiple. normal should=0, max is 3 (x4) for internal fonts
		With Rendered fonts the max scale it's not limited
		Parameters:
		scale: 0..3  -> 0:normal, 1:x2, 2:x3, 3:x4
*/
/******************************************************************************/
void XGLCD::setFontScale(uint8_t scale)
{
	setFontScale(scale,scale);
}

/******************************************************************************/
/*!		
		Set the Text size by it's multiple. normal should=0, max is 3 (x4) for internal fonts
		With Rendered fonts the max scale it's not limited
		This time you can specify different values for vertical and horizontal
		Parameters:
		xscale: 0..3  -> 0:normal, 1:x2, 2:x3, 3:x4 for internal fonts - 0...xxx for Rendered Fonts
		yscale: 0..3  -> 0:normal, 1:x2, 2:x3, 3:x4 for internal fonts - 0...xxx for Rendered Fonts
*/
/******************************************************************************/
void XGLCD::setFontScale(uint8_t xscale,uint8_t yscale)
{
	_scaling = false;
	if (!_TXTrender){
		xscale = xscale % 4;
		yscale = yscale % 4;
		_FNCR1_Reg &= ~RA8875_FNCR1_SCALE_MASK;
		_FNCR1_Reg |= xscale << 2;
		_FNCR1_Reg |= yscale;
		_writeRegister(RA8875_FNCR1,_FNCR1_Reg);
	}
    _scaleX = xscale + 1;
	_scaleY = yscale + 1;
	if (_scaleX > 1 || _scaleY > 1) _scaling = true;
}

/******************************************************************************/
/*!
 Choose space in pixels between chars
 Parameters:
 spc: 0...63pix (default 0=off)
 TODO: modded for Rendered Fonts
 */
/******************************************************************************/
void XGLCD::setFontSpacing(uint8_t spc)
{
    if (spc > RA8875_FWTSET_WIDTH_MASK) spc = RA8875_FWTSET_WIDTH_MASK;
    _FNTspacing = spc;
    if (!_TXTrender){
        _FWTSET_Reg &= ~RA8875_FWTSET_WIDTH_MASK;
        _FWTSET_Reg |= spc & RA8875_FWTSET_WIDTH_MASK;
        _writeRegister(RA8875_FWTSET,_FWTSET_Reg);
    }
}

/******************************************************************************/
/*!		
		return the current width of the font in pixel
		If font it's scaled, it will multiply.
		It's a fast business since the register it's internally tracked
		It can also return the usable rows based on the actual fontWidth
		Parameters: inColums (true:returns max colums)
		TODO: modded for Rendered Fonts
*/
/******************************************************************************/
uint8_t XGLCD::getFontWidth(boolean inColums)
{
	uint8_t temp;

	if (!_TXTrender){
		temp = (((_FNCR1_Reg >> 2) & 0x3) + 1) * _FNTwidth;
    } else {
        temp = _FNTwidth;
        if (temp < 1) return 0; //variable with
    }
    
    if (inColums){
		if (_scaleX < 2) return (_width / temp);
		temp = temp * _scaleX;
		return (_width / temp);
	} else {
		if (_scaleX < 2) return temp;
		temp = temp * _scaleX;
		return temp;
	}
}

/******************************************************************************/
/*!		
		return the current height of the font in pixel
		If font it's scaled, it will multiply.
		It's a fast business since the register it's internally tracked
		It can also return the usable rows based on the actual fontHeight
		Parameters: inRows (true:returns max rows)
		TODO: modded for Rendered Fonts
*/		
/******************************************************************************/
uint8_t XGLCD::getFontHeight(boolean inRows)
{
	uint8_t temp;

	if (!_TXTrender){
		temp = (((_FNCR1_Reg >> 0) & 0x3) + 1) * _FNTheight;
    } else {
        temp = _FNTheight;
    }

    if (inRows){
		if (_scaleY < 2) return (_height / temp);
		temp = temp * _scaleY;
		return (_height / temp);
	} else {
		if (_scaleY < 2) return temp;
		temp = temp * _scaleY;
		return temp;
	}
}

/******************************************************************************/
/*!	PRIVATE
		draw a string
		Works for all fonts, internal, ROM, external (render)
*/
/******************************************************************************/

void XGLCD::_textWrite(const char* buffer, uint16_t len)
{
    uint16_t i;
	if (len == 0) len = strlen(buffer);                                         //try get the info from the buffer
	if (len == 0) return;                                                       //better stop here, the string is empty!
	
	uint8_t loVOffset = 0;
	uint8_t hiVOffset = 0;
	uint8_t interlineOffset = 0;
	uint16_t fcolor = _foreColor;
	uint16_t bcolor = _backColor;
	uint16_t strngWidth = 0;
	uint16_t strngHeight = 0;
	if (!_TXTrender){
		loVOffset = _FNTbaselineLow * _scaleY;                                  //calculate lower baseline
		hiVOffset = _FNTbaselineTop * _scaleY;                                  //calculate topline
                                                                                //now check for offset if using an external fonts rom (RA8875 bug)
    }
	
	//_absoluteCenter or _relativeCenter cases...................
	//plus calculate the real width & height of the entire text in render mode (not trasparent)
	if (_absoluteCenter || _relativeCenter || (_TXTrender && !_backTransparent)){
		strngWidth = _STRlen_helper(buffer,len) * _scaleX;                      //this calculates the width of the entire text
		strngHeight = (_FNTheight * _scaleY) - (loVOffset + hiVOffset);         //the REAL heigh
		if (_absoluteCenter && strngWidth > 0){                                 //Avoid operations for strngWidth = 0
			_absoluteCenter = false;
			_cursorX = _cursorX - (strngWidth / 2);
            _cursorY = _cursorY - (strngHeight / 2) - hiVOffset;
            if (_portrait) swapvals(_cursorX,_cursorY);
		} else if (_relativeCenter && strngWidth > 0){                          //Avoid operations for strngWidth = 0
			_relativeCenter = false;
			if (_TXTAlignXToCenter) {
				if (!_portrait){
					_cursorX = (_width / 2) - (strngWidth / 2);
                } else {
					_cursorX = (_height / 2) - (strngHeight / 2) - hiVOffset;
                }
				_TXTAlignXToCenter = false;
			}
			if (_TXTAlignYToCenter) {
				if (!_portrait){
					_cursorY = (_height / 2) - (strngHeight / 2) - hiVOffset;
                } else {
					_cursorY = (_width / 2) - (strngWidth / 2);
                }
                _TXTAlignYToCenter = false;
			}
		}
        //if (_cursorX < 0) _cursorX = 0;                                       // X-Graph border check
        //if (_cursorY < 0) _cursorY = 0;                                       // X-Graph border check
		//if ((_absoluteCenter || _relativeCenter) &&  strngWidth > 0){         //Avoid operations for strngWidth = 0
		if (strngWidth > 0){//Avoid operations for strngWidth = 0
			_textPosition(_cursorX,_cursorY,false);
		}

	}
    
	if (!_textMode && !_TXTrender) _setTextMode(true);
	if (_textMode && _TXTrender)   _setTextMode(false);
    
	uint16_t gradientLen = 0;
	uint16_t gradientIndex = 0;
	uint16_t recoverColor = fcolor;

    fcolor = _TXTForeColor;
    bcolor = _TXTBackColor;
	if (_textMode && _TXTrecoverColor){
		if (_foreColor != _TXTForeColor) {_TXTrecoverColor = false;setForegroundColor(_TXTForeColor);}
		if (_backColor != _TXTBackColor) {_TXTrecoverColor = false;setBackgroundColor(_TXTBackColor);}
    }
	if (_FNTgradient){                                                          //coloring text
		recoverColor = _TXTForeColor;
		for (i=0;i<len;i++){                                                    //avoid non char in color index
			if (buffer[i] != 13 && buffer[i] != 10 && buffer[i] != 32) gradientLen++;   //length of the interpolation
		}
	}
                                                                                //instead write the background by using pixels (trough text rendering) better this trick
    if (_TXTrender && !_backTransparent && strngWidth > 0) fillRect(_cursorX,_cursorY,strngWidth,strngHeight,_backColor);
	for (i=0;i<len;i++){                                                        //Loop trough every char and write them one by one...
		if (_FNTgradient){
			if (buffer[i] != 13 && buffer[i] != 10 && buffer[i] != 32){
				if (!_TXTrender){
					setTextColor(colorInterpolation(_FNTgradientColor1,_FNTgradientColor2,gradientIndex++,gradientLen));
				} else {
					fcolor = colorInterpolation(_FNTgradientColor1,_FNTgradientColor2,gradientIndex++,gradientLen);
				}
			}
		}
		if (!_TXTrender) _charWrite(buffer[i],interlineOffset);		            // internal,ROM fonts
        if (_TXTrender)                                                         // X-Graph only draw text after a font is selected
            _charWriteR(buffer[i],interlineOffset,fcolor,bcolor);               // user fonts
    }
    
	if (_FNTgradient){                                                          //recover text color after colored text
		_FNTgradient = false;
        if (!_TXTrender){                                                       //recover original text color
            setTextColor(recoverColor,_backColor);
		} else {
			fcolor = recoverColor;
		}
	 }
}

/******************************************************************************/
/*!	PRIVATE
	Main routine that write a single char in render mode, this actually call another subroutine that do the paint job
	but this one take care of all the calculations...
	NOTE: It identify correctly println and /n & /r
*/
/******************************************************************************/
void XGLCD::_charWriteR(const char c,uint8_t offset,uint16_t fcolor,uint16_t bcolor)
{
    if (c == 13){                                                               // CR
	} else if (c == 10){                                                        // LF
		if (!_portrait){
			_cursorX = 0;
			_cursorY += (_FNTheight * _scaleY) + _FNTinterline + offset;
		} else {
			_cursorX += (_FNTheight * _scaleY) + _FNTinterline + offset;
			_cursorY = 0;
		}
		_textPosition(_cursorX,_cursorY,false);
	} else if (c == 32){                                                        // Space
		if (!_portrait){
			fillRect(_cursorX,_cursorY,(_spaceCharWidth * _scaleX),(_FNTheight * _scaleY),bcolor);
			_cursorX += (_spaceCharWidth * _scaleX) + _FNTspacing;
		} else {
			fillRect(_cursorY,_cursorX,(_spaceCharWidth * _scaleX),(_FNTheight * _scaleY),bcolor);
			_cursorY += (_spaceCharWidth * _scaleX) + _FNTspacing;
		}
	} else {                                                                    // Any other character
        int charIndex = _getCharCode(c);                                        //get char code
        if (charIndex > -1){                                                    //valid?
			int charW = 0;
			charW = _currentFont->chars[charIndex].image->image_width;          //get charW and glyph
			if (_TXTwrap){                                                      //check if goes out of screen and goes to a new line (if wrap) or just avoid
				if (!_portrait && (_cursorX + charW * _scaleX) >= _width){      //wrap = always on ??
                    //if (charW * _scaleX >= _width) return;                    //X-GRAPH: text shouldn't be wider then width of LCD
                    _cursorX = 0;
					_cursorY += (_FNTheight * _scaleY) + _FNTinterline + offset;
				} else if (_portrait && (_cursorY + charW * _scaleY) >= _width){
                    //if (charW * _scaleY >= _width) return;                    //X-GRAPH: text shouldn't be wider then width of LCD
					_cursorX += (_FNTheight * _scaleY) + _FNTinterline + offset;
                    _cursorY = 0;
				}
			} else {
				if (_portrait){
					if (_cursorY + charW * _scaleY >= _width) return;
				} else {
					if (_cursorX + charW * _scaleX >= _width) return;
				}
			}
            
            if (!_FNTcompression){                                              // Actual single char drawing here
				if (!_portrait){
					_drawChar_unc(_cursorX,_cursorY,charW,charIndex,fcolor);
				} else {
					_drawChar_unc(_cursorY,_cursorX,charW,charIndex,fcolor);
				}
			} else {
                                                                                // TODO: RLE compressed fonts
			}
            if (!_portrait){                                                    // add charW to total
				_cursorX += (charW * _scaleX) + _FNTspacing;
			} else {
				_cursorY += (charW * _scaleX) + _FNTspacing;
			}
		}
	}
}

/******************************************************************************/
/*!	PRIVATE
		Write a single char, only INT and FONT ROM char
		NOTE: It identify correctly println and /n & /r
*/
/******************************************************************************/
void XGLCD::_charWrite(const char c,uint8_t offset)
{
    if (_cursorX < 0) return;                                                   // X-Graph border checks
    if ((_cursorX + _FNTwidth) >= _width) return;
    if (_cursorY < 0) return;
    if ((_cursorY + _FNTheight) >= _height) return;
    
    bool dtacmd = false;
	if (c == 13){                                                               // CR
	} else if (c == 10){                                                        // LF
		if (!_portrait){
			_cursorX = 0;
			_cursorY += (_FNTheight + (_FNTheight * (_scaleY - 1))) + _FNTinterline + offset;
		} else {
			_cursorX += (_FNTheight + (_FNTheight * (_scaleY - 1))) + _FNTinterline + offset;
			_cursorY = 0;
		}
		_textPosition(_cursorX,_cursorY,false);
		dtacmd = false;
	} else {

		if (!dtacmd){
			dtacmd = true;
			if (!_textMode) _setTextMode(true);                                 // we are in graph mode?
			writeCommand(RA8875_MRWC);
        }
		_writeData(c);
		_waitBusy(0x80);
        if (!_portrait){                                                        // update cursor
			_cursorX += _FNTwidth;
		} else {
			_cursorY += _FNTwidth;
		}
	}
}


/******************************************************************************/
/*!	PRIVATE
		Search for glyph char code in font array
		It return font index or -1 if not found.
*/
/******************************************************************************/
int XGLCD::_getCharCode(uint8_t ch)
{
	int i;
	for (i=0;i<_currentFont->length;i++){                                       // search for char code
		if (_currentFont->chars[i].char_code == ch) return i;
	}
	return -1;
}

/******************************************************************************/
/*!	PRIVATE
		This helper loop trough a text string and return how long is (in pixel)
		NOTE: It identify correctly println and /n & /r and forget non present chars
*/
/******************************************************************************/
int16_t XGLCD::_STRlen_helper(const char* buffer,uint16_t len)
{
	if (!(_TXTrender)) {		                                                //_renderFont not active
		return (len * _FNTwidth);
	} else {									                                //_renderFont active
		int charIndex = -1;
		uint16_t i;
		if (len == 0) len = strlen(buffer);		                                //try to get data from string
		if (len == 0) return 0;					                                //better stop here
		if (_FNTwidth > 0){						                                // fixed width font
			return ((len * _spaceCharWidth));
		} else {								                                // variable width, need to loop trough entire string!
			uint16_t totW = 0;
			for (i = 0;i < len;i++){			                                //loop trough buffer
				if (buffer[i] == 32){			                                //a space
					totW += _spaceCharWidth;
				} else if (buffer[i] != 13 && buffer[i] != 10 && buffer[i] != 32){//avoid special char
					charIndex = _getCharCode(buffer[i]);
					if (charIndex > -1) {		                                //found!
						totW += (_currentFont->chars[charIndex].image->image_width);
					}
				}
			}
			return totW;						                                //return data
		}
	}
}


/******************************************************************************/
/*!	PRIVATE
		Here's the char render engine for uncompressed fonts, it actually render a single char.
		It's actually 2 functions, this one take care of every glyph line
		and perform some optimization second one paint concurrent pixels in chunks.
		Please do not steal this part of code!
*/
/******************************************************************************/
void XGLCD::_drawChar_unc(int16_t x,int16_t y,int charW,int index,uint16_t fcolor)
{
    if (x < 0) return;                                                          // X-Graph border checks
    if ((x + charW) >= _width) return;
    if (y < 0) return;
    if ((y + charW) >= _height) return;
    
	const uint8_t * charGlyp = _currentFont->chars[index].image->data;          //start by getting some glyph data...
	int			  totalBytes = _currentFont->chars[index].image->image_datalen;
	int i;
	uint8_t temp = 0;
	//some basic variable...
	uint8_t currentXposition = 0;                                               //the current position of the writing cursor in the x axis, from 0 to charW
	uint8_t currentYposition = 1;                                               //the current position of the writing cursor in the y axis, from 1 to _FNTheight
	int currentByte = 0;                                                        //the current byte in reading (from 0 to totalBytes)
	bool lineBuffer[charW];                                                     //the temporary line buffer (will be _FNTheight each char)
	int lineChecksum = 0;                                                       //part of the optimizer
	
	while (currentByte < totalBytes){                                           //the main loop that will read all bytes of the glyph
		temp = charGlyp[currentByte];
		for (i=7; i>=0; i--){
			if (currentXposition >= charW){                                     // exception //line buffer has been filled!
				currentXposition = 0;                                           //reset the line x position
                if (lineChecksum < 1){                                          //empty line
				} else if (lineChecksum == charW){                              //full line
					fillRect(	x,y + (currentYposition * _scaleY),charW * _scaleX,_scaleY,fcolor);
				} else {                                                        //line render
					_charLineRender(lineBuffer,charW,x,y,currentYposition,fcolor);
				}
                currentYposition++;                                             //next line
				lineChecksum = 0;                                               //reset checksum
			}
			lineBuffer[currentXposition] = bitRead(temp,i);                     //continue fill line buffer
			lineChecksum += lineBuffer[currentXposition];
			currentXposition++;
		}
		currentByte++;
	}
}

/******************************************************************************/
/*!	PRIVATE
		Font Line render optimized routine
		This will render ONLY a single font line by grouping chunks of same pixels
		Version 3.0 (fixed a bug that cause xlinePos to jump of 1 pixel
*/
/******************************************************************************/
void XGLCD::_charLineRender(bool lineBuffer[],int charW,int16_t x,int16_t y,int16_t currentYposition,uint16_t fcolor)
{
	int xlinePos = 0;
	int px;
	uint8_t endPix = 0;
	bool refPixel = false;
	while (xlinePos < charW){
		refPixel = lineBuffer[xlinePos];                                        //xlinePos pix as reference value for next pixels
		for (px = xlinePos;px <= charW;px++){                                   //detect and render concurrent pixels
			if (lineBuffer[px] == lineBuffer[xlinePos] && px < charW){          //grouping pixels with same val
				endPix++;
			} else {
				if (refPixel){
					fillRect(x,y + (currentYposition * _scaleY),endPix * _scaleX,_scaleY,fcolor);
				}
                xlinePos += endPix;                                             //reset and update some vals
                x += endPix * _scaleX;
				endPix = 0;
				break;                                                          //exit cycle for...
			}
		}
	}
}

/******************************************************************************/
/* Standard print functions */
/******************************************************************************/
 /*
void XGLCD::print(char *st, int x, int y, int deg)
{
    // TEMP TEMP TEMP: x == RIGHT to be added
    if (x == CENTER) setCursor(x, y, true);
    else setCursor(x, y, false);
    if (deg == 0) {
        _textWrite((const char *)st, strlen(st));
    }
    // other deg values to be added
}

void XGLCD::print(String st, int x, int y, int deg)
{
char s[st.length()+1];

    st.toCharArray(s, st.length()+1);
    print(s, x, y, deg);
}

void XGLCD::printNumI(long num, int x, int y, int length, char filler)
{
//tbf
}
                             
void XGLCD::printNumF(double num, byte dec, int x, int y, char divider, int length, char filler)
{
//tbf
}
*/
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								COLOR STUFF							           +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/******************************************************************************/
/*!
      Set the display 'Color Space'
	  Parameters:
	  Bit per Pixel color (colors): 8 or 16 bit
	  NOTE:
	  For display over 272*480 give the ability to use
	  Layers since at 16 bit it's not possible.
*/
/******************************************************************************/
void XGLCD::setColorBpp(uint8_t colors)
{
	if (colors != _color_bpp){                                                  //only if necessary
		if (colors < 16) {
			_color_bpp = 8;
			_colorIndex = 3;
			_writeRegister(RA8875_SYSR, RA8875_SYSR_8BPP);
			_maxLayers = 2;
		} else if (colors > 8) {                                                //65K
			_color_bpp = 16;
			_colorIndex = 0;
			_writeRegister(RA8875_SYSR, RA8875_SYSR_16BPP);
			_maxLayers = 1;
			_currentLayer = 0;
		}
	}
}

/******************************************************************************/
/*!
      Return current Color Space (8 or 16)
*/
/******************************************************************************/
uint8_t XGLCD::getColorBpp(void)
{
	return _color_bpp;
}
                             
/******************************************************************************/
/*!
      Sets set the foreground color using 16bit RGB565 color
	  It handles automatically color conversion when in 8 bit!
	  Parameters:
	  color: 16bit color RGB565
*/
/******************************************************************************/
void XGLCD::setForegroundColor(uint16_t color)
{
	_foreColor = color;

    _writeRegister(RA8875_FGCR0,((color & 0xF800) >> _RA8875colorMask[_colorIndex]));
    _writeRegister(RA8875_FGCR0+1,((color & 0x07E0) >> _RA8875colorMask[_colorIndex+1]));
    _writeRegister(RA8875_FGCR0+2,((color & 0x001F) >> _RA8875colorMask[_colorIndex+2]));
}
    
/******************************************************************************/
/*!
      Sets set the foreground color using 8bit R,G,B
	  Parameters:
	  R: 8bit RED
	  G: 8bit GREEN
	  B: 8bit BLUE
*/
/******************************************************************************/
void XGLCD::setForegroundColor(uint8_t R,uint8_t G,uint8_t B)
{
	_foreColor = Color565(R,G,B);

    _writeRegister(RA8875_FGCR0,R);
    _writeRegister(RA8875_FGCR0+1,G);
    _writeRegister(RA8875_FGCR0+2,B);
}
/******************************************************************************/
/*!
      Sets set the background color using 16bit RGB565 color
	  It handles automatically color conversion when in 8 bit!
	  Parameters:
	  color: 16bit color RGB565
	  Note: will set background Transparency OFF
*/
/******************************************************************************/
void XGLCD::setBackgroundColor(uint16_t color)
{
	_backColor = color;

    _writeRegister(RA8875_BGCR0,((color & 0xF800) >> _RA8875colorMask[_colorIndex]));//11
    _writeRegister(RA8875_BGCR0+1,((color & 0x07E0) >> _RA8875colorMask[_colorIndex+1]));//5
    _writeRegister(RA8875_BGCR0+2,((color & 0x001F) >> _RA8875colorMask[_colorIndex+2]));//0
}
/******************************************************************************/
/*!
      Sets set the background color using 8bit R,G,B
	  Parameters:
	  R: 8bit RED
	  G: 8bit GREEN
	  B: 8bit BLUE
	  Note: will set background Transparency OFF
*/
/******************************************************************************/
void XGLCD::setBackgroundColor(uint8_t R,uint8_t G,uint8_t B)
{
	_backColor = Color565(R,G,B);

    _writeRegister(RA8875_BGCR0,R);
    _writeRegister(RA8875_BGCR0+1,G);
    _writeRegister(RA8875_BGCR0+2,B);
}
/******************************************************************************/
/*!
      Sets set the transparent background color using 16bit RGB565 color
	  It handles automatically color conversion when in 8 bit!
	  Parameters:
	  color: 16bit color RGB565
	  Note: will set background Transparency ON
*/
/******************************************************************************/
void XGLCD::setTransparentColor(uint16_t color)
{
	_backColor = color;

    _writeRegister(RA8875_BGTR0,((color & 0xF800) >> _RA8875colorMask[_colorIndex]));
    _writeRegister(RA8875_BGTR0+1,((color & 0x07E0) >> _RA8875colorMask[_colorIndex+1]));
    _writeRegister(RA8875_BGTR0+2,((color & 0x001F) >> _RA8875colorMask[_colorIndex+2]));
}
/******************************************************************************/
/*!
      Sets set the Transparent background color using 8bit R,G,B
	  Parameters:
	  R: 8bit RED
	  G: 8bit GREEN
	  B: 8bit BLUE
	  Note: will set background Trasparency ON
*/
/******************************************************************************/
void XGLCD::setTransparentColor(uint8_t R,uint8_t G,uint8_t B)
{
	_backColor = Color565(R,G,B);//keep track

    _writeRegister(RA8875_BGTR0,R);
    _writeRegister(RA8875_BGTR0+1,G);
    _writeRegister(RA8875_BGTR0+2,B);
}

/******************************************************************************/
/*!		
		set foreground,background color (plus transparent background)
		Parameters:
		fColor: 16bit foreground color (text) RGB565
		bColor: 16bit background color RGB565
		backTransp:if true the bColor will be transparent
*/
/******************************************************************************/
void XGLCD::setColor(uint16_t fcolor, uint16_t bcolor, bool bcolorTraspFlag)
{
	if (fcolor != _foreColor) setForegroundColor(fcolor);
	if (bcolorTraspFlag){
		setTransparentColor(bcolor);
	} else {
		if (bcolor != _backColor) setBackgroundColor(bcolor);
	}
}
                             
/******************************************************************************/
/*!
    Compatibility layer functions
 */
/******************************************************************************/

void XGLCD::setColor(uint16_t color) {
    setForegroundColor(color);
}
                             
void XGLCD::setColor(uint8_t r, uint8_t g, uint8_t b) {
    setForegroundColor(r, g, b);
};
                             
uint16_t XGLCD::getColor(void) {
    return _foreColor;
}

void XGLCD::setBackColor(uint32_t color) {
    setBackgroundColor((uint16_t)color);
    //if (color == VGA_TRANSPARENT) _backTransparent = true;
    //else {
    //    _backcolor = color;
    //    _backTransparent = false;
    // }
}
                             
void XGLCD::setBackColor(uint8_t r, uint8_t g, uint8_t b) {
    setBackgroundColor(r, g, b);
    //_backTransparent = false;
};
                             
uint32_t XGLCD::getBackColor(void) {
    return (uint32_t)_backColor;
}

/******************************************************************************/
/*!
 convert a 16bit color(565) into 8bit color(332) as requested by RA8875 datasheet
 */
/******************************************************************************/

uint8_t XGLCD::_color16To8bpp(uint16_t color)
{
    return (map((color & 0xF800) >> 11, 0,28, 0,7)<<5 | map((color & 0x07E0) >> 5, 0,56, 0,7)<<2 | map(color & 0x001F, 0,24, 0,3));
}

/******************************************************************************/
/*!
 calculate a gradient color
 return a spectrum starting at blue to red (0...127)
 */
/******************************************************************************/
uint16_t XGLCD::gradient(uint8_t val)
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t q = val / 32;
    switch(q){
        case 0:
            r = 0; g = 2 * (val % 32); b = 31;
            break;
        case 1:
            r = 0; g = 63; b = 31 - (val % 32);
            break;
        case 2:
            r = val % 32; g = 63; b = 0;
            break;
        case 3:
            r = 31; g = 63 - 2 * (val % 32); b = 0;
            break;
    }
    return (r << 11) + (g << 5) + b;
}

/******************************************************************************/
/*!
 interpolate 2 16bit colors
 return a 16bit mixed color between the two
 Parameters:
 color1:
 color2:
 pos:0...div (mix percentage) (0:color1, div:color2)
 div:divisions between color1 and color 2
 */
/******************************************************************************/
uint16_t XGLCD::colorInterpolation(uint16_t color1,uint16_t color2,uint16_t pos,uint16_t div)
{
    if (pos == 0) return color1;
    if (pos >= div) return color2;
    uint8_t r1,g1,b1;
    Color565ToRGB(color1,r1,g1,b1);//split in r,g,b
    uint8_t r2,g2,b2;
    Color565ToRGB(color2,r2,g2,b2);//split in r,g,b
    return colorInterpolation(r1,g1,b1,r2,g2,b2,pos,div);
}

/******************************************************************************/
/*!
 interpolate 2 r,g,b colors
 return a 16bit mixed color between the two
 Parameters:
 r1.
 g1:
 b1:
 r2:
 g2:
 b2:
 pos:0...div (mix percentage) (0:color1, div:color2)
 div:divisions between color1 and color 2
 */
/******************************************************************************/
uint16_t XGLCD::colorInterpolation(uint8_t r1,uint8_t g1,uint8_t b1,uint8_t r2,uint8_t g2,uint8_t b2,uint16_t pos,uint16_t div)
{
    if (pos == 0) return Color565(r1,g1,b1);
    if (pos >= div) return Color565(r2,g2,b2);
    float pos2 = (float)pos/div;
    return Color565(
                    (uint8_t)(((1.0 - pos2) * r1) + (pos2 * r2)),
                    (uint8_t)((1.0 - pos2) * g1 + (pos2 * g2)),
                    (uint8_t)(((1.0 - pos2) * b1) + (pos2 * b2))
                    );
}


/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								SCROLL STUFF 							       +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
/******************************************************************************/
/*!     
        Sets the scroll mode. This is controlled by bits 6 and 7 of  
        REG[52h] Layer Transparency Register0 (LTPR0)
		Author: The Experimentalist
*/
/*****************************************************************************/
void XGLCD::setScrollMode(enum RA8875scrollMode mode)
{
    uint8_t temp = _readRegister(RA8875_LTPR0);
    temp &= 0x3F;
    switch(mode){
        case SIMULTANEOUS:                                                      // 00b : Layer 1/2 scroll simultaneously.
            // Do nothing
        break;
        case LAYER2ONLY:                                                        // 10b : Only Layer 2 scroll.
            temp |= 0x80;
        break;
        case BUFFERED:      	                                                // 11b: Buffer scroll (using Layer 2 as scroll buffer)
            temp |= 0xC0;
        break;
        case LAYER1ONLY:                                                        // 01b : Only Layer 1 scroll.
            temp |= 0x40;
            break;
        default:
            return;         	                                                //do nothing
    }
    //TODO: Should this be conditional on multi layer?
    //if (_useMultiLayers) _writeRegister(RA8875_LTPR0,temp);
    //_writeRegister(RA8875_LTPR0,temp);
	_writeData(temp);
}

/******************************************************************************/
/*!		
		Define a window for perform scroll
		Parameters:
		XL: x window start left
		XR: x window end right
		YT: y window start top
		YB: y window end bottom

*/
/******************************************************************************/
void XGLCD::setScrollWindow(int16_t XL,int16_t XR ,int16_t YT ,int16_t YB)
{
	if (_portrait){
		swapvals(XL,YT);
		swapvals(XR,YB);
	}
	
	_checkLimits_helper(XL,YT);
	_checkLimits_helper(XR,YB);
	
	_scrollXL = XL; _scrollXR = XR; _scrollYT = YT; _scrollYB = YB;
    _writeRegister(RA8875_HSSW0,(_scrollXL & 0xFF));
    _writeRegister(RA8875_HSSW0+1,(_scrollXL >> 8));
  
    _writeRegister(RA8875_HESW0,(_scrollXR & 0xFF));
    _writeRegister(RA8875_HESW0+1,(_scrollXR >> 8));   
    
    _writeRegister(RA8875_VSSW0,(_scrollYT & 0xFF));
    _writeRegister(RA8875_VSSW0+1,(_scrollYT >> 8));   
 
    _writeRegister(RA8875_VESW0,(_scrollYB & 0xFF));
    _writeRegister(RA8875_VESW0+1,(_scrollYB >> 8));
	delay(1);
}

/******************************************************************************/
/*!
		Perform the scroll

*/
/******************************************************************************/
void XGLCD::scroll(int16_t x,int16_t y)
{ 
	if (_portrait) swapvals(x,y);
	//if (y > _scrollYB) y = _scrollYB;//??? mmmm... not sure
	if (_scrollXL == 0 && _scrollXR == 0 && _scrollYT == 0 && _scrollYB == 0){  //do nothing, scroll window inactive
	} else {
		_writeRegister(RA8875_HOFS0,(x & 0xFF)); 
		_writeRegister(RA8875_HOFS1,(x >> 8));
 
		_writeRegister(RA8875_VOFS0,(y & 0xFF));
		_writeRegister(RA8875_VOFS1,(y >> 8));
	}
}	 
                             
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								BTE STUFF 								       +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/******************************************************************************/
/* 
		Block Transfer Move
		Can move a rectangular block from any area of memory (eg. layer 1) to any other (eg layer 2)
		Can move with transparency - note THE TRANSPARENT COLOUR IS THE TEXT FOREGROUND COLOUR
		ReverseDir is for moving overlapping areas - may need to use reverse to prevent it double-copying the overlapping area (this option not available with transparency or monochrome)
		ROP is Raster Operation. Usually use RA8875_ROP_SOURCE but a few others are defined
		Defaults to current layer if not given or layer is zero.
		Monochrome uses the colour-expansion mode: the input is a bit map which is then converted to the current foreground and background colours, transparent background is optional
		Monochrome data is assumed to be linear, originally written to the screen memory in 16-bit chunks with drawPixels().
		Monochrome mode uses the ROP to define the offset of the first image bit within the first byte. This also depends on the width of the block you are trying to display.
		Monochrome skips 16-bit words in the input pattern - see the example for more explanation and a trick to interleave 2 characters in the space of one.

		This function returns immediately but the actual transfer can take some time
		Caller should check the busy status before issuing any more RS8875 commands.

		Basic usage:
		BTE_Move(SourceX, SourceY, Width, Height, DestX, DestY) = copy something visible on the current layer
		BTE_Move(SourceX, SourceY, Width, Height, DestX, DestY, 2) = copy something from layer 2 to the current layer
		BTE_Move(SourceX, SourceY, Width, Height, DestX, DestY, 2, 1, true) = copy from layer 2 to layer 1, with the transparency option
		BTE_Move(SourceX, SourceY, Width, Height, DestX, DestY, 0, 0, true, RA8875_BTEROP_ADD) = copy on the current layer, using transparency and the ADD/brighter operation 
		BTE_Move(SourceX, SourceY, Width, Height, DestX, DestY, 0, 0, false, RA8875_BTEROP_SOURCE, false, true) = copy on the current layer using the reverse direction option for overlapping areas
*/

void  XGLCD::BTE_move(int16_t SourceX, int16_t SourceY, int16_t Width, int16_t Height, int16_t DestX, int16_t DestY, uint8_t SourceLayer, uint8_t DestLayer,bool Transparent, uint8_t ROP, bool Monochrome, bool ReverseDir)
{
	
	if (SourceLayer == 0) SourceLayer = _currentLayer;
	if (DestLayer == 0) DestLayer = _currentLayer;
    if (SourceLayer == 2) SourceY |= 0x8000;                                    //set the high bit of the vertical coordinate to indicate layer 2
	if (DestLayer == 2) DestY |= 0x8000;                                        //set the high bit of the vertical coordinate to indicate layer 2
    ROP &= 0xF0;                                                                //Ensure the lower bits of ROP are zero
	if (Transparent) {
		if (Monochrome) {
			ROP |= 0x0A;                                                        //colour-expand transparent
		} else {
			ROP |= 0x05;                                                        //set the transparency option
		}
	} else {
		if (Monochrome) {
			ROP |= 0x0B;                                                        //colour-expand normal
		} else {
			if (ReverseDir) {
				ROP |= 0x03;                                                    //set the reverse option
			} else {
				ROP |= 0x02;                                                    //standard block-move operation
			}
		}
	}

	_waitBusy(0x40);                                                            //Check that another BTE operation is not still in progress // X-Graph ??? Based on which register ???
	if (_textMode) _setTextMode(false);                                         //we are in text mode?
	BTE_moveFrom(SourceX,SourceY);
	BTE_size(Width,Height);
	BTE_moveTo(DestX,DestY);
	BTE_ropcode(ROP);
	if (Monochrome) _writeRegister(RA8875_BECR0, 0xC0); else _writeRegister(RA8875_BECR0, 0x80); //Execute BTE! (This selects linear addressing mode for the monochrome source data)
	_waitBusy(0x40);
	//we are supposed to wait for the thing to become unbusy
	//caller can call _waitBusy(0x40) to check the BTE busy status (except it's private)
}

/******************************************************************************/
/*! TESTING

*/
/******************************************************************************/
void XGLCD::BTE_size(int16_t w, int16_t h)
{
	if (_portrait) swapvals(w,h);
    _writeRegister(RA8875_BEWR0,w & 0xFF);
    _writeRegister(RA8875_BEWR0+1,w >> 8);
    _writeRegister(RA8875_BEHR0,h & 0xFF);
    _writeRegister(RA8875_BEHR0+1,h >> 8);
}	

/******************************************************************************/
/*!

*/
/******************************************************************************/

void XGLCD::BTE_moveFrom(int16_t SX,int16_t SY)
{
	if (_portrait) swapvals(SX,SY);
	_writeRegister(RA8875_HSBE0,SX & 0xFF);
	_writeRegister(RA8875_HSBE0+1,SX >> 8);
	_writeRegister(RA8875_VSBE0,SY & 0xFF);
	_writeRegister(RA8875_VSBE0+1,SY >> 8);
}	

/******************************************************************************/
/*!

*/
/******************************************************************************/

void XGLCD::BTE_moveTo(int16_t DX,int16_t DY)
{
	if (_portrait) swapvals(DX,DY);
	_writeRegister(RA8875_HDBE0,DX & 0xFF);
	_writeRegister(RA8875_HDBE0+1,DX >> 8);
	_writeRegister(RA8875_VDBE0,DY & 0xFF);
	_writeRegister(RA8875_VDBE0+1,DY >> 8);
}	

/******************************************************************************/
/*! TESTING
	Use a ROP code EFX
*/
/******************************************************************************/
void XGLCD::BTE_ropcode(unsigned char setx)
{
    _writeRegister(RA8875_BECR1,setx);
}

/******************************************************************************/
/*! TESTING
	Enable BTE transfer
*/
/******************************************************************************/
void XGLCD::BTE_enable(bool on)
{	
	uint8_t temp = _readRegister(RA8875_BECR0);
	on == true ? temp &= ~(1 << 7) : temp |= (1 << 7);
	_writeData(temp);
	//_writeRegister(RA8875_BECR0,temp);  
	_waitBusy(0x40);
}


/******************************************************************************/
/*! TESTING
	Select BTE mode (CONT (continuous) or RECT)
*/
/******************************************************************************/
void XGLCD::BTE_dataMode(enum RA8875btedatam m)
{	
	uint8_t temp = _readRegister(RA8875_BECR0);
	m == CONT ? temp &= ~(1 << 6) : temp |= (1 << 6);
	_writeData(temp);
	//_writeRegister(RA8875_BECR0,temp);  
}

/******************************************************************************/
/*! TESTING
	Select the BTE SOURCE or DEST layer (1 or 2)
*/
/******************************************************************************/

void XGLCD::BTE_layer(enum RA8875btelayer sd,uint8_t l)
{
	uint8_t temp;
	sd == SOURCE ? temp = _readRegister(RA8875_VSBE0+1) : temp = _readRegister(RA8875_VDBE0+1);
	l == 1 ? temp &= ~(1 << 7) : temp |= (1 << 7);
	_writeData(temp);
	//_writeRegister(RA8875_VSBE1,temp);  
}

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								LAYER STUFF             				       +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/******************************************************************************/
/*!
		Instruct the RA8875 chip to use 2 layers
		If resolution bring to restrictions it will switch to 8 bit
		so you can always use layers.
		Parameters:
		on: true (enable multiple layers), false (disable)
      
*/
/******************************************************************************/
void XGLCD::useLayers(boolean on)
{
	if (_useMultiLayers == on) return;                                          //no reason to do change that it's already as desidered.
	if (_color_bpp > 8) {                                                       //try to set up 8bit color space
		setColorBpp(8);
		_waitBusy(0x80);                                                        // X-Graph: added 0x80 (was empty): why is a waitbusy needed here ??
	}
	if (on){
		_useMultiLayers = true;
		_DPCR_Reg |= RA8875_DPCR_TWO_LAYERS;
		clearActiveWindow(true);
	} else {
		_useMultiLayers = false;
		_DPCR_Reg &= ~RA8875_DPCR_TWO_LAYERS;
		clearActiveWindow(false);
	}
	
	_writeRegister(RA8875_DPCR,_DPCR_Reg);
	if (!_useMultiLayers && _color_bpp < 16) setColorBpp(16);                   //bring color back to 16
}


/******************************************************************************/
/*!

      
*/
/******************************************************************************/
void XGLCD::layerEffect(enum RA8875boolean efx)
{
	uint8_t	reg = 0b00000000;
	if (!_useMultiLayers) useLayers(true);                                      // turn on multiple layers if it's off
	switch(efx){                                                                // bit 2,1,0 of LTPR0
		case LAYER1:                                                            //only layer 1 visible  [000] -> do nothing
		break;
		case LAYER2:                                                            //only layer 2 visible  [001]
			reg |= (1 << 0);
		break;
		case TRANSPARENT:                                                       //transparent mode [011]
			reg |= (1 << 0); reg |= (1 << 1);
		break;
		case LIGHTEN:                                                           //lighten-overlay mode [010]
			reg |= (1 << 1);
		break;
		case OR:                                                                //boolean OR mode           [100]
			reg |= (1 << 2);
		break;
		case AND:                                                               //boolean AND mode         [101]
			reg |= (1 << 0); reg |= (1 << 2);
		break;
		case FLOATING:                                                          //floating windows    [110]
			reg |= (1 << 1); reg |= (1 << 2);
		break;
		default:
		break;
	}
	_writeRegister(RA8875_LTPR0,reg);
}

/******************************************************************************/
/*!

      
*/
/******************************************************************************/
void XGLCD::layerTransparency(uint8_t layer1,uint8_t layer2)
{
	if (layer1 > 8) layer1 = 8;
	if (layer2 > 8) layer2 = 8;
	if (!_useMultiLayers) useLayers(true);                                      //turn on multiple layers if it's off
	//if (_useMultiLayers) _writeRegister(RA8875_LTPR1, ((layer2 & 0x0F) << 4) | (layer1 & 0x0F));
	//uint8_t res = 0b00000000;//RA8875_LTPR1
	//reg &= ~(0x07);//clear bit 2,1,0
	_writeRegister(RA8875_LTPR1, ((layer2 & 0xF) << 4) | (layer1 & 0xF));
}

/******************************************************************************/
/*! return the current drawing layer. If layers are OFF, return 255

*/
/******************************************************************************/
uint8_t XGLCD::getCurrentLayer(void)
{
	if (!_useMultiLayers) return 255;
	return _currentLayer;
}

/******************************************************************************/
/*! select pattern

*/
/******************************************************************************/
void XGLCD::setPattern(uint8_t num, enum RA8875pattern p)
{
	uint8_t maxLoc;
	uint8_t temp = 0b00000000;
	if (p != P16X16) {
		maxLoc = 16;                                                            //at 8x8 max 16 locations
	} else {
		maxLoc = 4;                                                             //at 16x16 max 4 locations
		temp |= (1 << 7);
	}
	if (num > (maxLoc - 1)) num = maxLoc - 1;
	temp = temp | num;
	writeTo(PATTERN);
	_writeRegister(RA8875_PTNO,temp);
}

/******************************************************************************/
/*! write pattern

*/
/******************************************************************************/
void XGLCD::writePattern(int16_t x,int16_t y,const uint8_t *data,uint8_t size,bool setAW)
{
	int16_t i;
	int16_t a,b,c,d;
	if (size < 8 || size > 16) return;
	if (setAW) getActiveWindow(a,b,c,d);
	setActiveWindow(x,x+size-1,y,y+size-1);
	setXY(x,y);
	
	if (_textMode) _setTextMode(false);                                         //we are in text mode?
	writeCommand(RA8875_MRWC);
	for (i=0;i<(size*size);i++) {
		_writeData(data[i*2]);
		_writeData(data[i*2+1]);
		_waitBusy(0x80);
	}
	if (setAW) setActiveWindow(a,b,c,d);                                        //set as it was before
}

/******************************************************************************/
/*! This is the most important function to write on:
	LAYERS
	CGRAM
	PATTERN
	CURSOR
	Parameter:
	d (L1, L2, CGRAM, PATTERN, CURSOR)
	When writing on layers 0 or 1, if the layers are not enable it will enable automatically
	If the display doesn't support layers, it will automatically switch to 8bit color
	Remember that when layers are ON you need to disable manually, once that only Layer 1 will be visible

*/
/******************************************************************************/
void XGLCD::writeTo(enum RA8875writes d)
{
	uint8_t temp = _readRegister(RA8875_MWCR1);
	//bool trigMultilayer = false;
	switch(d){
		case L1:
			temp &= ~((1<<3) | (1<<2));
			temp &= ~(1 << 0);
			_currentLayer = 0;
			//trigMultilayer = true;
            _writeData(temp);
			if (!_useMultiLayers) useLayers(true);
            _writeData(temp);
		break;
        case L2:
			temp &= ~((1<<3) | (1<<2));
			temp |= (1 << 0);
			_currentLayer = 1;
			//trigMultilayer = true;
			_writeData(temp);  
			if (!_useMultiLayers) useLayers(true);
		break;
		case CGRAM:
			temp &= ~(1 << 3);
			temp |= (1 << 2);
			if (_FNCR0_Reg & RA8875_FNCR0_CGRAM){                               //REG[0x21] bit7 must be 0
                _FNCR0_Reg &= ~RA8875_FNCR0_CGRAM;
				_writeRegister(RA8875_FNCR0,_FNCR0_Reg);  
				_writeRegister(RA8875_MWCR1,temp);
			} else {
				_writeData(temp);  
			}
		break;
        case PATTERN:
			temp |= (1 << 3);
			temp |= (1 << 2);
			_writeData(temp);  
		break;
		case CURSOR:
			temp |= (1 << 3);
			temp &= ~(1 << 2);
			_writeData(temp);  
		break;
		default:
		//break;
		return;
	}
	//if (trigMultilayer && !_useMultiLayers) useLayers(true);//turn on multiple layers if it's off
	//_writeRegister(RA8875_MWCR1,temp);   
}


/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+								PWM STUFF								       +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/******************************************************************************/
/*!
		PWM out
		Parameters:
		pw: pwm selection (1,2)
		p: 0...255 rate
		
*/
/******************************************************************************/
void XGLCD::PWMout(uint8_t pw,uint8_t p)
{
	uint8_t reg;
	pw > 1 ? reg = RA8875_P2DCR : reg = RA8875_P1DCR;
	_writeRegister(reg, p);
}

/******************************************************************************/
/*!
		Set the brightness of the backlight (if connected to pwm)
		(basic controls pwm 1)
		Parameters:
		val: 0...255
*/
/******************************************************************************/
void XGLCD::brightness(uint8_t val)
{
	_brightness = val;
	PWMout(1,_brightness);
}

/******************************************************************************/
/*!
		controls the backligh by using PWM engine.
		Parameters:
		on: true(backlight on), false(backlight off)
*/
/******************************************************************************/
void XGLCD::backlight(boolean on)
{
	if (on == true){
        PWMsetup(1,true, RA8875_PWM_CLK_DIV1024);                               //setup PWM ch 1 for backlight
        PWMout(1,_brightness);                                                  //turn on PWM1
    } else {
        PWMsetup(1,false, RA8875_PWM_CLK_DIV1024);                              //setup PWM ch 1 for backlight
	}
}

/******************************************************************************/
/*!
		Setup PWM engine
		Parameters:
		pw: pwm selection (1,2)
		on: turn on/off
		clock: the clock setting
		[private]
*/
/******************************************************************************/
void XGLCD::PWMsetup(uint8_t pw,boolean on, uint8_t clock)
{
	uint8_t reg;
	uint8_t set;
	if (pw > 1){
		reg = RA8875_P2CR;
		on == true ? set = RA8875_PxCR_ENABLE : set = RA8875_PxCR_DISABLE;
	} else {
		reg = RA8875_P1CR;
		on == true ? set = RA8875_PxCR_ENABLE : set = RA8875_PxCR_DISABLE;
	}
	_writeRegister(reg,(set | (clock & 0xF)));
}



/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+							     TOUCH SCREEN        					       +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/******************************************************************************/
/*!   
	  Initialize support for on-chip resistive Touch Screen controller

 Touch Panel Control Register 0  TPCR0  [0x70]
 7: 0(disable, 1:(enable)
 6,5,4:TP Sample Time Adjusting (000...111)
 3:Touch Panel Wakeup Enable 0(disable),1(enable)
 2,1,0:ADC Clock Setting (000...111) set fixed to 010: (System CLK) / 4, 10Mhz Max
!*/
/******************************************************************************/
#define RA8875_TPCR0_TOUCH 0x83 // enable touch panel / 512 system clocks / disable wake-up / system_clk/8
#define RA8875_TPCR1_IDLE 0x40 // manual mode / Vref internal / debounce disabled / Idle mode
#define RA8875_TPCR1_WAIT 0x41 // manual mode / Vref internal / debounce disabled / Wait for TP event
#define RA8875_TPCR1_LATCH_X 0x42 // manual mode / Vref internal / debounce disabled / Latch X data
#define RA8875_TPCR1_LATCH_Y 0x43 // manual mode / Vref internal / debounce disabled / Latch Y data

void XGLCD::touchBegin(void)
{
    _writeRegister(RA8875_TPCR0, RA8875_TPCR0_TOUCH);
    _writeRegister(RA8875_TPCR1, RA8875_TPCR1_WAIT);
}

/******************************************************************************/
/*!   
	  Read 10bit internal ADC of RA8875 registers and perform corrections
	  It will return always RAW data
	  Parameters:
	  x: out 0...1023
	  Y: out 0...1023

*/
/******************************************************************************/
#define TC_AVERAGE 13                                                           // Number of samples for averaging (only 5 results are used)
#define TC_DEBOUNCE 50                                                          // xx msec debouncing timeout

uint32_t XGLCD::_readTouchADC(bool xy) {
    uint32_t avg[TC_AVERAGE];
    uint32_t i,j,n,t;
    
    if (xy) _writeRegister(RA8875_TPCR1, RA8875_TPCR1_LATCH_Y); else _writeRegister(RA8875_TPCR1, RA8875_TPCR1_LATCH_X); // First do a dummy read to let the touchscreen voltage stabilize
    delayMicroseconds(10);                                                      // Wait for the ADC to stabilize
    _writeRegister(RA8875_TPCR1, RA8875_TPCR1_IDLE);                            // Read the ADC data but don't use this data
    _readRegister(RA8875_TPXH);
    _readRegister(RA8875_TPYH);
    _readRegister(RA8875_TPXYL);

    for (i = 0; i < TC_AVERAGE; i++) {                                          // Now get several readings for an averaging
        if (xy) _writeRegister(RA8875_TPCR1, RA8875_TPCR1_LATCH_Y); else _writeRegister(RA8875_TPCR1, RA8875_TPCR1_LATCH_X);
        delayMicroseconds(100);                                                 // Wait for the ADC to stabilize
        _writeRegister(RA8875_TPCR1, RA8875_TPCR1_IDLE);                        // Read the ADC data and this time use the data
        if (xy) j = _readRegister(RA8875_TPYH); else j = _readRegister(RA8875_TPXH);
        avg[i] = j;
        avg[i] <<= 2;
        j = _readRegister(RA8875_TPXYL);
        if (xy) avg[i] += (j >> 2) & 0x03; else avg[i] += j & 0x3;              // R8875 stores 2-bits of 10-bit result in 3th register (for both X and Y)
    }
    
    for (i = 0; i < TC_AVERAGE; i++) {                                          // Sort all read values
        j = 0;
        while (j < i) {
            n = j++;
            if (avg[n] > avg[j]) {
                t = avg[n];
                avg[n] = avg[j];
                avg[j] = t;
            }
        }
    }

    // Calculate the mean value of the middle measurements
    return (avg[(TC_AVERAGE/2)-3]+avg[(TC_AVERAGE/2)-2]+avg[(TC_AVERAGE/2)-1]+avg[TC_AVERAGE/2]+avg[(TC_AVERAGE/2)+1]+avg[(TC_AVERAGE/2)+2]+avg[(TC_AVERAGE/2)+3])/7;
}

bool XGLCD::readTouchADC(uint32_t *x, uint32_t *y)
{
    if ((readStatus() & 0x20) == 0) {                                           // Read touchscreen touched bit in status register
        _writeRegister(RA8875_TPCR1, RA8875_TPCR1_WAIT);                        // Not touched, put in touch detection mode again (maybe not needed this line)
        _tc_debounce = TC_DEBOUNCE;                                             // Debouncing
    } else {
        delay(_tc_debounce);                                                    // Touched, first wait for debouncing timeout
        _tc_debounce = TC_DEBOUNCE;                                             // Default debouncing time
        if (readStatus() & 0x20) {                                              // If still touched get the data
            *x = _readTouchADC(false);                                          // Read X coordinates
            *y = _readTouchADC(true);                                           // Read Y coordinates
            _writeRegister(RA8875_TPCR1, RA8875_TPCR1_WAIT);                    // Start waiting for a touch again
            delayMicroseconds(1000);                                            // Wait a bit
            if (readStatus() & 0x20) {                                          // Make sure the screen is still touched to prevent false coordinates during touch release
                _tc_debounce = 0;                                               // It's still touched, next touch detection does not require debouncing
                return true;
            }
        } else _writeRegister(RA8875_TPCR1, RA8875_TPCR1_WAIT);                 // Not touched, put in touch detection mode again
    }
    return false;                                                               // Not touched
}

/******************************************************************************/
/*!   
	  Returns 10bit x,y data with TRUE scale (0...1023)
	  Parameters:
	  x: out 0...1023
	  Y: out 0...1023
*/
/******************************************************************************/
bool XGLCD::touchReadAdc(uint32_t *x, uint32_t *y)
{
    uint32_t tx,ty;
    bool touched;
    
	touched = readTouchADC(&tx,&ty);
	*x = map(tx,_tsAdcMinX,_tsAdcMaxX,0,1024);
	*y = map(ty,_tsAdcMinY,_tsAdcMaxY,0,1024);
    return touched;
}

/******************************************************************************/
/*!   
	  Returns pixel x,y data with SCREEN scale (screen width, screen Height)
	  Parameters:
	  x: out 0...screen width  (pixels)
	  Y: out 0...screen Height (pixels)
	  Check for out-of-bounds here as touches near the edge of the screen
	  can be safely mapped to the nearest point of the screen.
	  If the screen is rotated, then the min and max will be modified elsewhere
	  so that this always corresponds to screen pixel coordinates.
	  /M.SANDERSCROCK added constrain
*/
/******************************************************************************/
bool XGLCD::touchReadPixel(uint32_t *x, uint32_t *y)
{
	uint32_t tx,ty;
    bool touched;
    
	touched = readTouchADC(&tx,&ty);
	*x = constrain(map(tx,_tsAdcMinX,_tsAdcMaxX,0,_width-1),0,_width-1);
	*y = constrain(map(ty,_tsAdcMinY,_tsAdcMaxY,0,_height-1),0,_height-1);
    return touched;
}

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+							SPI & LOW LEVEL STUFF						       +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/******************************************************************************/
/*! PRIVATE
		Write in a register
		Parameters:
		reg: the register
		val: the data
*/
/******************************************************************************/
void XGLCD::_writeRegister(const uint8_t reg, uint8_t val)
{
    _spiCSLow;                                                                  //writeCommand(reg);
    _spiwrite16(RA8875_CMDWRITE+reg);
    _spiCSHigh;
    _spiCSHigh;
    _spiCSLow;
    _spiwrite16(val);                                                           // RA8875_DATAWRITE = 0x00 (so skip the addition to speed up things)
    _spiCSHigh;
}

/******************************************************************************/
/*! PRIVATE
		Returns the value inside register
		Parameters:
		reg: the register
*/
/******************************************************************************/
uint8_t XGLCD::_readRegister(const uint8_t reg)
{
	writeCommand(reg);
	return _readData(false);
}

/******************************************************************************/
/*!
		Write data
		Parameters:
		d: the data
*/
/******************************************************************************/
void XGLCD::_writeData(uint8_t data)
{
    _spiCSLow;
    //_spiwrite(RA8875_DATAWRITE);_spiwrite(data);
    _spiwrite16(data);                                                          // RA8875_DATAWRITE = 0x00 (so skip the addition to speed up things)
    _spiCSHigh;
}

/******************************************************************************/
/*! 
		Write 16 bit data
		Parameters:
		d: the data (16 bit)
*/
/******************************************************************************/
void  XGLCD::_writeData16(uint16_t data)
{
    _spiCSLow;
#ifdef _spiwrite24
    _spiwrite24(RA8875_DATAWRITE, data);
#else
	_spiwrite(RA8875_DATAWRITE);
	_spiwrite16(data);
#endif
    _spiCSHigh;
}

/******************************************************************************/
/*!	PRIVATE

*/
/******************************************************************************/
uint8_t XGLCD::_readData(bool stat)
{
    uint8_t x;
    
    _spisetSpeed(SPI_SPEED_READ);
    _spiCSLow;
#ifdef _spixread
    if (stat == true) {_spixread(RA8875_CMDREAD, x);} else {_spixread(RA8875_DATAREAD, x);}
#else
    if (stat == true) {_spiwrite(RA8875_CMDREAD);} else {_spiwrite(RA8875_DATAREAD);}
    delayMicroseconds(50);                                                      // Stabilize time, else first bit is read wrong
    _spiread(x);
#endif
    _spiCSHigh;
    _spisetSpeed(SPI_SPEED_WRITE);
    return x;

}

/******************************************************************************/
/*!

*/
/******************************************************************************/
uint8_t	XGLCD::readStatus(void)
{
	return _readData(true);
}

/******************************************************************************/
/*! PRIVATE
		Write a command
		Parameters:
		d: the command
*/
/******************************************************************************/
void XGLCD::writeCommand(const uint8_t d)
{
    _spiCSLow;
	//_spiwrite(RA8875_CMDWRITE);_spiwrite(d);
    _spiwrite16(RA8875_CMDWRITE+d);
    _spiCSHigh;
}

