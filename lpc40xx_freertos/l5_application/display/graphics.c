#include "graphics.h"
#include "fonts.h"
#include <stdio.h>
#include <stdlib.h>

const uint8_t _width = 32;
const uint8_t _height = 64;

#define swap(a, b)                                                             \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }

void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w,
                int16_t h, uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;

  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        drawPixel(x + i, y + j, color);
      } else {
        drawPixel(x + i, y + j, BLACK);
      }
    }
  }
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  drawLine(x, y, x, y + h - 1, color);
}

void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  drawLine(x, y, x + w - 1, y, color);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  for (int16_t i = x; i < x + w; i++) {
    drawFastVLine(i, y, h, color);
  }
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y + h - 1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x + w - 1, y, h, color);
}

void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
              uint16_t bg, uint8_t size) {
  if (c < fontStart || c > fontEnd) {
    c = 0;
  } else {
    c -= fontStart;
  }

  if ((x >= _width) ||                               // Clip right
      (y >= _height) ||                              // Clip bottom
      ((x + (fontDesc[c].width * size) - 1) < 0) ||  // Clip left
      ((y + (fontDesc[c].height * size) - 1) < 0)) { // Clip top
    printf("Returning here\n");
    return;
  }

  uint8_t bitCount = 0;
  uint16_t fontIndex = fontDesc[c].offset + 2; //((fontDesc + c)->offset) + 2;

  for (int8_t i = 0; i < fontDesc[c].height; i++) { // i<fontHeight
    uint8_t line;
    for (int8_t j = 0; j < fontDesc[c].width; j++) { // j<fontWidth
      if (bitCount++ % 8 == 0) {
        line = fontData[fontIndex++];
      }
      if (line & 0x80) {
        printf("Inside line & 0x80\n");
        if (size == 1) { // default sizeFast
          printf("Calling drawPixel %d, %d\n", x + j, y + i);
          drawPixel(x + j, y + i, color);
        } else { // big size
          fillRect(x + (j * size), y + (i * size), size, size, color);
        }
      } else if (bg != color) {
        printf("Inside else if\n");
        if (size == 1) // default size
          drawPixel(x + j, y + i, bg);
        else { // big size
          fillRect(x + j * size, y + i * size, size, size, bg);
        }
      }
      line <<= 1;
    }
    bitCount = 0;
  }
}
