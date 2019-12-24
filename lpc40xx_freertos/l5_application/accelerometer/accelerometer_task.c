#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "acceleration.h"
#include "accelerometer_task.h"
#include "object.h"

void accelerometer_task(void *params) {

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