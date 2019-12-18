
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "display_tasks.h"
#include "graphics.h"
#include "led_matrix.h"
#include "object.h"
#include "queue.h"
#include "semphr.h"

extern uint8_t game_screen_state;
extern SemaphoreHandle_t crash;
extern SemaphoreHandle_t car_moving;
extern SemaphoreHandle_t no_sound;
extern SemaphoreHandle_t change_game_state;

static void draw_game_screen() {
  generate_random_obstacles();
  move();
  draw();
  collision_detector();
}

void display_task(void *params) {
  bool change_state = false;

  game_screen_state = START_SCREEN;

  while (true) {
    if (xSemaphoreTake(change_game_state, 0)) {
      change_state = true;
    }

    switch (game_screen_state) {
    case START_SCREEN:
      xSemaphoreGive(no_sound);
      draw_start_screen();
      game_init();
      if (change_state) {
        led_matrix__turnOffAllPixels();
        game_screen_state = COUNTDOWN_SCREEN;
        change_state = false;
      }
      break;
    case COUNTDOWN_SCREEN:
      draw_countdown_screen();
      if (change_state) {
        change_state = false;
      }
      game_screen_state = GAME_SCREEN;
      break;
    case GAME_SCREEN:
      xSemaphoreGive(car_moving);
      draw_game_screen();
      if (change_state) {
        game_screen_state = PAUSE_SCREEN;
        change_state = false;
      }
      break;
    case PAUSE_SCREEN:
      xSemaphoreGive(no_sound);
      if (change_state) {
        game_screen_state = GAME_SCREEN;
        change_state = false;
      }
      break;
    case CAR_CRASH:
      xSemaphoreGive(crash);
      draw_crash_screen();
      vTaskDelay(5000);
      led_matrix__turnOffAllPixels();
      game_screen_state = GAME_OVER;
      break;
    case GAME_OVER:
      draw_gameover_screen();
      xSemaphoreGive(no_sound);
      if (change_state) {
        game_screen_state = START_SCREEN;
        change_state = false;
        led_matrix__turnOffAllPixels();
      }
      break;
    }

    vTaskDelay(50);
  }
}