/*
scons
python nxp-programmer/flash.py

*/

#include <stdio.h>

#include "FreeRTOS.h"
#include "acceleration.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "sj2_cli.h"
#include "task.h"

#include "car.h"
#include "graphics.h"
#include "led_matrix.h"

static void blink_task(void *params);
static void led_matrix_task(void *params);
static void accelerometer_task(void *params);

static gpio_s led0, led1;

car_t player_car;

int main(void) {
  led_matrix__setupLedMatrixPins();
  led_matrix__setDefaultPinStates();

  player_car.x = 15;
  player_car.y = 10;
  player_car.color = CYAN;
  led0 = board_io__get_led0();
  led1 = board_io__get_led1();

  xTaskCreate(blink_task, "led0", configMINIMAL_STACK_SIZE, (void *)&led0,
              PRIORITY_LOW, NULL);
  xTaskCreate(blink_task, "led1", configMINIMAL_STACK_SIZE, (void *)&led1,
              PRIORITY_LOW, NULL);
  xTaskCreate(led_matrix_task, "led_matrix", (2048 / sizeof(void *)), NULL,
              PRIORITY_LOW, NULL);
  xTaskCreate(accelerometer_task, "acc_task", 2048, NULL, PRIORITY_LOW, NULL);

  sj2_cli__init();

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler
                         // runs out of memory and fails

  return 0;
}

void move_car_left() {
  led_matrix__turnOffAllPixels();
  if (player_car.x > 0) {
    player_car.x -= 1;
  }
}

void move_car_right() {
  led_matrix__turnOffAllPixels();
  if (player_car.x < (31 - 3)) {
    player_car.x += 1;
  }
}

static void accelerometer_task(void *params) {
  acceleration__axis_data_s acc_sensor_values;
  uint32_t y = 0;
  while (1) {
    acc_sensor_values = acceleration__get_data();
    y = acc_sensor_values.y;

    switch (y) {
    case 0 ... 150:
      printf("Go straight, y value=%lu\n", y);
      break;

    case 151 ... 800:
      move_car_right();
      printf("Go slight right, y value=%lu\n", y);
      break;

    case 3100 ... 3944:
      move_car_left();
      printf("Go extreme left, y value=%lu\n", y);
      break;

    case 3945 ... 4095:
      printf("Go straight, y value=%lu\n", y);
      break;

    default:
      break;
    }
    vTaskDelay(100);
  }
}

static void led_matrix_task(void *params) {

  // led_matrix__drawAllPixels(white_7);
  // led_matrix__turnOnAllPixels(red_4);
  // led_matrix__drawPixel(0, 0, white_7);
  // led_matrix__drawPixel(5, 1, red_4);

  // led_matrix__drawPixel(21, 21, yellow_6);
  // led_matrix__drawPixel(25, 43, magenta_5);
  // led_matrix__drawPixel(20, 20, red_4);

  // led_matrix__drawPixel(31, 0, white_7);
  // led_matrix__drawPixel(31, 31, blue_1);
  // led_matrix__drawPixel(31, 63, green_2);

  // drawLine(0, 0, 31, 63, red_4);
  // drawRect(0, 0, 10, 20, white_7);
  // fillRect(0, 0, 20, 10, green_2);
  // drawPixel(19, 20, yellow_6);
  // led_matrix__updateDisplay();
  // drawChar(5, 5, 'R', red_4, red_4, 1);

  while (true) {
    draw_car(player_car);
    // led_matrix__turnOnAllPixels(red_4);
    // printf("LED ON..\n");
    // led_matrix__updateDisplay();
    // vTaskDelay(1);
    // led_matrix__turnOffAllPixels();
    // printf("LED OFF..\n");
    // led_matrix__updateDisplay();
    // drawPixel(pixel_x, pixel_y, red_4);
    vTaskDelay(1);
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