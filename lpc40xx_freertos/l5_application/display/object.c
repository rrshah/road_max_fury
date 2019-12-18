
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "graphics.h"
#include "object.h"
#include "semphr.h"
#include "task.h"

#define BORDER_HEIGHT (52)
#define BORDER_WIDTH (2)
#define CAR_WIDTH (4)
#define CAR_HEIGHT (5)
#define CAR_HEIGHT_WITH_PADDING (6)
#define CAR_WIDTH_WITH_PADDING (6)
#define NUM_OF_OBSTACLES (10)
#define LEVEL_1_SCORE (30)
#define LEVEL_2_SCORE (100)
#define LEVEL_3_SCORE (200)

extern SemaphoreHandle_t countdown;
extern SemaphoreHandle_t play;
extern SemaphoreHandle_t level;

const uint8_t car[] = {0x30, 0x78, 0x30, 0x78, 0x30, 0};

const uint8_t letter_R[] = {0x84, 0x88, 0x90, 0xA0, 0xC0,
                            0xFC, 0x84, 0x84, 0x84, 0xFC};
const uint8_t letter_O[] = {0x78, 0x84, 0x84, 0x84, 0x84,
                            0x84, 0x84, 0x84, 0x84, 0x78};
const uint8_t letter_A[] = {0x84, 0x84, 0x84, 0x84, 0x84,
                            0xFC, 0x84, 0x84, 0x84, 0x78};
const uint8_t letter_D[] = {0xF0, 0x88, 0x84, 0x84, 0x84,
                            0x84, 0x84, 0x84, 0x88, 0xF0};
const uint8_t letter_M[] = {0x84, 0x84, 0x84, 0x84, 0x84,
                            0xB4, 0xB4, 0xB4, 0xCC, 0x84};
const uint8_t letter_X[] = {0x84, 0x84, 0x84, 0x48, 0x30,
                            0x30, 0x48, 0x84, 0x84, 0x84};
const uint8_t letter_F[] = {0x80, 0x80, 0x80, 0x80, 0x80,
                            0xFC, 0x80, 0x80, 0x80, 0xFC};
const uint8_t letter_U[] = {0x78, 0x84, 0x84, 0x84, 0x84,
                            0x84, 0x84, 0x84, 0x84, 0x84};
const uint8_t letter_Y[] = {0x30, 0x30, 0x30, 0x30, 0x30,
                            0x30, 0x48, 0x84, 0x84, 0x84};

const uint8_t letter_smallP[] = {0x80, 0x80, 0xF8, 0x88, 0xF8};
const uint8_t letter_smallL[] = {0xF8, 0x80, 0x80, 0x80, 0x80};
const uint8_t letter_smallA[] = {0x88, 0x88, 0xF8, 0x88, 0X70};
const uint8_t letter_smallY[] = {0x20, 0x20, 0x20, 0x50, 0x88};

const uint8_t score_letter_S[] = {0xE0, 0x20, 0xE0, 0x80, 0xE0};
const uint8_t score_letter_C[] = {0xE0, 0x80, 0x80, 0x80, 0xE0};
const uint8_t score_letter_O[] = {0xE0, 0xA0, 0xA0, 0xA0, 0xE0};
const uint8_t score_letter_R[] = {0xA0, 0xC0, 0xE0, 0xA0, 0xE0};
const uint8_t score_letter_E[] = {0xE0, 0x80, 0xC0, 0x80, 0xE0};

const uint8_t level_letter_L[] = {0xE0, 0x80, 0x80, 0x80, 0x80};
const uint8_t level_letter_V[] = {0x40, 0xA0, 0xA0, 0xA0, 0xA0};

const uint8_t score_colon[] = {0x00, 0x80, 0x00, 0x80, 0x00};

