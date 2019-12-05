
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "display_tasks.h"
#include "graphics.h"
#include "led_matrix.h"
#include "object.h"

extern uint8_t game_screen_state;

static void draw_game_screen() {
  generate_random_obstacles();
  move();
  draw();
  collision_detector();
}

void display_task(void *params) {

  object__init_player_car();
  game_screen_state = START_SCREEN;

  while (true) {
    switch (game_screen_state) {
    case START_SCREEN:
      draw_start_screen();
      break;
    case GAME_SCREEN:
      draw_game_screen();
      break;
    case CAR_CRASH:
      // led_matrix__turnOnAllPixels(RED);
      break;
    }

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
