#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "acceleration.h"
#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "sj2_cli.h"

static void blink_task(void *params);
static void accelerometer_task(void *params);

static gpio_s led0, led1;
acceleration__axis_data_s acc_sensor_values;
uint32_t x = 0, y = 0, z = 0;

int main(void) {
  led0 = board_io__get_led0();
  led1 = board_io__get_led1();

  xTaskCreate(blink_task, "led0", configMINIMAL_STACK_SIZE, (void *)&led0, PRIORITY_LOW, NULL);
  xTaskCreate(blink_task, "led1", configMINIMAL_STACK_SIZE, (void *)&led1, PRIORITY_LOW, NULL);
  xTaskCreate(accelerometer_task, "acc_task", 2048, NULL, PRIORITY_LOW, NULL);

#if 0
#else
  sj2_cli__init();

#endif

  puts("Starting RTOS");
  vTaskStartScheduler();

  return 0;
}

static void blink_task(void *params) {
  const gpio_s led = *((gpio_s *)params);

  gpio__toggle(led);
  vTaskDelay(500);
}

static void accelerometer_task(void *params) {
  while (1) {

    acc_sensor_values = acceleration__get_data();
    x = acc_sensor_values.x;
    y = acc_sensor_values.y;
    z = acc_sensor_values.z;
    // printf("x value= %i , y value = %i , z value = %i\n", x,y,z;
    // printf("y values=%i\n", y);

    switch (y) {

    case 0 ... 150:
      printf("Go straight, y value=%i!!\n\n", y);
      break;

    case 151 ... 265:
      printf("Go slight right, y value=%i !!\n", y);
      break;

    case 266 ... 380:
      printf("Go more right, y value=%i !!\n", y);
      break;

    case 381 ... 495:
      printf("Go extreme right, y value=%i !!\n", y);
      break;

    case 496 ... 800:
      printf("Go extreme right, y value=%i !!\n", y);
      break;

    case 3100 ... 3599:
      printf("Go extreme left, y value=%i !!\n", y);
      break;

    case 3830 ... 3944:
      printf("Go go slight left, y value=%i !!\n", y);
      break;

    case 3715 ... 3829:
      printf("Go go more left, y value=%i !!\n", y);
      break;

    case 3600 ... 3714:
      printf("Go extreme left, y value=%i !!\n", y);
      break;

    case 3945 ... 4095:
      printf("Go straight, y value=%i\n!!", y);
      break;

    default:
      break;
    }
    vTaskDelay(100);
  }
}