const uint8_t number[][5] = {
    {0xE0, 0xA0, 0xA0, 0xA0, 0xE0}, {0xE0, 0x40, 0x40, 0xC0, 0x40},
    {0xE0, 0x80, 0xE0, 0x20, 0xE0}, {0xE0, 0x20, 0x60, 0x20, 0xE0},
    {0x40, 0xE0, 0xC0, 0x80, 0x80}, {0xE0, 0x20, 0xE0, 0x80, 0xE0},
    {0xE0, 0Xa0, 0xE0, 0x80, 0XE0}, {0x20, 0x20, 0x20, 0x20, 0xE0},
    {0xE0, 0xA0, 0xE0, 0xA0, 0xE0}, {0xE0, 0x20, 0xE0, 0xA0, 0xE0}};

const uint8_t play_button[] = {0xC0, 0xE0, 0xF1, 0xE0, 0xC0};

static uint16_t score = 0;
uint8_t game_screen_state = START_SCREEN;

static uint8_t current_level = 1;
static uint8_t num_of_on_screen_obstacles = 0;
static bitmap_object player_car;
static bitmap_object countdown_car = {.color = CYAN,
                                      .x = (LED_MATRIX_WIDTH / 2) -
                                           (CAR_WIDTH_WITH_PADDING / 2),
                                      .y = 10,
                                      .image = car,
                                      .height = CAR_HEIGHT_WITH_PADDING,
                                      .width = CAR_WIDTH_WITH_PADDING,
                                      .isAlive = true};

static bitmap_object car_obstacle[NUM_OF_OBSTACLES];
static level_t levels[] = {{
                               .level_obstacle_mod = 1,
                               .num_of_obstacles = 3,
                               .score_per_car = 3,
                           },
                           {
                               .level_obstacle_mod = 3,
                               .num_of_obstacles = 3,
                               .score_per_car = 5,
                           },
                           {
                               .level_obstacle_mod = 4,
                               .num_of_obstacles = 4,
                               .score_per_car = 10,
                           }};

bitmap_object obstacle_types[] = {
    {.color = MAGENTA,
     .image = car,
     .movement_type = DOWN,
     .height = CAR_HEIGHT_WITH_PADDING,
     .width = CAR_WIDTH_WITH_PADDING,
     .speed = 30,
     .counter = 0,
     .isAlive = true},
    {.color = BLUE,
     .image = car,
     .movement_type = DOWN,
     .height = CAR_HEIGHT_WITH_PADDING,
     .width = CAR_WIDTH_WITH_PADDING,
     .speed = 10,
     .counter = 0,
     .isAlive = true},
    {.color = RED,
     .image = car,
     .movement_type = DOWN,
     .height = CAR_HEIGHT_WITH_PADDING,
     .width = CAR_WIDTH_WITH_PADDING,
     .speed = 1,
     .counter = 0,
     .isAlive = true},
    {.color = GREEN,
     .image = car,
     .movement_type = DOWN_AND_LEFT_RIGHT,
     .height = CAR_HEIGHT_WITH_PADDING,
     .width = CAR_WIDTH_WITH_PADDING,
     .speed = 30,
     .counter = 0,
     .isAlive = true},
};

void object__draw(bitmap_object object) {
  drawBitmap(object.x, object.y, object.image, object.width, object.height,
             object.color);
}

static void get_score(uint8_t *hundred, uint8_t *ten, uint8_t *unit) {
  uint16_t temp_score = score;
  *unit = temp_score % 10;
  temp_score /= 10;
  *ten = temp_score % 10;
  temp_score /= 10;
  *hundred = temp_score % 10;
}

void draw_score() {
  uint8_t hundred, ten, unit;

  drawBitmap(1, 59, score_letter_S, 3, 5, BLUE);
  drawBitmap(5, 59, score_letter_C, 3, 5, BLUE);
  drawBitmap(9, 59, score_letter_O, 3, 5, BLUE);
  drawBitmap(13, 59, score_letter_R, 3, 5, BLUE);
  drawBitmap(17, 59, score_letter_E, 3, 5, BLUE);

  get_score(&hundred, &ten, &unit);
  drawBitmap(21, 59, number[hundred], 3, 5, CYAN);
  drawBitmap(25, 59, number[ten], 3, 5, CYAN);
  drawBitmap(29, 59, number[unit], 3, 5, CYAN);
}

