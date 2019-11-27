#include "object.h"
#include "graphics.h"

#define BORDER_HEIGHT (52)
#define BORDER_WIDTH (3)
#define CAR_WIDTH (4)
#define CAR_HEIGHT (5)

#define NUM_OF_OBSTACLES 1

extern const uint8_t car;
static bitmap_object player_car = {.x = 15, .y = 10, .height = 5, .width = 4, .color = CYAN, .image = &car};
// static bitmap_object car_obstacle[NUM_OF_OBSTACLES];

void object__draw(bitmap_object object) {
  drawBitmap(object.x, object.y, object.image, object.width, object.height, object.color);
}

void object__init_player_car() {
  player_car.x = 15;
  player_car.y = 10;
  player_car.height = 5;
  player_car.width = 4;
  player_car.color = CYAN;
  player_car.image = &car;
}

// void object__init_player_car() {
//   for (int i = 0; i < NUM_OF_OBSTACLES; i++) {

//     car_obstacle[i].x = 20 * i;

//     car_obstacle[i].y = 50;
//     car_obstacle[i].height = 5;
//     car_obstacle[i].width = 4;
//     car_obstacle[i].color = RED;

//     car_obstacle[i].image = &car;

//     car_obstacle[i].isAlive = true;
//   }
// }

void move() {
  // move_obstacles();
}

static void draw_borders() {
  fillRect(0, 0, BORDER_WIDTH, BORDER_HEIGHT, GREEN);
  fillRect(29, 0, BORDER_WIDTH, BORDER_HEIGHT, GREEN);
}

// static void draw_score();
static void draw_player_car() { object__draw(player_car); }
// static void draw_obstacles();

void draw() {
  draw_borders();
  draw_player_car();
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