#include "object.h"
#include "graphics.h"

#define NUM_OF_OBSTACLES 4

static bitmap_object car_obstacle[NUM_OF_OBSTACLES];

void object__init() {
  //   for (i = 0; i < 4; i++) {

  //     car_obstacle[i].x = 20 * i;

  //     car_obstacle[i].y = 10;

  //     car_obstacle[i].image = car;

  //     car_obstacle[i].life = 1;
  //   }
}

void object__move(bitmap_object object) {}

void object__draw(bitmap_object object) {
  drawBitmap(object.x, object.y, object.image, object.width, object.height, object.color);
}