void draw_level() {
  drawBitmap(1, 53, level_letter_L, 3, 5, GREEN);
  drawBitmap(5, 53, score_letter_E, 3, 5, GREEN);
  drawBitmap(9, 53, level_letter_V, 3, 5, GREEN);
  drawBitmap(13, 53, score_letter_E, 3, 5, GREEN);
  drawBitmap(17, 53, level_letter_L, 3, 5, GREEN);

  drawBitmap(21, 53, number[current_level], 3, 5, GREEN);
}

void object__init_player_car(void) {
  player_car.x = (LED_MATRIX_WIDTH / 2) - (CAR_WIDTH_WITH_PADDING / 2);
  player_car.y = 10;
  player_car.height = CAR_HEIGHT_WITH_PADDING;
  player_car.width = CAR_WIDTH_WITH_PADDING;
  player_car.color = CYAN;
  player_car.image = car;
}

static void move_obstacles(bitmap_object *obstacle) {
  obstacle->counter++;
  if (obstacle->counter < obstacle->speed) {
    return;
  }

  switch (obstacle->movement_type) {
  case DOWN:
    obstacle->y = obstacle->y - 1;
    break;
  case DOWN_AND_LEFT_RIGHT:
    obstacle->y = obstacle->y - 1;
    if (obstacle->direction == RIGHT) {
      if (obstacle->x <
          (LED_MATRIX_WIDTH - BORDER_WIDTH - CAR_WIDTH_WITH_PADDING)) {
        obstacle->x = obstacle->x + 1;
      } else {
        obstacle->direction = LEFT;
      }
    } else {
      if (obstacle->x > BORDER_WIDTH) {
        obstacle->x = obstacle->x - 1;
      } else {
        obstacle->direction = RIGHT;
      }
    }
  }

  if (obstacle->y == (-1) * CAR_HEIGHT_WITH_PADDING) {
    obstacle->isAlive = false;
    score += levels[current_level - 1].score_per_car;
    if (score > LEVEL_1_SCORE && score < LEVEL_2_SCORE && current_level != 2) {
      xSemaphoreGive(level);
      current_level = 2;
    } else if (score > LEVEL_2_SCORE && current_level != 3) {
      xSemaphoreGive(level);
      current_level = 3;
    }
    num_of_on_screen_obstacles--;
  }
}

void move() {
  for (uint8_t i = 0; i < NUM_OF_OBSTACLES; i++) {
    if (car_obstacle[i].isAlive) {
      move_obstacles(&car_obstacle[i]);
    }
  }
}

static void draw_borders() {
  fillRect(0, 0, BORDER_WIDTH, BORDER_HEIGHT, YELLOW);
  fillRect(30, 0, BORDER_WIDTH, BORDER_HEIGHT, YELLOW);
  drawLine(0, BORDER_HEIGHT, 31, BORDER_HEIGHT, WHITE);
}

static void draw_player_car() { object__draw(player_car); }
static void draw_obstacles() {
  for (uint8_t i = 0; i < NUM_OF_OBSTACLES; i++) {
    if (car_obstacle[i].isAlive) {
      object__draw(car_obstacle[i]);
    }
  }
}

void move_car_left() {
  if (game_screen_state != GAME_SCREEN) {
    return;
  }
  if (player_car.x > BORDER_WIDTH - 1) {
    player_car.x -= 1;
  }
}

void move_car_right() {
  if (game_screen_state != GAME_SCREEN) {
    return;
  }
  if (player_car.x < (LED_MATRIX_WIDTH - BORDER_WIDTH - CAR_WIDTH - 1)) {
    player_car.x += 1;
  }
}

