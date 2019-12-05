
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "display_tasks.h"
#include "graphics.h"
#include "led_matrix.h"
#include "object.h"

void display_task(void *params) {
  object__init_player_car();
  generate_random_obstacles();

  while (true) {
    move();
    draw();

    vTaskDelay(30);
  }
}

// smiley
static const uint8_t smiley[] = {0x3c, 0x42, 0x99, 0xa5, 0x81,
                                 0xa5, 0x81, 0x42, 0x3c};

void test_graphics_task(void *params) {

  // drawLine(0, 0, 31, 63, red_4);
  // drawRect(0, 0, 10, 20, white_7);
  // fillRect(0, 0, 20, 10, YELLOW);
  // drawChar(5, 5, 'R', red_4, red_4, 1);
  drawBitmap(15, 31, smiley, 8, 9, RED);

  while (true) {
    // draw_car(player_car);
    vTaskDelay(1);
  }
}