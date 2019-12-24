/*
scons
python nxp-programmer/flash.py
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "acceleration.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "sj2_cli.h"
#include "uart.h"

#include "accelerometer_task.h"
#include "button_task.h"
#include "game_tasks.h"
#include "led_matrix.h"
#include "mp3_decoder_tasks.h"
#include "object.h"

static void blink_task(void *params);

static gpio_s led0, led1;

int main(void) {
  led_matrix__setupLedMatrixPins();
  led_matrix__setDefaultPinStates();

  mp3_decoder_init();

  led0 = board_io__get_led0();
  led1 = board_io__get_led1();

  xTaskCreate(blink_task, "led0", configMINIMAL_STACK_SIZE, (void *)&led0, PRIORITY_LOW, NULL);
  xTaskCreate(blink_task, "led1", configMINIMAL_STACK_SIZE, (void *)&led1, PRIORITY_LOW, NULL);
  xTaskCreate(accelerometer_task, "acc_task", (1024 / sizeof(void *)), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(game_task, "game_task", (2048 / sizeof(void *)), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(button_task, "button_task", (1024 / sizeof(void *)), NULL, PRIORITY_HIGH, NULL);
  xTaskCreate(mp3_player_task, "mp3_player_task", (1024 / sizeof(void *)), NULL, PRIORITY_LOW, NULL);

  sj2_cli__init();

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler
                         // runs out of memory and fails

  return 0;
}

static void blink_task(void *params) {
  const gpio_s led = *((gpio_s *)params);

  // Warning: This task starts with very minimal stack, so do not use printf()
  // API here to avoid stack overflow
  while (true) {
    gpio__toggle(led);
    vTaskDelay(500);
  }
}
