#include "screen.h"
#include "graphics.h"

void draw_borders() {
  fillRect(0, 0, 3, 51, GREEN);
  fillRect(28, 0, 3, 51, GREEN);
  drawLine(0, 51, 31, 51, WHITE);
}