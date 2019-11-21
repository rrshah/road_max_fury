#include "car.h"
#include "graphics.h"

void draw_car(car_t car) {
  drawLine(car.x + 1, car.y, car.x + 1, car.y + 4, car.color);
  drawLine(car.x + 2, car.y, car.x + 2, car.y + 4, car.color);

  drawPixel(car.x, car.y + 1, car.color);
  drawPixel(car.x, car.y + 3, car.color);
  drawPixel(car.x + 3, car.y + 1, car.color);
  drawPixel(car.x + 3, car.y + 3, car.color);
  /*
    drawPixel(car.x + 1, car.y, car.color);
    drawPixel(car.x + 2, car.y, car.color);

    drawPixel(car.x, car.y + 1, car.color);
    drawPixel(car.x + 1, car.y + 1, car.color);
    drawPixel(car.x + 2, car.y + 1, car.color);
    drawPixel(car.x + 3, car.y + 1, car.color);

    drawPixel(car.x + 1, car.y + 2, car.color);
    drawPixel(car.x + 2, car.y + 2, car.color);

    drawPixel(car.x, car.y + 3, car.color);
    drawPixel(car.x + 1, car.y + 3, car.color);
    drawPixel(car.x + 2, car.y + 3, car.color);
    drawPixel(car.x + 3, car.y + 3, car.color);

    drawPixel(car.x + 1, car.y + 4, car.color);
    drawPixel(car.x + 2, car.y + 4, car.color);
  */
}