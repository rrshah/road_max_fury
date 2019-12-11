#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "gpio.h"
#include "mp3_decoder_tasks.h"
#include "semphr.h"
#include "uart.h"

#define MP3_COMMAND_SIZE (8)

enum sounds { COUNTDOWN = 0, CRASH = 1, LEVEL_CHANGE = 2, CAR = 3, PLAY = 4, NO_SOUND = 5 };

SemaphoreHandle_t countdown;
SemaphoreHandle_t crash;
SemaphoreHandle_t level;
SemaphoreHandle_t car_moving;
SemaphoreHandle_t play;
SemaphoreHandle_t no_sound;

uint8_t mp3_player_init[] = {0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, 0x02, 0xEF};
uint8_t game_sounds[][MP3_COMMAND_SIZE] = {
    {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x01, 0xEF}, {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x02, 0xEF},
    {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x03, 0xEF}, {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x06, 0xEF},
    {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x09, 0xEF}, {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x07, 0xEF}};

static void send_mp3_command(uint8_t command[], int size) {
  for (int i = 0; i < size; i++) {
    uart__put(UART__3, command[i], 100);
  }
}

static void play_audio(int event_in_game) { send_mp3_command(game_sounds[event_in_game], MP3_COMMAND_SIZE); }

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
  QueueHandle_t rxq_handle = xQueueCreateStatic(sizeof(rxq_storage), sizeof(char), rxq_storage, &rxq_struct);
  QueueHandle_t txq_handle = xQueueCreateStatic(sizeof(txq_storage), sizeof(char), txq_storage, &txq_struct);

  uart__enable_queues(UART__3, txq_handle, rxq_handle);

  // Enable TX3, RX3 Pins
  gpio__construct_with_function(0, 1, GPIO__FUNCTION_2);
  gpio__construct_with_function(0, 0, GPIO__FUNCTION_2);
}

void mp3_player_task(void *pvParameters) {
  send_mp3_command(mp3_player_init, MP3_COMMAND_SIZE);
  uint8_t current_state = NO_SOUND;

  while (true) {
    if (xSemaphoreTake(countdown, 0) && current_state != COUNTDOWN) {
      play_audio(COUNTDOWN);
      current_state = COUNTDOWN;
    } else if (xSemaphoreTake(crash, 0) && current_state != CRASH) {
      play_audio(CRASH);
      current_state = CRASH;
    } else if (xSemaphoreTake(car_moving, 0) && current_state != CAR) {
      play_audio(CAR);
      current_state = CAR;
    } else if (xSemaphoreTake(no_sound, 0) && current_state != NO_SOUND) {
      play_audio(NO_SOUND);
      current_state = NO_SOUND;
    } else if (xSemaphoreTake(level, 0) && current_state != LEVEL_CHANGE) {
      play_audio(LEVEL_CHANGE);
      current_state = LEVEL_CHANGE;
    } else if (xSemaphoreTake(play, 0) && current_state != PLAY) {
      play_audio(PLAY);
      current_state = PLAY;
    }

    vTaskDelay(100);
  }
}

void mp3_decoder_init(void) {
  uart3_init();
  countdown = xSemaphoreCreateBinary();
  crash = xSemaphoreCreateBinary();
  level = xSemaphoreCreateBinary();
  car_moving = xSemaphoreCreateBinary();
  play = xSemaphoreCreateBinary();
  no_sound = xSemaphoreCreateBinary();
}