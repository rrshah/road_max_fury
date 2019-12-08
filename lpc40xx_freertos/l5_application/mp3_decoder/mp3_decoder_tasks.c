#include <stdio.h>
#include <string.h>

#include "mp3_decoder_tasks.h"
//#include "semphr.h"
#include "gpio.h"
#include "uart.h"

#define MP3_COMMAND_SIZE (8)

enum sounds {
  COUNTDOWN = 0,
  CRASH = 1,
  LEVEL_CHANGE = 2,
  CAR = 3,
  PLAY = 4,
  NO_SOUND = 5
};

uint8_t mp3_player_init[] = {0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, 0x02, 0xEF};
uint8_t game_sounds[][MP3_COMMAND_SIZE] = {
    {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x01, 0xEF},
    {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x02, 0xEF},
    {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x03, 0xEF},
    {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x06, 0xEF},
    {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x09, 0xEF},
    {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x07, 0xEF}};

static void send_mp3_command(uint8_t command[], int size) {
  for (int i = 0; i < size; i++) {
    uart__put(UART__3, command[i], 100);
  }
}

static void play_audio(int event_in_game) {
  send_mp3_command(game_sounds[event_in_game], MP3_COMMAND_SIZE);
}

static void uart3_init(void) {

  // Enable peripheral
  uart__init(UART__3, clock__get_peripheral_clock_hz(), 9600);

  // Memory for the queue data structure
  static StaticQueue_t rxq_struct;
  static StaticQueue_t txq_struct;

  // Memory where the queue actually stores the data
  static uint8_t rxq_storage[32];
  static uint8_t txq_storage[128];

  // Make UART more efficient by backing it with RTOS queues (optional but
  // highly recommended with RTOS)
  QueueHandle_t rxq_handle = xQueueCreateStatic(
      sizeof(rxq_storage), sizeof(char), rxq_storage, &rxq_struct);
  QueueHandle_t txq_handle = xQueueCreateStatic(
      sizeof(txq_storage), sizeof(char), txq_storage, &txq_struct);

  uart__enable_queues(UART__3, txq_handle, rxq_handle);

  // Enable TX3, RX3 Pins
  gpio__construct_with_function(0, 1, GPIO__FUNCTION_2);
  gpio__construct_with_function(0, 0, GPIO__FUNCTION_2);
}

void mp3_player_task(void *pvParameters) {
  send_mp3_command(mp3_player_init, MP3_COMMAND_SIZE);
  uint8_t current_state = NO_SOUND;

  while (true) {
    play_audio(CRASH);
    /*    if (xSemaphoreReceive()) {
        }*/
    vTaskDelay(1000);
  }
}

void mp3_decoder_init(void) { uart3_init(); }