void draw_countdown_screen() {
  uint8_t i = 3;
  while (i > 0) {
    xSemaphoreGive(countdown);
    draw_player_car();
    draw_borders();
    drawBitmap((LED_MATRIX_WIDTH / 2) - (CAR_WIDTH_WITH_PADDING / 2) + 2,
               BORDER_HEIGHT - 15, number[i], 3, 5, GREEN);
    draw_score();
    draw_level();
    vTaskDelay(500);

    led_matrix__turnOffAllPixels();
    draw_borders();
    draw_score();
    draw_level();
    vTaskDelay(500);
    i--;
  }
  xSemaphoreGive(play);
  vTaskDelay(500);
}

void draw() {
  draw_player_car();
  draw_borders();
  draw_obstacles();
  draw_score();
  draw_level();
}

static void generate_obstacle(bitmap_object *obstacle) {
  uint8_t x, index;

  obstacle->y = BORDER_HEIGHT - CAR_HEIGHT_WITH_PADDING;

  x = rand() % LED_MATRIX_WIDTH;
  if (x < BORDER_WIDTH) {
    x = BORDER_WIDTH;
  } else if (x > (LED_MATRIX_WIDTH - BORDER_WIDTH - CAR_WIDTH_WITH_PADDING)) {
    x = LED_MATRIX_WIDTH - BORDER_WIDTH - CAR_WIDTH_WITH_PADDING;
  }
  obstacle->x = x;

  index = rand() % levels[current_level - 1].level_obstacle_mod;

  obstacle->image = obstacle_types[index].image;
  obstacle->isAlive = true;
  obstacle->color = obstacle_types[index].color;
  obstacle->movement_type = obstacle_types[index].movement_type;
  obstacle->height = obstacle_types[index].height;
  obstacle->width = obstacle_types[index].width;
  obstacle->speed = obstacle_types[index].speed;
  obstacle->counter = 0;
  obstacle->direction = index % 2;
}

void generate_random_obstacles() {
  static int counter = 0;
  counter++;
  if (counter < 20)
    return;
  if (num_of_on_screen_obstacles < levels[current_level - 1].num_of_obstacles) {
    for (uint8_t i = 0; i < NUM_OF_OBSTACLES; i++) {
      if (!car_obstacle[i].isAlive) {
        generate_obstacle(&car_obstacle[i]);
        break;
      }
    }

    num_of_on_screen_obstacles++;
  }
  counter = 0;
}

void draw_start_screen() {
  drawBitmap(1, 48, letter_R, 6, 10, RED);
  drawBitmap(9, 48, letter_O, 6, 10, RED);
  drawBitmap(17, 48, letter_A, 6, 10, RED);
  drawBitmap(25, 48, letter_D, 6, 10, RED);
  drawBitmap(5, 35, letter_M, 6, 10, RED);
  drawBitmap(13, 35, letter_A, 6, 10, RED);
  drawBitmap(21, 35, letter_X, 6, 10, RED);
  drawBitmap(1, 22, letter_F, 6, 10, RED);
  drawBitmap(9, 22, letter_U, 6, 10, RED);
  drawBitmap(17, 22, letter_R, 6, 10, RED);
  drawBitmap(25, 22, letter_Y, 6, 10, RED);

  drawBitmap(2, 13, letter_smallP, 5, 5, BLUE);
  drawBitmap(8, 13, letter_smallL, 5, 5, BLUE);
  drawBitmap(14, 13, letter_smallA, 5, 5, BLUE);
  drawBitmap(20, 13, letter_smallY, 5, 5, BLUE);

  drawBitmap(26, 13, play_button, 4, 5, GREEN);
}

static bool check_collision(const bitmap_object obstacle) {
  bool check_y = false;
  if (((obstacle.x + 1) >= player_car.x + 1) &&
      ((obstacle.x + 1) <= (player_car.x + CAR_WIDTH))) {
    check_y = true;
  } else if (((obstacle.x + CAR_WIDTH) >= (player_car.x + 1)) &&
             ((obstacle.x + CAR_WIDTH) <= (player_car.x + CAR_WIDTH))) {
    check_y = true;
  }

  if (!check_y) {
    return false;
  }

  if (((obstacle.y >= player_car.y) &&
       (obstacle.y <= (player_car.y + CAR_HEIGHT - 1))) ||
      (((obstacle.y + CAR_HEIGHT - 1) >= player_car.y) &&
       ((obstacle.y + CAR_HEIGHT - 1) <= (player_car.y + CAR_HEIGHT - 1)))) {
    return true;
  }
  return false;
}

