#pragma once
#include <stdbool.h>
#include <stdint.h>

enum move { DOWN = 0, DOWN_AND_LEFT_RIGHT = 1 };
enum direction { RIGHT = 0, LEFT = 1 };
enum screen_state { START_SCREEN = 0, GAME_SCREEN = 1, CAR_CRASH = 2 };

typedef struct {
  uint8_t level_obstacle_mod;
  uint8_t num_of_obstacles;
  uint8_t score_multiplier;
} level_t;

typedef struct b {
  uint8_t x;
  int8_t y;
  const uint8_t *image; // ptr->image
  bool isAlive;
  uint8_t color;
  uint8_t movement_type;
  uint8_t height, width;
  uint8_t speed;
  uint8_t counter;
  uint8_t direction;
} bitmap_object;

// void object__init();
void move_car_left();
void move_car_right();
void draw();
void move();
void generate_random_obstacles();
void object__init_player_car(void);
void draw_start_screen();
void collision_detector();
void draw_crash_screen();