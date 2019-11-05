/*
Multifont GFX library is adapted from Adafruit_GFX library by Paul Kourany
v1.0.0, May 2014 Initial Release
v1.0.1, June 2014 Font Compilation update
v1.0.2, Aug 2015 Added charWidth(char) function to return char width in pixels

Please read README.pdf for details
*/

#ifndef _ADAFRUIT_MFGFX_H
#define _ADAFRUIT_MFGFX_H

#include "gpio.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//#include "Particle.h"
//#include "fonts.h"

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif

#define swap(a, b)                                                             \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }

#define WIDTH (64)
#define HEIGHT (32)

typedef struct Adafruit_GFX {
  int16_t _width;
  int16_t _height; // Display w/h as modified by current rotation
  int16_t cursor_x;
  int16_t cursor_y;
  uint16_t textcolor;
  int16_t textbgcolor;
  uint8_t textsize;
  uint8_t rotation;
  uint8_t font;
  uint8_t fontStart;
  uint8_t fontEnd;
  int8_t fontKern;
  const uint8_t *fontData;
  // const FontDescriptor *fontDesc;
  bool wrap; // If set, 'wrap' text at right edge of display
} Adafruit_GFX;

void Adafruit_GFX_init(int16_t w, int16_t h); // Constructor
void drawPixel(int16_t x, int16_t y, uint16_t color);

// Virtual
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void fillScreen(uint16_t color);
void invertDisplay(bool i);
void drawFastChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
                  uint16_t bg, uint8_t size);
// Virtual ends

// These exist only with Adafruit_GFX (no subclass overrides)
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
                      uint16_t color);
void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
                      int16_t delta, uint16_t color);
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                  int16_t y2, uint16_t color);
void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                  int16_t y2, uint16_t color);
void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius,
                   uint16_t color);
void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius,
                   uint16_t color);
void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w,
                int16_t h, uint16_t color);
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
              uint16_t bg, uint8_t size);
void setCursor(int16_t x, int16_t y);
void setTextColor(uint16_t c);
void setTextColor_bg(uint16_t c, uint16_t bg);
void setTextSize(uint8_t s);
void setTextWrap(bool w);
void setRotation(uint8_t r);
void setFont(uint8_t f);
/// no subclass overrides ends here

// This is a virtual fucntion
size_t write(uint8_t);

int16_t charWidth(unsigned char c);

int16_t height(void);
int16_t width(void);

uint8_t getRotation(void);

/******* RGBPannel ***********/

//  Constructor for 16x32 panel:
//  RGBmatrixPanel(uint8_t a, uint8_t b, uint8_t c,
//    uint8_t sclk, uint8_t latch, uint8_t oe, bool dbuf, uint16_t width=32);

// Constructor for 32x32 panel (adds 'd' pin):
void RGBmatrixPanel_init(bool dbuf, uint16_t width); // default width = 32

void led_matrix__begin(void);
void drawPixel(int16_t x, int16_t y, uint16_t c);
void fillScreen(uint16_t c);
void updateDisplay(void);
void swapBuffers(bool);
void dumpMatrix(void);
uint8_t *backBuffer(void);
uint16_t Color333(uint8_t r, uint8_t g, uint8_t b);
uint16_t Color444(uint8_t r, uint8_t g, uint8_t b);
uint16_t Color888(uint8_t r, uint8_t g, uint8_t b);
uint16_t Color888_gflag(uint8_t r, uint8_t g, uint8_t b, bool gflag);
uint16_t ColorHSV(long hue, uint8_t sat, uint8_t val, bool gflag);

typedef struct RGBmatrixPanel {
  uint8_t *matrixbuff[2];
  uint8_t nRows;
  volatile uint8_t backindex;
  volatile bool swapflag;

  uint8_t _sclk, _latch, _oe, _a, _b, _c, _d;

  // Counters/pointers for interrupt handler:
  volatile uint8_t row, plane;
  volatile uint8_t *buffptr;
} RGBmatrixPanel;

/******* RGBPannel Ends here***********/
#endif // _ADAFRUIT_MFGFX_H
