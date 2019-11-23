#pragma once

#include "stdint.h"

void uart3_loopback_test(void *pvParameters);

void play_audio_test(void *pvParameters);

void mp3_player_task(void *pvParameters);

void play_audio(int even_group);

void send_mp3_command(uint8_t *command, int size);