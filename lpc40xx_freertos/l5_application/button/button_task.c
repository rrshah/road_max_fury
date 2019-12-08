#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "button_task.h"
#include "gpio.h"
#include "gpio_isr.h"
#include "lpc_peripherals.h"

SemaphoreHandle_t button_pressed_signal;
static gpio_s button;

void button_isr(void) { xSemaphoreGiveFromISR(button_pressed_signal, NULL); }

void sleep_on_sem_task(void *p) {
  while (1) {
    if (xSemaphoreTake(button_pressed_signal, portMAX_DELAY)) {

      printf("Button Presed\n");
    }
  }
}

void init_button(void) { button = gpio__construct_as_input(GPIO__PORT_0, 9); }

void setup_button_isr(void) {
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, gpio__interrupt_dispatcher);
  gpio0__attach_interrupt(9, GPIO_INTR__FALLING_EDGE, button_isr);
}

void test_button_task(void *params) {
  init_button();
  setup_button_isr();

  button_pressed_signal = xSemaphoreCreateBinary();

  // xTaskCreate(sleep_on_sem_task, "sem", (512U * 4) / sizeof(void *), NULL, PRIORITY_LOW, NULL);

  while (1) {
    vTaskDelay(10);
  }
}