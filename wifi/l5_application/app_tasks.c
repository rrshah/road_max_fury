#include "app_tasks.h"
#include "uart.h"

#include <stdio.h>
#include <string.h>

void uart3_loopback_test(void *pvParameters) {
  // uart3_init();
  char str[] = "Loop Test\n";
  char input_byte = '\0';

  while (1) {
    // Send String on UART3
    for (int i = 0; i < strlen(str); i++) {
      uart__put(UART__3, str[i], 100);
    }

    vTaskDelay(50);

    // Get same string on UART3
    while (uart__get(UART__3, &input_byte, 10)) {
      uart__put(UART__0, input_byte, 2);
    }

    printf("Wifi Task runnning: %c\n", input_byte);
    vTaskDelay(50);
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
  // char AT_String[] = "LOOP\r\n";
  // char AT_String[] = "AT\r\n";
  char AT_String[] = "AT+CWMODE=3\r\n";
  // char AT_String[] = "AT+GMR\r\n";
  uint8_t len = strlen(AT_String);
  char input_byte = '\0';

  char DisableEchoCommand[] = "ATE1\r\n";
  uint8_t len_echo = strlen(DisableEchoCommand);
  for (uint8_t i = 0; i < len_echo; i++) {
    uart__put(UART__3, DisableEchoCommand[i], 100);
    vTaskDelay(1);
  }

  while (1) {
    for (uint8_t i = 0; i < len; i++) {
      uart__put(UART__3, AT_String[i], 100);
      vTaskDelay(1);
    }
    vTaskDelay(100);

    // Should receive "OK"
    while (uart__get(UART__3, &input_byte, 100)) {
      uart__put(UART__0, input_byte, 100);
    }
    vTaskDelay(500);
    // printf("Wifi run\n");
  }
}

/*
static constexpr uint32_t kBaudRate = 115200;
  // Confirmation responses recieved from the ESP8266 module
  static constexpr uint8_t kConfirmationResponse[] = "\r\nOK";
  static constexpr uint8_t kSendRequestConfirmationResponse[] = "\r\nSEND OK";
  // Error response recieved from ESP8266 module
  static constexpr uint8_t kErrorResponse[] = "\r\nERROR";
  // Commands being sent to the ESP8266 module
  static constexpr uint8_t kTestCommand[] = "AT\r\n";
  static constexpr uint8_t kDisableEchoCommand[] = "ATE0\r\n";
  static constexpr uint8_t kClientModeCommand[] = "AT+CWMODE=1\r\n";
  static constexpr uint8_t kResetCommand[] = "AT+RST\r\n";
  static constexpr char kDeepSleepCommand[] = "AT+GSLP=%" PRIu32 "\r\n";
  static constexpr uint8_t kGetModuleVersionCommand[] = "AT+GMR\r\n";
  static constexpr uint8_t kGetNetworkConnectionInfoCommand[] =
    "AT+CIPSTATUS\r\n";
  static constexpr char kConnectToAccessPointCommand[] =
    "AT+CWJAP=\"%s\",\"%s\"\r\n";
  static constexpr uint8_t kDisconnectFromAccessPointCommand[] =
    "AT+CWQAP\r\n";
  static constexpr char kConnectToServerCommand[] =
    "AT+CIPSTART=\"%s\",\"%s\",%" PRIu16 "\r\n";
  static constexpr uint8_t kDisconnectFromServerCommand[] = "AT+CIPCLOSE\r\n";
  static constexpr char kSendDataCommand[] = "AT+CIPSEND=%" PRIu32 "\r\n";
  // GET request format
  static constexpr char kGetRequest[] =
    "GET %s HTTP/1.1\r\n"
    "Host: %.*s\r\n"
    "\r\n";
  // POST request format
  static constexpr char kPostRequest[] =
    "POST %s HTTP/1.1\r\n"
    "Host: %.*s\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "Content-Length: %" PRIu32 "\r\n"
    "\r\n"
    "%s\r\n"
    "\r\n";

*/