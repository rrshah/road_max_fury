#include "app_tasks.h"
#include "uart.h"

#include <stdio.h>
#include <string.h>

void uart3_loopback_test(void *pvParameters) {

  char str[] = "Loop Test\n";
  char input_byte = '\0';
  while (1) {
    // Send String on UART3
    for (int i = 0; i < strlen(str); i++) {
      uart__put(UART__3, str[i], 100);
    }

    // Get same string on UART3
    while (uart__get(UART__3, &input_byte, 10)) {
      uart__put(UART__0, input_byte, 2);
    }

    // printf("Loopback is running.\n");
    vTaskDelay(500);
  }
}

void play_audio_test(void *pvParameters) {

  uint8_t cmd_1[] = {0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, 0x02, 0xEF};
  uint8_t cmd_2[] = {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x04, 0xEF};

  while (1) {
    // Send Command 1
    for (int i = 0; i < 8; i++) {
      uart__put(UART__3, cmd_1[i], 100);
    }

    vTaskDelay(10);

    // Send Command 2
    for (int i = 0; i < 8; i++) {
      uart__put(UART__3, cmd_2[i], 100);
    }

    printf("Mp3 decoder task is running\n");
    vTaskDelay(5000000);
  }
}