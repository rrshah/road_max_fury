#pragma once

#include <stdint.h>

#define CAR_WIDTH (4)
#define CAR_HEIGHT (5)
typedef struct car {
  uint8_t x;
  uint8_t y;
  uint8_t color;
} car_t;

void draw_car(car_t car);