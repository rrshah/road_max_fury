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

bitmap_object obstacle_cars[] = {{.x = 10,
                                  .y = BORDER_HEIGHT - CAR_HEIGHT_WITH_PADDING,
                                  .color = RED,
                                  .image = car,
                                  .movement_type = DOWN,
                                  .height = CAR_HEIGHT_WITH_PADDING,
                                  .width = CAR_WIDTH_WITH_PADDING,
                                  .speed = 1,
                                  .counter = 0},
                                 {.x = 20,
                                  .y = BORDER_HEIGHT - CAR_HEIGHT_WITH_PADDING,
                                  .color = YELLOW,
                                  .image = car,
                                  .movement_type = DOWN_AND_LEFT_RIGHT,
                                  .height = CAR_HEIGHT_WITH_PADDING,
                                  .width = CAR_WIDTH_WITH_PADDING,
                                  .speed = 30,
                                  .counter = 0}};

static bitmap_object player_car;
static bitmap_object car_obstacle[NUM_OF_OBSTACLES];

void object__draw(bitmap_object object) {
  drawBitmap(object.x, object.y, object.image, object.width, object.height,
             object.color);
}

void object__init_player_car(void) {
  player_car.x = 25;
  player_car.y = 10;
  player_car.height = 6;
  player_car.width = 6;
  player_car.color = CYAN;
  player_car.image = car;
}

static void move_obstacles(bitmap_object *obstacle) {
  obstacle->counter++;
  if (obstacle->counter < obstacle->speed) {
    return;
  }

  switch (obstacle->movement_type) {
  case DOWN:
    obstacle->y = obstacle->y - 1;
    break;
  case DOWN_AND_LEFT_RIGHT:
    obstacle->y = obstacle->y - 1;
    if (obstacle->direction == RIGHT) {
      if (obstacle->x <
          (LED_MATRIX_WIDTH - BORDER_WIDTH - CAR_WIDTH_WITH_PADDING)) {
        obstacle->x = obstacle->x + 1;
      } else {
        obstacle->direction = LEFT;
      }
    } else {
      if (obstacle->x > BORDER_WIDTH) {
        obstacle->x = obstacle->x - 1;
      } else {
        obstacle->direction = RIGHT;
      }
    }
  }
}

void move() {
  move_obstacles(&obstacle_cars[0]);
  move_obstacles(&obstacle_cars[1]);
}

static void draw_borders() {
  fillRect(0, 0, BORDER_WIDTH, BORDER_HEIGHT, GREEN);
  fillRect(30, 0, BORDER_WIDTH, BORDER_HEIGHT, GREEN);
}

// static void draw_score();
void draw_player_car() { object__draw(player_car); }
// static void draw_obstacles();

void draw() {
  draw_player_car();
  draw_borders();

  object__draw(obstacle_cars[0]);
  object__draw(obstacle_cars[1]);
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