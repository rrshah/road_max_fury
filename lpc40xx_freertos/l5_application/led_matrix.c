
#include <stdio.h>
#include <string.h>

#include "delay.h"
#include "gpio.h"
#include "led_matrix.h"

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

// static uint8_t led_matrix_buffer[16][64] = {0};
static uint8_t *led_matrix_buffer[LED_MATRIX_COLUMNS];
// static uint8_t row_to_display = 15;

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

  // TODO rit_enable(refresh, 1);
  // TODO refreshTimer.begin(refreshISR, 200, uSec);
}

void led_matrix__init(uint8_t matrix_buffer[][LED_MATRIX_WIDTH]) {

  for (int i = 0; i < LED_MATRIX_ROWS; i++) {
    led_matrix_buffer[i] = matrix_buffer[i];
    memset(led_matrix_buffer[i], 0, LED_MATRIX_COLUMNS);
  }
}

bool led_matrix__drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((y < 0) || (y >= 64) || (x < 0) || (x >= 32)) {
    return false;
  }

  if (x < 16) {
    led_matrix_buffer[x][y] |= color;
  } else {
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
void led_matrix__turnOffAllPixels() {
  for (int i = 0; i < LED_MATRIX_ROWS; i++) {
    memset(led_matrix_buffer[i], 0, LED_MATRIX_COLUMNS);
  }
}

void led_matrix__selectRow(uint8_t row) {
  row %= 16;
  (row & 0x1) ? gpio__set(P_addrA) : gpio__reset(P_addrA);
  (row & 0x2) ? gpio__set(P_addrB) : gpio__reset(P_addrB);
  (row & 0x4) ? gpio__set(P_addrC) : gpio__reset(P_addrC);
  (row & 0x8) ? gpio__set(P_addrD) : gpio__reset(P_addrD);
}

void led_matrix__de_assert_output_enable_latch(void) {
  gpio__set(P_OE);    // Disable LED output during row/plane switchover
  gpio__set(P_LATCH); // Latch data loaded during *prior* interrupt
  // gpio__reset(P_CLOCK); // Start the clock LOW
}

void led_matrix__assert_output_disable_latch(void) {
  gpio__reset(P_OE);    // Re-enable output
  gpio__reset(P_LATCH); // Latch down
}

void led_matrix__clock_in_pixel_data(uint8_t pixel_value) {
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

void led_matrix__updateRowOfDisplay(uint8_t row_to_display) {
  // STEP 2
  led_matrix__de_assert_output_enable_latch();

  // STEP 3 .. Update Row
  led_matrix__selectRow(row_to_display);

  // printf("Row: %d:\n", row_to_display);
  // STEP 1 ..  Clock in the data for the current row one bit at a time
  for (uint32_t i = 0; i < 64; i++) {
    led_matrix__clock_in_pixel_data(led_matrix_buffer[row_to_display][i]);
    // printf(" Col_%d: 0x%02X\t", i, led_matrix_buffer[row_to_display][i]);
  }
  // printf("Next.\n");

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
void led_matrix__updateDisplay(void) {
  uint8_t current_row = 0;
  for (; current_row < 16; current_row++) {
    led_matrix__updateRowOfDisplay(current_row);
    delay__ms(100);
  }
}