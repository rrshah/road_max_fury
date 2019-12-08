#pragma once

#include "stdint.h"

void test_button_task(void *params);
void sleep_on_sem_task(void *p);
void init_button(void);
void setup_button_isr(void);
