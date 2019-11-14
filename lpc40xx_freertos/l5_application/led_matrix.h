#pragma once

#include <stdbool.h>
#include <stdint.h>

enum { black_0 = 0, blue_1 = 1, green_2 = 2, cyan_3 = 3, red_4, magenta_5, yellow_6, white_7 = 7 } colors;

#define LED_MATRIX_HEIGHT 32
#define LED_MATRIX_WIDTH 64

// It is half of the height..since we have 4
// addr lines, thus (2^4 =) 16 addressable rows
#define LED_MATRIX_ROWS (LED_MATRIX_HEIGHT / 2)
#define LED_MATRIX_COLUMNS 64

void led_matrix__init(uint8_t matrix_buffer[][LED_MATRIX_WIDTH]);
void led_matrix__setupLedMatrixPins(void);
void led_matrix__setDefaultPinStates(void);

bool led_matrix__drawPixel(int16_t x, int16_t y, uint16_t color);
// To match function prototype required for AdafruitGFX library
void drawPixel(int16_t x, int16_t y, uint16_t c);
void led_matrix__turnOnAllPixels(uint16_t color);
void led_matrix__turnOffAllPixels(void);

void led_matrix__selectRow(uint8_t row);
void led_matrix__de_assert_output_enable_latch(void);
void led_matrix__assert_output_disable_latch(void);
void led_matrix__clock_in_pixel_data(uint8_t pixel_value);
void led_matrix__updateDisplay(void);
/** @brief Updates LED Matrix Row
 *  @param row_to_display (row 0 to row 31)
 */
void led_matrix__updateRowOfDisplay(uint8_t row_to_display);