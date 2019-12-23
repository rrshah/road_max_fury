
#include <stdio.h>
#include <string.h>

#include "clock.h"
#include "delay.h"
#include "gpio.h"
#include "hw_timer.h"
#include "led_matrix.h"
#include "sys_time.h"

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

static const lpc_timer_e led_matrix__refresh_timer = LPC_TIMER__1;
static const lpc_timer__mr_e led_matrix__refresh_timer_mr = LPC_TIMER__MR1;

uint8_t led_matrix_buffer[16][64] = {0};
static uint8_t row_to_display = 15;

bool led_matrix__drawPixel(int16_t x, int16_t y, uint16_t color) {
  uint8_t mask = 7;
  if ((y < 0) || (y >= LED_MATRIX_HEIGHT) || (x < 0) || (x >= LED_MATRIX_WIDTH)) {
    return false;
  }

  if (x < 16) {
    led_matrix_buffer[x][y] &= ~mask;
    led_matrix_buffer[x][y] |= color;
  } else {
    led_matrix_buffer[x - 16][y] &= ~(mask << 3);
    led_matrix_buffer[x - 16][y] |= (color << 3);
  }
  return true;
}

void drawPixel(int16_t x, int16_t y, uint16_t c) { led_matrix__drawPixel(x, y, c); }

void led_matrix__turnOnAllPixels(uint16_t color) {
  for (int i = 0; i < LED_MATRIX_ROWS; i++) {
    memset(led_matrix_buffer[i], (uint8_t)((color << 3) | color), LED_MATRIX_COLUMNS);
  }
}
void led_matrix__turnOffAllPixels() { memset(led_matrix_buffer, 0, (LED_MATRIX_WIDTH / 2) * LED_MATRIX_HEIGHT); }

static void led_matrix__selectRow(const uint8_t row) {
  (row & 0x1) ? gpio__set(P_addrA) : gpio__reset(P_addrA);
  (row & 0x2) ? gpio__set(P_addrB) : gpio__reset(P_addrB);
  (row & 0x4) ? gpio__set(P_addrC) : gpio__reset(P_addrC);
  (row & 0x8) ? gpio__set(P_addrD) : gpio__reset(P_addrD);
}

static void led_matrix__de_assert_output_enable_latch(void) {
  gpio__set(P_OE);    // Disable LED output during row/plane switchover
  gpio__set(P_LATCH); // Latch data loaded during *prior* interrupt
  // gpio__reset(P_CLOCK); // Start the clock LOW
}

static void led_matrix__assert_output_disable_latch(void) {
  gpio__reset(P_OE);    // Re-enable output
  gpio__reset(P_LATCH); // Latch down
}

static void led_matrix__clock_in_pixel_data(const uint8_t pixel_value) {
  (pixel_value & 0x01) ? gpio__set(P_B1) : gpio__reset(P_B1); // B1
  (pixel_value & 0x02) ? gpio__set(P_G1) : gpio__reset(P_G1); // G1
  (pixel_value & 0x04) ? gpio__set(P_R1) : gpio__reset(P_R1); // R1

  (pixel_value & 0x08) ? gpio__set(P_B2) : gpio__reset(P_B2); // B2
  (pixel_value & 0x10) ? gpio__set(P_G2) : gpio__reset(P_G2); // G2
  (pixel_value & 0x20) ? gpio__set(P_R2) : gpio__reset(P_R2); // R2
  // Generate 1 clock pulse
  gpio__set(P_CLOCK);   // hi
  gpio__reset(P_CLOCK); // lo
}

static void led_matrix__updateDisplay() {
  // fprintf(stderr, "uptime %ld\n", sys_time__get_uptime_ms());

  if (++row_to_display > 15) {
    row_to_display = 0;
  }
  // STEP 2
  led_matrix__de_assert_output_enable_latch();

  // STEP 3 .. Update Row
  led_matrix__selectRow(row_to_display);

  // STEP 1 ..  Clock in the data for the current row one bit at a time
  for (uint32_t i = 0; i < 64; i++) {
    led_matrix__clock_in_pixel_data(led_matrix_buffer[row_to_display][i]);
  }

  // STEP 4 .. Pull the latch and output enable pins low again
  // enabling the output
  led_matrix__assert_output_disable_latch();
}

/*
For each row of pixels, we repeat the following cycle of steps:

    1. Clock in the data for the current row one bit at a time

    2. Pull the latch and output enable pins high. This enables the latch,
    allowing the row data to reach the output driver but it also disables
    the output so that no LEDs are lit while we're switching rows.

    3. Switch rows by driving the appropriate row select lines.

    4. Pull the latch and output enable pins low again, enabling the output and
    closing the latch so we can clock in the next row of data.

*/

static void led_matrix__updateDisplay_isr() {
  led_matrix__updateDisplay();
  hw_timer__set_value(led_matrix__refresh_timer, 0);
  hw_timer__acknowledge_interrupt(led_matrix__refresh_timer, led_matrix__refresh_timer_mr);
}

static void led_matrix__refresh_timer_init() {
  const uint32_t sys_time__us_per_sec = UINT32_C(1) * 1000 * 1000;
  const uint32_t prescalar_for_1us = (clock__get_peripheral_clock_hz() / sys_time__us_per_sec) - 1;
  const uint32_t one_ms_in_us = 1000;

  // Enable the timer with 1uS resolution with an interrupt
  hw_timer__enable(led_matrix__refresh_timer, prescalar_for_1us, led_matrix__updateDisplay_isr);
  hw_timer__enable_match_isr(led_matrix__refresh_timer, led_matrix__refresh_timer_mr, one_ms_in_us);
}

void led_matrix__setupLedMatrixPins(void) {
  P_addrA = gpio__construct_as_output(GPIO__PORT_0, 26);
  P_addrB = gpio__construct_as_output(GPIO__PORT_1, 31);
  P_addrC = gpio__construct_as_output(GPIO__PORT_1, 20);
  P_addrD = gpio__construct_as_output(GPIO__PORT_1, 28);
  P_LATCH = gpio__construct_as_output(GPIO__PORT_2, 2);
  P_OE = gpio__construct_as_output(GPIO__PORT_2, 5);
  P_CLOCK = gpio__construct_as_output(GPIO__PORT_2, 0);
  P_R1 = gpio__construct_as_output(GPIO__PORT_1, 14);
  P_G1 = gpio__construct_as_output(GPIO__PORT_4, 29);
  P_B1 = gpio__construct_as_output(GPIO__PORT_0, 7);
  P_R2 = gpio__construct_as_output(GPIO__PORT_4, 28);
  P_G2 = gpio__construct_as_output(GPIO__PORT_0, 6);
  P_B2 = gpio__construct_as_output(GPIO__PORT_0, 8);
}

void led_matrix__setDefaultPinStates(void) {

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

  led_matrix__refresh_timer_init();
}
