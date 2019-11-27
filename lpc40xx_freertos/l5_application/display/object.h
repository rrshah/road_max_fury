#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct b {
  uint8_t x, y;
  const uint8_t *image; // ptr->image
  bool isAlive;
  uint8_t color;
  uint8_t movement_direction;
  uint8_t height, width;
  uint8_t speed;
} bitmap_object;

// void object__init();
void move_car_left();
void move_car_right();
void draw();
void move();