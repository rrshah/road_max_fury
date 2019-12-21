
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "draw.h"
#include "graphics.h"
#include "object.h"

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

const uint8_t number[][5] = {{0xE0, 0xA0, 0xA0, 0xA0, 0xE0}, {0xE0, 0x40, 0x40, 0xC0, 0x40},
                             {0xE0, 0x80, 0xE0, 0x20, 0xE0}, {0xE0, 0x20, 0x60, 0x20, 0xE0},
                             {0x40, 0xE0, 0xC0, 0x80, 0x80}, {0xE0, 0x20, 0xE0, 0x80, 0xE0},
                             {0xE0, 0Xa0, 0xE0, 0x80, 0XE0}, {0x20, 0x20, 0x20, 0x20, 0xE0},
                             {0xE0, 0xA0, 0xE0, 0xA0, 0xE0}, {0xE0, 0x20, 0xE0, 0xA0, 0xE0}};

static uint16_t score = 0;
uint8_t game_screen_state = START_SCREEN;

uint8_t current_level = 1;
static uint8_t num_of_on_screen_obstacles = 0;
static bitmap_object player_car;
bitmap_object start_screen_car = {.color = CYAN,
                                  .x = (LED_MATRIX_WIDTH / 2) - (CAR_WIDTH_WITH_PADDING / 2),
                                  .y = 5,
                                  .image = car,
                                  .height = CAR_HEIGHT_WITH_PADDING,
                                  .width = CAR_WIDTH_WITH_PADDING,
                                  .direction = RIGHT,
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

static void object__init_player_car(void) {
  player_car.x = (LED_MATRIX_WIDTH / 2) - (CAR_WIDTH_WITH_PADDING / 2);
  player_car.y = 10;
  player_car.height = CAR_HEIGHT_WITH_PADDING;
  player_car.width = CAR_WIDTH_WITH_PADDING;
  player_car.color = CYAN;
  player_car.image = car;
}

void game_init() {
  for (uint8_t i = 0; i < NUM_OF_OBSTACLES; i++) {
    car_obstacle[i].isAlive = false;
  }
  num_of_on_screen_obstacles = 0;
  current_level = 1;
  score = 0;
  object__init_player_car();
}

void object__draw(bitmap_object object) {
  drawBitmap(object.x, object.y, object.image, object.width, object.height, object.color);
}

void get_score(uint8_t *hundred, uint8_t *ten, uint8_t *unit) {
  uint16_t temp_score = score;
  *unit = temp_score % 10;
  temp_score /= 10;
  *ten = temp_score % 10;
  temp_score /= 10;
  *hundred = temp_score % 10;
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
      if (obstacle->x < (LED_MATRIX_WIDTH - BORDER_WIDTH - CAR_WIDTH_WITH_PADDING)) {
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
    drawBitmap((LED_MATRIX_WIDTH / 2) - (CAR_WIDTH_WITH_PADDING / 2) + 2, BORDER_HEIGHT - 15, number[i], 3, 5, GREEN);
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
  // draw_and_move_borders();
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

void move_start_screen_car() {

  if (start_screen_car.direction == RIGHT) {
    if (start_screen_car.x < (LED_MATRIX_WIDTH - BORDER_WIDTH - CAR_WIDTH_WITH_PADDING)) {
      start_screen_car.x = start_screen_car.x + 1;
    } else {
      start_screen_car.direction = LEFT;
    }
  } else {
    if (start_screen_car.x > BORDER_WIDTH) {
      start_screen_car.x = start_screen_car.x - 1;
    } else {
      start_screen_car.direction = RIGHT;
    }
  }
}

// void draw_start_screen() {
//   drawRect(0, 0, LED_MATRIX_WIDTH, LED_MATRIX_HEIGHT, WHITE);
//   drawBitmap(1, 48, letter_R, 6, 10, RED);
//   drawBitmap(9, 48, letter_O, 6, 10, RED);
//   drawBitmap(17, 48, letter_A, 6, 10, RED);
//   drawBitmap(25, 48, letter_D, 6, 10, RED);
//   drawBitmap(5, 35, letter_M, 6, 10, RED);
//   drawBitmap(13, 35, letter_A, 6, 10, RED);
//   drawBitmap(21, 35, letter_X, 6, 10, RED);
//   drawBitmap(1, 22, letter_F, 6, 10, RED);
//   drawBitmap(9, 22, letter_U, 6, 10, RED);
//   drawBitmap(17, 22, letter_R, 6, 10, RED);
//   drawBitmap(25, 22, letter_Y, 6, 10, RED);

//   drawBitmap(2, 13, letter_smallP, 5, 5, BLUE);
//   drawBitmap(8, 13, letter_smallL, 5, 5, BLUE);
//   drawBitmap(14, 13, letter_smallA, 5, 5, BLUE);
//   drawBitmap(20, 13, letter_smallY, 5, 5, BLUE);

//   drawBitmap(26, 13, play_button, 4, 5, GREEN);
//   object__draw(start_screen_car);
//   move_start_screen_car();
// }

static bool check_collision(const bitmap_object obstacle) {
  bool check_y = false;
  if (((obstacle.x + 1) >= player_car.x + 1) && ((obstacle.x + 1) <= (player_car.x + CAR_WIDTH))) {
    check_y = true;
  } else if (((obstacle.x + CAR_WIDTH) >= (player_car.x + 1)) &&
             ((obstacle.x + CAR_WIDTH) <= (player_car.x + CAR_WIDTH))) {
    check_y = true;
  }

  if (!check_y) {
    return false;
  }

  if (((obstacle.y >= player_car.y) && (obstacle.y <= (player_car.y + CAR_HEIGHT - 1))) ||
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
