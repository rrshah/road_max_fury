#include "draw.h"
#include "graphics.h"
#include "object.h"

void draw_borders(void) {
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

const uint8_t letter_R[] = {0x84, 0x88, 0x90, 0xA0, 0xC0, 0xFC, 0x84, 0x84, 0x84, 0xFC};
const uint8_t letter_O[] = {0x78, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x78};
const uint8_t letter_A[] = {0x84, 0x84, 0x84, 0x84, 0x84, 0xFC, 0x84, 0x84, 0x84, 0x78};
const uint8_t letter_D[] = {0xF0, 0x88, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x88, 0xF0};
const uint8_t letter_M[] = {0x84, 0x84, 0x84, 0x84, 0x84, 0xB4, 0xB4, 0xB4, 0xCC, 0x84};
const uint8_t letter_X[] = {0x84, 0x84, 0x84, 0x48, 0x30, 0x30, 0x48, 0x84, 0x84, 0x84};
const uint8_t letter_F[] = {0x80, 0x80, 0x80, 0x80, 0x80, 0xFC, 0x80, 0x80, 0x80, 0xFC};
const uint8_t letter_U[] = {0x78, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84, 0x84};
const uint8_t letter_Y[] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x48, 0x84, 0x84, 0x84};

const uint8_t letter_smallP[] = {0x80, 0x80, 0xF8, 0x88, 0xF8};
const uint8_t letter_smallL[] = {0xF8, 0x80, 0x80, 0x80, 0x80};
const uint8_t letter_smallA[] = {0x88, 0x88, 0xF8, 0x88, 0X70};
const uint8_t letter_smallY[] = {0x20, 0x20, 0x20, 0x50, 0x88};

const uint8_t play_button[] = {0xC0, 0xE0, 0xF1, 0xE0, 0xC0};

extern bitmap_object start_screen_car;

void draw_start_screen() {
  drawRect(0, 0, LED_MATRIX_WIDTH, LED_MATRIX_HEIGHT, WHITE);
  drawBitmap(1, 48, letter_R, 6, 10, RED);
  drawBitmap(9, 48, letter_O, 6, 10, RED);
  drawBitmap(17, 48, letter_A, 6, 10, RED);
  drawBitmap(25, 48, letter_D, 6, 10, RED);
  drawBitmap(5, 35, letter_M, 6, 10, RED);
  drawBitmap(13, 35, letter_A, 6, 10, RED);
  drawBitmap(21, 35, letter_X, 6, 10, RED);
  drawBitmap(1, 22, letter_F, 6, 10, RED);
  drawBitmap(9, 22, letter_U, 6, 10, RED);
  drawBitmap(17, 22, letter_R, 6, 10, RED);
  drawBitmap(25, 22, letter_Y, 6, 10, RED);

  drawBitmap(2, 13, letter_smallP, 5, 5, BLUE);
  drawBitmap(8, 13, letter_smallL, 5, 5, BLUE);
  drawBitmap(14, 13, letter_smallA, 5, 5, BLUE);
  drawBitmap(20, 13, letter_smallY, 5, 5, BLUE);

  drawBitmap(26, 13, play_button, 4, 5, GREEN);
  object__draw(start_screen_car);
  move_start_screen_car();
}

void draw_score() {
  uint8_t hundred, ten, unit;

  drawBitmap(1, 59, score_letter_S, 3, 5, BLUE);
  drawBitmap(5, 59, score_letter_C, 3, 5, BLUE);
  drawBitmap(9, 59, score_letter_O, 3, 5, BLUE);
  drawBitmap(13, 59, score_letter_R, 3, 5, BLUE);
  drawBitmap(17, 59, score_letter_E, 3, 5, BLUE);

  get_score(&hundred, &ten, &unit);
  drawBitmap(21, 59, number[hundred], 3, 5, CYAN);
  drawBitmap(25, 59, number[ten], 3, 5, CYAN);
  drawBitmap(29, 59, number[unit], 3, 5, CYAN);
}