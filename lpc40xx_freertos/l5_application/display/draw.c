#include "draw.h"
#include "graphics.h"
#include "object.h"

static void draw_borders(void) {
  fillRect(0, 0, BORDER_WIDTH, BORDER_HEIGHT, YELLOW);
  fillRect(30, 0, BORDER_WIDTH, BORDER_HEIGHT, YELLOW);
  drawLine(0, BORDER_HEIGHT, 31, BORDER_HEIGHT, WHITE);
}

const uint8_t crash_screen_B[] = {0xF0, 0X88, 0xF0, 0x88, 0xF0};
const uint8_t crash_screen_O[] = {0x70, 0x88, 0x88, 0x88, 0x70};
const uint8_t crash_screen_M[] = {0x88, 0x88, 0xA8, 0xD8, 0x88};
const uint8_t crash_screen_exclaimation[] = {0xC0, 0x00, 0xC0, 0xC0, 0xC0};

void draw_crash_screen(void) {
  drawBitmap(3, 22, crash_screen_B, 5, 5, RED);
  drawBitmap(9, 22, crash_screen_O, 5, 5, RED);
  drawBitmap(15, 22, crash_screen_O, 5, 5, RED);
  drawBitmap(21, 22, crash_screen_M, 5, 5, RED);
  drawBitmap(27, 22, crash_screen_exclaimation, 2, 5, RED);
  draw_borders();
}

const uint8_t game_over_G[] = {0xF8, 0x88, 0xB8, 0x80, 0xF8};
const uint8_t game_over_A[] = {0x88, 0x88, 0xF8, 0x50, 0x20};
const uint8_t game_over_M[] = {0x88, 0x88, 0xA8, 0xD8, 0x88};
const uint8_t game_over_E[] = {0xF8, 0x80, 0xF8, 0x80, 0xF8};
const uint8_t game_over_O[] = {0x70, 0x88, 0x88, 0x88, 0x70};
const uint8_t game_over_V[] = {0x20, 0x50, 0x88, 0x88, 0x88};
const uint8_t game_over_R[] = {0x98, 0xA0, 0xF8, 0x88, 0xF8};

const uint8_t score_letter_S[] = {0xE0, 0x20, 0xE0, 0x80, 0xE0};
const uint8_t score_letter_C[] = {0xE0, 0x80, 0x80, 0x80, 0xE0};
const uint8_t score_letter_O[] = {0xE0, 0xA0, 0xA0, 0xA0, 0xE0};
const uint8_t score_letter_R[] = {0xA0, 0xC0, 0xE0, 0xA0, 0xE0};
const uint8_t score_letter_E[] = {0xE0, 0x80, 0xC0, 0x80, 0xE0};

static const uint8_t smiley[] = {0x3c, 0x42, 0x99, 0xa5, 0x81, 0xa5, 0x81, 0x42, 0x3c};

static const uint8_t number[][5] = {{0xE0, 0xA0, 0xA0, 0xA0, 0xE0}, {0xE0, 0x40, 0x40, 0xC0, 0x40},
                                    {0xE0, 0x80, 0xE0, 0x20, 0xE0}, {0xE0, 0x20, 0x60, 0x20, 0xE0},
                                    {0x40, 0xE0, 0xC0, 0x80, 0x80}, {0xE0, 0x20, 0xE0, 0x80, 0xE0},
                                    {0xE0, 0Xa0, 0xE0, 0x80, 0XE0}, {0x20, 0x20, 0x20, 0x20, 0xE0},
                                    {0xE0, 0xA0, 0xE0, 0xA0, 0xE0}, {0xE0, 0x20, 0xE0, 0xA0, 0xE0}};

void draw_gameover_screen() {
  uint8_t hundred, ten, unit;
  drawRect(0, 0, LED_MATRIX_WIDTH, LED_MATRIX_HEIGHT, WHITE);
  drawBitmap(4, 52, game_over_G, 5, 5, RED);
  drawBitmap(10, 52, game_over_A, 5, 5, RED);
  drawBitmap(16, 52, game_over_M, 5, 5, RED);
  drawBitmap(22, 52, game_over_E, 5, 5, RED);
  drawBitmap(4, 44, game_over_O, 5, 5, RED);
  drawBitmap(10, 44, game_over_V, 5, 5, RED);
  drawBitmap(16, 44, game_over_E, 5, 5, RED);
  drawBitmap(22, 44, game_over_R, 5, 5, RED);

  drawBitmap(6, 30, score_letter_S, 3, 5, BLUE);
  drawBitmap(10, 30, score_letter_C, 3, 5, BLUE);
  drawBitmap(14, 30, score_letter_O, 3, 5, BLUE);
  drawBitmap(18, 30, score_letter_R, 3, 5, BLUE);
  drawBitmap(22, 30, score_letter_E, 3, 5, BLUE);

  get_score(&hundred, &ten, &unit);
  drawBitmap(10, 24, number[hundred], 3, 5, CYAN);
  drawBitmap(14, 24, number[ten], 3, 5, CYAN);
  drawBitmap(18, 24, number[unit], 3, 5, CYAN);

  drawBitmap(12, 8, smiley, 8, 9, YELLOW);
}

const uint8_t level_letter_L[] = {0xE0, 0x80, 0x80, 0x80, 0x80};
const uint8_t level_letter_V[] = {0x40, 0xA0, 0xA0, 0xA0, 0xA0};

extern uint8_t current_level;

void draw_level() {
  drawBitmap(5, 53, level_letter_L, 3, 5, GREEN);
  drawBitmap(9, 53, score_letter_E, 3, 5, GREEN);
  drawBitmap(13, 53, level_letter_V, 3, 5, GREEN);
  drawBitmap(17, 53, score_letter_E, 3, 5, GREEN);
  drawBitmap(21, 53, level_letter_L, 3, 5, GREEN);

  drawBitmap(25, 53, number[current_level], 3, 5, GREEN);
}