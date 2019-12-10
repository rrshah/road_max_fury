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

#include "button_task.h"
#include "display_tasks.h"
#include "graphics.h"
#include "led_matrix.h"
#include "mp3_decoder_tasks.h"
#include "object.h"

static void blink_task(void *params);
static void accelerometer_task(void *params);

static gpio_s led0, led1;

int main(void) {
  srand(time(NULL));
  led_matrix__setupLedMatrixPins();
  led_matrix__setDefaultPinStates();

  mp3_decoder_init();

  led0 = board_io__get_led0();
  led1 = board_io__get_led1();

  xTaskCreate(blink_task, "led0", configMINIMAL_STACK_SIZE, (void *)&led0, PRIORITY_LOW, NULL);
  xTaskCreate(blink_task, "led1", configMINIMAL_STACK_SIZE, (void *)&led1, PRIORITY_LOW, NULL);
  // xTaskCreate(test_led_matrix_task, "led_matrix", (2048 / sizeof(void *)),
  // NULL, PRIORITY_LOW, NULL); xTaskCreate(test_graphics_task,
  // "test_graphics_task", 2048, NULL, PRIORITY_LOW, NULL);
  xTaskCreate(accelerometer_task, "acc_task", (1024 / sizeof(void *)), NULL, PRIORITY_LOW, NULL);
  xTaskCreate(display_task, "display_task", (4096 / sizeof(void *)), NULL, PRIORITY_LOW, NULL);

  xTaskCreate(mp3_player_task, "mp3_player_task", (2048 / sizeof(void *)), NULL, PRIORITY_LOW, NULL);

  sj2_cli__init();

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler
                         // runs out of memory and fails

  return 0;
}

static void accelerometer_task(void *params) {

  acceleration__axis_data_s acc_sensor_values;
  uint32_t y = 0;
  while (1) {
    acc_sensor_values = acceleration__get_data();
    y = acc_sensor_values.y;

    switch (y) {
    case 0 ... 150:
      // printf("Go straight, y value=%lu\n", y);
      break;

    case 151 ... 800:
      move_car_right();
      // printf("Go slight right, y value=%lu\n", y);
      break;

    case 3100 ... 3944:
      move_car_left();
      // printf("Go extreme left, y value=%lu\n", y);
      break;

    case 3945 ... 4095:
      // printf("Go straight, y value=%lu\n", y);
      break;

    default:
      break;
    }
    vTaskDelay(50);
  }
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
