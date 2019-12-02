#include <stdio.h>

#include "graphics.h"
#include "object.h"

#define BORDER_HEIGHT (52)
#define BORDER_WIDTH (3)
#define CAR_WIDTH (4)
#define CAR_HEIGHT (5)

#define NUM_OF_OBSTACLES 1

extern const uint8_t car[];
static bitmap_object player_car;
// static bitmap_object car_obstacle[NUM_OF_OBSTACLES];

void object__draw(bitmap_object object) {
  drawBitmap(object.x, object.y, object.image, object.width, object.height, object.color);
}

// void draw_car(uint8_t x, uint8_t y, uint8_t color) {
//   drawLine(x - 1, y, x - 1, y + CAR_HEIGHT, BLACK);

//   drawLine(x + 1, y, x + 1, y + CAR_HEIGHT, color);
//   drawLine(x + 2, y, x + 2, y + CAR_HEIGHT, color);
//   drawPixel(x, y + 1, color);
//   drawPixel(x, y + 3, color);
//   drawPixel(x + 2, y + 1, color);
//   drawPixel(x + 2, y + 3, color);

//   drawLine(x + CAR_WIDTH + 1, y, x + CAR_WIDTH + 1, y + CAR_HEIGHT, BLACK);
// }

void clear_previous_pixels(uint8_t x, uint8_t y) {
  drawLine(x - 1, y, x - 1, y + CAR_HEIGHT, BLACK);
  // drawLine(x + CAR_WIDTH + 1, y, x + CAR_WIDTH + 1, y + CAR_HEIGHT, BLACK);
}

void object__init_player_car(void) {
  player_car.x = 15;
  player_car.y = 10;
  player_car.height = 5;
  player_car.width = 4;
  player_car.color = CYAN;
  player_car.image = car;
}

void move() {
  // move_obstacles();
}

static void draw_borders() {
  fillRect(0, 0, BORDER_WIDTH, BORDER_HEIGHT, GREEN);
  fillRect(29, 0, BORDER_WIDTH, BORDER_HEIGHT, GREEN);
}

// static void draw_score();
void draw_player_car() {

  // draw_car(player_car.x, player_car.y, player_car.color);
  object__draw(player_car);
  clear_previous_pixels(player_car.x, player_car.y);
}
// static void draw_obstacles();

void draw() {
  // object__init_player_car();
  draw_borders();
  draw_player_car();
  // led_matrix__drawPixel(5, 6, RED);
}

void object__move(bitmap_object object) {}

void move_car_left() {
  if (player_car.x > BORDER_WIDTH) {
    player_car.x -= 1;
  }
}

void move_car_right() {
  if (player_car.x < (LED_MATRIX_WIDTH - BORDER_WIDTH - CAR_WIDTH)) {
    player_car.x += 1;
  }
}