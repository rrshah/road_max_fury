#pragma once

#include <stdbool.h>
#include <stdint.h>

enum {
  BLACK = 0,
  BLUE = 1,
  GREEN = 2,
  CYAN = 3,
  RED = 4,
  MAGENTA = 5,
  YELLOW = 6,
  WHITE = 7
} colors;

#define LED_MATRIX_HEIGHT (64)
#define LED_MATRIX_WIDTH (32)

// It is half of the height..since we have 4
// addr lines, thus (2^4 =) 16 addressable rows
#define LED_MATRIX_ROWS (LED_MATRIX_HEIGHT / 2)
#define LED_MATRIX_COLUMNS 64

void led_matrix__setupLedMatrixPins(void);
void led_matrix__setDefaultPinStates(void);

bool led_matrix__drawPixel(int16_t x, int16_t y, uint16_t color);
// To match function prototype required for AdafruitGFX library
void drawPixel(int16_t x, int16_t y, uint16_t c);
void led_matrix__turnOnAllPixels(uint16_t color);
void led_matrix__turnOffAllPixels(void);