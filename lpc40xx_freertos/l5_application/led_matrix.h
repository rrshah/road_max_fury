#pragma once
#include <stdint.h>

void led_matrix__init(void);
void drawPixel(int16_t x, int16_t y, uint16_t c);
void led_matrix__updateDisplay(void);