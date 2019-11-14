/*
scons
python nxp-programmer/flash.py

*/

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "board_io.h"
#include "common_macros.h"
#include "gpio.h"
#include "led_matrix.h"
#include "sj2_cli.h"

static void blink_task(void *params);
static void uart_task(void *params);
static void led_matrix_task(void *params);

static gpio_s led0, led1;

int main(void) {

  led0 = board_io__get_led0();
  led1 = board_io__get_led1();

  xTaskCreate(blink_task, "led0", configMINIMAL_STACK_SIZE, (void *)&led0,
              PRIORITY_LOW, NULL);
  xTaskCreate(blink_task, "led1", configMINIMAL_STACK_SIZE, (void *)&led1,
              PRIORITY_LOW, NULL);
  xTaskCreate(led_matrix_task, "led_matrix", (4096 / sizeof(void *)), NULL,
              PRIORITY_HIGH, NULL);

  // It is advised to either run the uart_task, or the SJ2 command-line (CLI),
  // but not both Change '#if 0' to '#if 1' and vice versa to try it out
#if 0
  // printf() takes more stack space, size this tasks' stack higher
  xTaskCreate(uart_task, "uart", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
#else
  sj2_cli__init();
  UNUSED(uart_task); // uart_task is un-used in if we are doing cli init()
#endif

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler
                         // runs out of memory and fails

  return 0;
}

static void led_matrix_task(void *params) {
  uint8_t led_matrix_buffer[16][64];
  led_matrix__init(led_matrix_buffer);
  led_matrix__setupLedMatrixPins();
  led_matrix__setDefaultPinStates();

  // led_matrix__drawAllPixels(white_7);
  //   led_matrix__drawPixel(0, 0, white_7);
  //   led_matrix__drawPixel(5, 1, red_4);

  //   led_matrix__drawPixel(21, 21, yellow_6);
  //   led_matrix__drawPixel(25, 43, magenta_5);
  //   led_matrix__drawPixel(20, 20, red_4);

  //   led_matrix__drawPixel(31, 0, white_7);
  //   led_matrix__drawPixel(31, 31, blue_1);
  //   led_matrix__drawPixel(31, 63, green_2);

  // led_matrix__updateDisplay();
  while (true) {
    led_matrix__turnOnAllPixels(red_4);
    printf("LED ON..\n");
    led_matrix__updateDisplay();
    vTaskDelay(1000);
    led_matrix__turnOffAllPixels();
    printf("LED OFF..\n");
    led_matrix__updateDisplay();
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

// This sends periodic messages over printf() which uses system_calls.c to send
// them to UART0
static void uart_task(void *params) {
  TickType_t previous_tick = 0;
  TickType_t ticks = 0;

  while (true) {
    // This loop will repeat at precise task delay, even if the logic below
    // takes variable amount of ticks
    vTaskDelayUntil(&previous_tick, 2000);

    /* Calls to fprintf(stderr, ...) uses polled UART driver, so this entire
     * output will be fully sent out before this function returns. See
     * system_calls.c for actual implementation.
     *
     * Use this style print for:
     *  - Interrupts because you cannot use printf() inside an ISR
     *  - During debugging in case system crashes before all output of printf()
     * is sent
     */
    ticks = xTaskGetTickCount();
    fprintf(stderr,
            "%u: This is a polled version of printf used for debugging ... "
            "finished in",
            (unsigned)ticks);
    fprintf(stderr, " %lu ticks\n", (xTaskGetTickCount() - ticks));

    /* This deposits data to an outgoing queue and doesn't block the CPU
     * Data will be sent later, but this function would return earlier
     */
    ticks = xTaskGetTickCount();
    printf("This is a more efficient printf ... finished in");
    printf(" %lu ticks\n\n", (xTaskGetTickCount() - ticks));
  }
}
