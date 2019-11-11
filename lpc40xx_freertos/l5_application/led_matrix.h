#pragma once
#include <stdint.h>

#define LED_MATRIX_HEIGHT 32
#define LED_MATRIX_WIDTH 64

//It is half of the height..since we have 4 
// addr lines, thus (2^4 =) 16 addressable rows
#define LED_MATRIX_ROWS LED_MATRIX_HEIGHT/2
#define LED_MATRIX_COLUMNS 64

void led_matrix__init(uint8_t matrix_buffer[][LED_MATRIX_WIDTH]);
void drawPixel(int16_t x, int16_t y, uint16_t c);
void led_matrix__updateDisplay(void);