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

void uart3_Rx_test(void *pvParameters) {
  // char str[] = "Loop Test\n";
  char input_byte = '\0';

  while (1) {

    // Get same string on UART3
    while (uart__get(UART__3, &input_byte, 10)) {
      uart__put(UART__0, input_byte, 2);
    }
    vTaskDelay(5);
  }
}

void wifi_test(void *pvParameters) {
  char AT_String[] = "AT\r\n";
  // char AT_String[] = "AT+CWMODE=3\r\n";
  char input_byte = '\0';

  while (1) {
    for (int i = 0; i < strlen(AT_String); i++) {
      uart__put(UART__3, AT_String[i], 100);
    }

    // Should receive "OK"
    while (uart__get(UART__3, &input_byte, 10)) {
      uart__put(UART__0, input_byte, 10);
    }
    vTaskDelay(50);
  }
}