void collision_detector() {
  for (uint8_t i = 0; i < NUM_OF_OBSTACLES; i++) {
    if (car_obstacle[i].isAlive) {
      if (check_collision(car_obstacle[i])) {
        game_screen_state = CAR_CRASH;
      }
    }
  }
}

const uint8_t crash_screen_B[] = {0xF0, 0X88, 0xF0, 0x88, 0xF0};
const uint8_t crash_screen_O[] = {0x70, 0x88, 0x88, 0x88, 0x70};
const uint8_t crash_screen_M[] = {0x88, 0x88, 0xA8, 0xD8, 0x88};
const uint8_t crash_screen_exclaimation[] = {0xC0, 0x00, 0xC0, 0xC0, 0xC0};

void draw_crash_screen() {
  drawBitmap(3, 22, crash_screen_B, 5, 5, RED);
  drawBitmap(9, 22, crash_screen_O, 5, 5, RED);
  drawBitmap(15, 22, crash_screen_O, 5, 5, RED);
  drawBitmap(21, 22, crash_screen_M, 5, 5, RED);
  drawBitmap(27, 22, crash_screen_exclaimation, 2, 5, RED);
  draw_borders();
}

const uint8_t game_over_G[] = {0xF8, 0x88, 0xB8, 0x80, 0xF8};
const uint8_t game_over_A[] = {0x88, 0x88, 0xF8, 0x50, 0x20};
const uint8_t game_over_M[] = {0x88, 0x88, 0xA8, 0xD8, 0x88};
const uint8_t game_over_E[] = {0xF8, 0x80, 0xF8, 0x80, 0xF8};
const uint8_t game_over_O[] = {0x70, 0x88, 0x88, 0x88, 0x70};
const uint8_t game_over_V[] = {0x20, 0x50, 0x88, 0x88, 0x88};
const uint8_t game_over_R[] = {0x98, 0xA0, 0xF8, 0x88, 0xF8};

static const uint8_t smiley[] = {0x3c, 0x42, 0x99, 0xa5, 0x81,
                                 0xa5, 0x81, 0x42, 0x3c};

void draw_gameover_screen() {
  uint8_t hundred, ten, unit;
  drawRect(0, 0, LED_MATRIX_WIDTH, LED_MATRIX_HEIGHT, WHITE);
  drawBitmap(4, 52, game_over_G, 5, 5, RED);
  drawBitmap(10, 52, game_over_A, 5, 5, RED);
  drawBitmap(16, 52, game_over_M, 5, 5, RED);
  drawBitmap(22, 52, game_over_E, 5, 5, RED);
  drawBitmap(4, 44, game_over_O, 5, 5, RED);
  drawBitmap(10, 44, game_over_V, 5, 5, RED);
  drawBitmap(16, 44, game_over_E, 5, 5, RED);
  drawBitmap(22, 44, game_over_R, 5, 5, RED);

  drawBitmap(6, 30, score_letter_S, 3, 5, BLUE);
  drawBitmap(10, 30, score_letter_C, 3, 5, BLUE);
  drawBitmap(14, 30, score_letter_O, 3, 5, BLUE);
  drawBitmap(18, 30, score_letter_R, 3, 5, BLUE);
  drawBitmap(22, 30, score_letter_E, 3, 5, BLUE);

  get_score(&hundred, &ten, &unit);
  drawBitmap(10, 24, number[hundred], 3, 5, CYAN);
  drawBitmap(14, 24, number[ten], 3, 5, CYAN);
  drawBitmap(18, 24, number[unit], 3, 5, CYAN);

  drawBitmap(12, 8, smiley, 8, 9, YELLOW);
}
