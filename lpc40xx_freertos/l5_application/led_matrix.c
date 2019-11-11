#include "led_matrix.h"
#include "delay.h"
#include "gpio.h"
#include "string.h"
#include <stdio.h>

#define LED_MATRIX_HEIGHT 32
#define LED_MATRIX_WIDTH 64

#define ROW 32
#define COLUMN 64

// GPIO for driving the LED Matrix
static gpio_s P_addrA; // addrA
static gpio_s P_addrB; // addrB
static gpio_s P_addrC; // addrC
static gpio_s P_addrD; // addrD
static gpio_s P_LATCH; // LATCH
static gpio_s P_OE;    // OE
static gpio_s P_CLOCK; // CLOCK
static gpio_s P_R1;    // R1
static gpio_s P_G1;    // G1
static gpio_s P_B1;    // B1
static gpio_s P_R2;    // R2
static gpio_s P_G2;    // G2
static gpio_s P_B2;    // B2

static uint8_t led_matrix_buffer[ROW / 2][COLUMN];
static uint8_t row_to_display = 15;

static void led_matrix__setup_pins(void) {
  P_addrA = gpio__construct_as_output(GPIO__PORT_0, 26);
  P_addrB = gpio__construct_as_output(GPIO__PORT_1, 31);
  P_addrC = gpio__construct_as_output(GPIO__PORT_1, 20);
  P_addrD = gpio__construct_as_output(GPIO__PORT_1, 28);
  P_LATCH = gpio__construct_as_output(GPIO__PORT_2, 2);
  P_OE = gpio__construct_as_output(GPIO__PORT_2, 5);
  P_CLOCK = gpio__construct_as_output(GPIO__PORT_2, 0);
  P_R1 = gpio__construct_as_output(GPIO__PORT_1, 0);
  P_G1 = gpio__construct_as_output(GPIO__PORT_1, 1);
  P_B1 = gpio__construct_as_output(GPIO__PORT_1, 4);
  P_R2 = gpio__construct_as_output(GPIO__PORT_4, 28);
  P_G2 = gpio__construct_as_output(GPIO__PORT_0, 6);
  P_B2 = gpio__construct_as_output(GPIO__PORT_0, 8);
}

static void led_matrix__set_default_pin_states(void) {

  // Enable all comm & address pins as outputs, set default states:
  gpio__reset(P_CLOCK);
  gpio__reset(P_LATCH);
  gpio__set(P_OE);
  gpio__reset(P_addrA);
  gpio__reset(P_addrB);
  gpio__reset(P_addrC);
  gpio__reset(P_addrD);
  gpio__reset(P_R1);
  gpio__reset(P_G1);
  gpio__reset(P_B1);
  gpio__reset(P_R2);
  gpio__reset(P_G2);
  gpio__reset(P_B2);

  // TODO rit_enable(refresh, 1);
  // TODO refreshTimer.begin(refreshISR, 200, uSec);
}

// static void led_matrix__updateDisplay(void) {
void led_matrix__updateDisplay(void) {
  gpio__set(P_OE);      // Disable LED output during row/plane switchover
  gpio__set(P_LATCH);   // Latch data loaded during *prior* interrupt
  gpio__reset(P_CLOCK); // Start the clock LOW

  if (++row_to_display > 15) {
    row_to_display = 0;
  }
  // RESET timer duration
  // refreshTimer.resetPeriod_SIT(duration, uSec);

  gpio__reset(P_OE);    // Re-enable output
  gpio__reset(P_LATCH); // Latch down

  for (uint32_t i = 0; i < 64; i++) {
    (led_matrix_buffer[row_to_display][i] & 0x04) ? gpio__set(P_R1)
                                                  : gpio__reset(P_R1); // R1
    (led_matrix_buffer[row_to_display][i] & 0x02) ? gpio__set(P_G1)
                                                  : gpio__reset(P_G1); // G1
    (led_matrix_buffer[row_to_display][i] & 0x01) ? gpio__set(P_B1)
                                                  : gpio__reset(P_B1); // B1
    (led_matrix_buffer[row_to_display][i] & 0x20) ? gpio__set(P_R2)
                                                  : gpio__reset(P_R2); // R2
    (led_matrix_buffer[row_to_display][i] & 0x10) ? gpio__set(P_G2)
                                                  : gpio__reset(P_G2); // G2
    (led_matrix_buffer[row_to_display][i] & 0x08) ? gpio__set(P_B2)
                                                  : gpio__reset(P_B2); // B2

    gpio__set(P_CLOCK);   // hi
    gpio__reset(P_CLOCK); // lo
  }
}

void led_matrix__init(void) {
  led_matrix__setup_pins();
  led_matrix__set_default_pin_states();

  memset(led_matrix_buffer, 0, sizeof(led_matrix_buffer));
}

void drawPixel(int16_t x, int16_t y, uint16_t c) {
  if ((y < 0) || (y >= LED_MATRIX_WIDTH) || (x < 0) ||
      (x >= LED_MATRIX_HEIGHT)) {
    return;
  }

  if (x > 15) {
    led_matrix_buffer[x - 16][y] |= (c << 3);
  } else {
    led_matrix_buffer[x][y] |= c;
  }
}