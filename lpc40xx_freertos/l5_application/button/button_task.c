#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "button_task.h"
#include "gpio.h"
#include "gpio_isr.h"
#include "lpc_peripherals.h"

static SemaphoreHandle_t button_pressed_signal;
static gpio_s button;
static void setup_button_isr(void);
static void init_button(void);

void button_isr(void) { xSemaphoreGiveFromISR(button_pressed_signal, NULL); }

void button_task(void *params) {
  init_button();
  setup_button_isr();

  while (1) {
    if (xSemaphoreTake(button_pressed_signal, portMAX_DELAY)) {
      printf("BT Press\n");
    }
    vTaskDelay(10);
  }
}

static void init_button(void) { button = gpio__construct_as_input(GPIO__PORT_0, 9); }

static void setup_button_isr(void) {
  button_pressed_signal = xSemaphoreCreateBinary();
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, gpio__interrupt_dispatcher);
  gpio0__attach_interrupt(9, GPIO_INTR__FALLING_EDGE, button_isr);
}