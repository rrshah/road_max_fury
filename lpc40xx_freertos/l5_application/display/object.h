#pragma once
#include <stdbool.h>
#include <stdint.h>

enum move { DOWN = 0, DOWN_AND_LEFT_RIGHT = 1 };

typedef struct b {
  uint8_t x, y;
  const uint8_t *image; // ptr->image
  bool isAlive;
  uint8_t color;
  uint8_t movement_direction;
  uint8_t height, width;
  uint8_t speed;
  uint8_t counter;
} bitmap_object;

// void object__init();
void move_car_left();
void move_car_right();
void draw();
void move();
void object__init_player_car(void);
void draw_player_car();