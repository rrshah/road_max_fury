
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "display_tasks.h"
#include "graphics.h"
#include "led_matrix.h"
#include "object.h"

#define BORDER_HEIGHT (52)
#define BORDER_WIDTH (3)
#define CAR_WIDTH (4)
#define CAR_HEIGHT (5)

extern const uint8_t car;
bitmap_object player_car = {.x = 15, .y = 10, .height = 5, .width = 4, .color = CYAN, .image = &car};

void move_car_left() {
  if (player_car.x > BORDER_WIDTH) {
    player_car.x -= 1;
  }
}

void move_car_right() {
  if (player_car.x < (LED_MATRIX_WIDTH - BORDER_WIDTH - CAR_WIDTH)) {
    player_car.x += 1;
  }
}

static void move() {
  // move_obstacles();
}

static void draw_borders() {
  fillRect(0, 0, BORDER_WIDTH, BORDER_HEIGHT, GREEN);
  fillRect(29, 0, BORDER_WIDTH, BORDER_HEIGHT, GREEN);
}

// static void draw_score();
static void draw_player_car() { object__draw(player_car); }
// static void draw_obstacles();

static void draw() {
  draw_borders();
  draw_player_car();
}
void display_task(void *params) {
  while (true) {
    led_matrix__turnOffAllPixels();
    move();
    draw();
    vTaskDelay(200);
  }
}

void test_led_matrix_task(void *params) {

  led_matrix__turnOnAllPixels(BLUE);
  led_matrix__drawPixel(0, 0, BLUE);
  led_matrix__drawPixel(5, 1, BLUE);

  led_matrix__drawPixel(21, 21, BLUE);
  led_matrix__drawPixel(25, 43, BLUE);
  led_matrix__drawPixel(20, 20, BLUE);

  led_matrix__drawPixel(31, 0, BLUE);
  led_matrix__drawPixel(31, 31, BLUE);
  led_matrix__drawPixel(31, 63, BLUE);

  // led_matrix__updateDisplay();

  while (true) {

    led_matrix__turnOnAllPixels(BLUE);
    printf("LED ON..\n");
    // led_matrix__updateDisplay();

    vTaskDelay(1);

    led_matrix__turnOffAllPixels();
    printf("LED OFF..\n");
    // led_matrix__updateDisplay();

    vTaskDelay(100);
  }
}

// smiley
static const uint8_t smiley[] = {0x3c, 0x42, 0x99, 0xa5, 0x81, 0xa5, 0x81, 0x42, 0x3c};

void test_graphics_task(void *params) {

  // drawLine(0, 0, 31, 63, red_4);
  // drawRect(0, 0, 10, 20, white_7);
  // fillRect(0, 0, 20, 10, YELLOW);
  // drawChar(5, 5, 'R', red_4, red_4, 1);
  drawBitmap(15, 31, smiley, 8, 9, RED);

  while (true) {
    // draw_car(player_car);
    vTaskDelay(1);
  }
}

// void led_matrix_task(void *params) {

//   // led_matrix__drawAllPixels(white_7);
//   // led_matrix__turnOnAllPixels(red_4);
//   // led_matrix__drawPixel(0, 0, white_7);
//   // led_matrix__drawPixel(5, 1, red_4);

//   // led_matrix__drawPixel(21, 21, yellow_6);
//   // led_matrix__drawPixel(25, 43, magenta_5);
//   // led_matrix__drawPixel(20, 20, red_4);

//   // led_matrix__drawPixel(31, 0, white_7);
//   // led_matrix__drawPixel(31, 31, blue_1);
//   // led_matrix__drawPixel(31, 63, green_2);

//   // drawLine(0, 0, 31, 63, red_4);
//   // drawRect(0, 0, 10, 20, white_7);
//   // fillRect(0, 0, 20, 10, green_2);
//   // drawPixel(19, 20, yellow_6);
//   // led_matrix__updateDisplay();

//   while (true) {
//     led_matrix__turnOffAllPixels();
//     // draw_car(player_car);
//     // led_matrix__turnOnAllPixels(red_4);
//     // printf("LED ON..\n");
//     // led_matrix__updateDisplay();
//     // vTaskDelay(1);
//     // led_matrix__turnOffAllPixels();
//     // printf("LED OFF..\n");
//     // led_matrix__updateDisplay();
//     // drawPixel(pixel_x, pixel_y, red_4);
//     vTaskDelay(100);
//   }
// }