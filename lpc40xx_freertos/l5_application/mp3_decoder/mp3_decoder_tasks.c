#include "mp3_decoder_tasks.h"
#include "uart.h"

#include "queue.h"
#include <stdio.h>
#include <string.h>

uint8_t mp3PLayer_init[] = {0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, 0x02, 0xEF};
uint8_t car_sound[] = {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x04, 0xEF};

uint8_t MP3_command_size = 8;

extern QueueHandle_t MP3_decoder_queue;

void play_audio_test(void *pvParameters) {

  while (1) {
    int first_song = 2;

    int mp3_init = 1;
    if (xQueueSend(MP3_decoder_queue, &mp3_init, 100)) {
      printf("sent even numbeer%i\n", mp3_init);
    } else {
      printf("event sending failed\n");
    }

    if (xQueueSend(MP3_decoder_queue, &first_song, 100)) {
      printf("first song played%i\n", first_song);
    } else {
      printf("first song playing failed\n");
    }
    printf("Mp3 decoder task is running\n");
    vTaskDelay(2000);
  }
}

void mp3_player_task(void *pvParameters) {
  int event_in_game;
  while (1) {
    if (xQueueReceive(MP3_decoder_queue, &event_in_game, 100)) {
      printf("event occured=%i\n", event_in_game);
      play_audio(event_in_game);
    } else {
      printf("event receive failed\n");
    }
  }
  vTaskDelay(100);
}

void play_audio(int event_in_game) {
  switch (event_in_game) {
  case 1:
    send_mp3_command(mp3PLayer_init, MP3_command_size);
    break;

  case 2:
    send_mp3_command(car_sound, MP3_command_size);
    break;

  default:
    break;
  }
}

void send_mp3_command(uint8_t command[], int size) {
  for (int i = 0; i < size; i++) {
    uart__put(UART__3, command[i], 100);
  }
}

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