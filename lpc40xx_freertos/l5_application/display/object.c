#include <stdio.h>

#include "graphics.h"
#include "object.h"

#define BORDER_HEIGHT (52)
#define BORDER_WIDTH (2)
#define CAR_WIDTH (4)
#define CAR_HEIGHT (5)
#define CAR_HEIGHT_WITH_PADDING (6)
#define CAR_WIDTH_WITH_PADDING (6)
#define NUM_OF_OBSTACLES (1)

extern const uint8_t car[];

static bitmap_object obstacle_cars[] = {{.x = 10,
                                         .y = BORDER_HEIGHT,
                                         .color = RED,
                                         .image = car,
                                         .movement_direction = DOWN,
                                         .height = CAR_HEIGHT_WITH_PADDING,
                                         .width = CAR_WIDTH_WITH_PADDING,
                                         .speed = 1,
                                         .counter = 0},
                                        {.x = 20,
                                         .y = BORDER_HEIGHT,
                                         .color = YELLOW,
                                         .image = car,
                                         .movement_direction = DOWN,
                                         .height = CAR_HEIGHT_WITH_PADDING,
                                         .width = CAR_WIDTH_WITH_PADDING,
                                         .speed = 10,
                                         .counter = 0}};

static bitmap_object player_car;
static bitmap_object car_obstacle[NUM_OF_OBSTACLES];

void object__draw(bitmap_object object) {
  drawBitmap(object.x, object.y, object.image, object.width, object.height, object.color);
}

void object__init_player_car(void) {
  player_car.x = 25;
  player_car.y = 10;
  player_car.height = 6;
  player_car.width = 6;
  player_car.color = CYAN;
  player_car.image = car;
}

static void move_obstacles(bitmap_object obstacle) {
  obstacle.counter++;
  // if (obstacle.counter != obstacle.speed) {
  //  return;
  //}

  switch (obstacle.movement_direction) {
  case DOWN:
    obstacle.y -= 1;
  }
}

void move() { move_obstacles(obstacle_cars[0]); }

static void draw_borders() {
  fillRect(0, 0, BORDER_WIDTH, BORDER_HEIGHT, GREEN);
  fillRect(30, 0, BORDER_WIDTH, BORDER_HEIGHT, GREEN);
}

// static void draw_score();
void draw_player_car() { object__draw(player_car); }
// static void draw_obstacles();

void draw() {
  // object__init_player_car();
  draw_player_car();
  object__draw(obstacle_cars[0]);
  // draw_borders();
  // led_matrix__drawPixel(5, 6, RED);
}

void object__move(bitmap_object object) {}

void move_car_left() {
  if (player_car.x > BORDER_WIDTH - 1) {
    player_car.x -= 1;
  }
}

void move_car_right() {
  if (player_car.x < (LED_MATRIX_WIDTH - BORDER_WIDTH - CAR_WIDTH - 1)) {
    player_car.x += 1;
  }
}