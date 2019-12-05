#include "object.h"
#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>

#define BORDER_HEIGHT (52)
#define BORDER_WIDTH (2)
#define CAR_WIDTH (4)
#define CAR_HEIGHT (5)
#define CAR_HEIGHT_WITH_PADDING (6)
#define CAR_WIDTH_WITH_PADDING (6)
#define NUM_OF_OBSTACLES (10)

extern const uint8_t car[];
static uint8_t current_level = 1;
static uint8_t num_of_on_screen_obstacles = 0;
static bitmap_object player_car;
static bitmap_object car_obstacle[NUM_OF_OBSTACLES];
static level_t levels[] = {{
    .level_obstacle_mod = 1,
    .num_of_obstacles = 3,
}};

bitmap_object obstacle_types[] = {{.color = RED,
                                   .image = car,
                                   .movement_type = DOWN,
                                   .height = CAR_HEIGHT_WITH_PADDING,
                                   .width = CAR_WIDTH_WITH_PADDING,
                                   .speed = 20,
                                   .counter = 0,
                                   .isAlive = true},
                                  {.color = YELLOW,
                                   .image = car,
                                   .movement_type = DOWN_AND_LEFT_RIGHT,
                                   .height = CAR_HEIGHT_WITH_PADDING,
                                   .width = CAR_WIDTH_WITH_PADDING,
                                   .speed = 30,
                                   .counter = 0,
                                   .isAlive = true}};

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

  if (obstacle->y == (-1) * CAR_HEIGHT_WITH_PADDING) {
    obstacle->isAlive = false;
    num_of_on_screen_obstacles--;
  }
}

void move() {
  for (uint8_t i = 0; i < NUM_OF_OBSTACLES; i++) {
    if (car_obstacle[i].isAlive) {
      move_obstacles(&car_obstacle[i]);
    }
  }
}

static void draw_borders() {
  fillRect(0, 0, BORDER_WIDTH, BORDER_HEIGHT, GREEN);
  fillRect(30, 0, BORDER_WIDTH, BORDER_HEIGHT, GREEN);
  drawLine(0, BORDER_HEIGHT, 31, BORDER_HEIGHT, WHITE);
}

static void draw_player_car() { object__draw(player_car); }
static void draw_obstacles() {
  for (uint8_t i = 0; i < NUM_OF_OBSTACLES; i++) {
    if (car_obstacle[i].isAlive) {
      object__draw(car_obstacle[i]);
    }
  }
}

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

void draw() {
  draw_player_car();
  draw_borders();
  draw_obstacles();
}

static void generate_obstacle(bitmap_object *obstacle) {
  uint8_t x, index;

  obstacle->y = BORDER_HEIGHT - CAR_HEIGHT_WITH_PADDING;

  x = rand() % LED_MATRIX_WIDTH;
  if (x < BORDER_WIDTH) {
    x = BORDER_WIDTH;
  } else if (x > (LED_MATRIX_WIDTH - BORDER_WIDTH - CAR_WIDTH_WITH_PADDING)) {
    x = LED_MATRIX_WIDTH - BORDER_WIDTH - CAR_WIDTH_WITH_PADDING;
  }
  obstacle->x = x;

  index = rand() % levels[current_level - 1].level_obstacle_mod;

  obstacle->image = obstacle_types[index].image;
  obstacle->isAlive = true;
  obstacle->color = obstacle_types[index].color;
  obstacle->movement_type = obstacle_types[index].movement_type;
  obstacle->height = obstacle_types[index].height;
  obstacle->width = obstacle_types[index].width;
  obstacle->speed = obstacle_types[index].speed;
  obstacle->counter = 0;
  obstacle->direction = RIGHT;
}

void generate_random_obstacles() {
  static int counter = 0;
  counter++;
  if (counter < 20)
    return;
  if (num_of_on_screen_obstacles < levels[current_level - 1].num_of_obstacles) {
    for (uint8_t i = 0; i < NUM_OF_OBSTACLES; i++) {
      if (!car_obstacle[i].isAlive) {
        generate_obstacle(&car_obstacle[i]);
        break;
      }
    }

    num_of_on_screen_obstacles++;
  }
  counter = 0;
}