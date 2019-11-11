/*
To run the test 'cd' into test Framework directory and run the command as below

$ cd ..\UT_FrmWrk_Book\
$ rake unit single_file=../road_max_fury/test/test_led_matrix.c

Add src and test files in 'gcc.yml' (in the UnitTest Frw folder ) as follows

      # Source code under test
      - './code/'
      - *unit_tests_path
      - '../road_max_fury/l3_drivers/'
      - '../road_max_fury/l3_drivers/sources/'
      - '../road_max_fury/l4_io/'
      - '../road_max_fury/l4_io/sources/'
      - '../road_max_fury/l5_application/'
      - '../road_max_fury/test/'

Adjust the formating as in 'gcc.yml'. The 3rd path is for the CUT and
the 4th path is for the tests.
*/

// C:\UT_FrmWrk_Book\install_unity_cmock\vendor\unity\src\unity.h
#include "unity.h" // Single Unity Test Framework include
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "led_matrix.h"

static uint8_t led_matrix_buffer[16][64];

void setUp(void)
{
    memset(led_matrix_buffer, 0xff, (16 * 64));
    led_matrix__init(led_matrix_buffer);
}

/*
Notes on 2D Array. 
Check how to pass a 2D array to a funtion 
Check how to initialize and access 2D array

https://www.geeksforgeeks.org/pass-2d-array-parameter-c/
https://beginnersbook.com/2014/01/2d-arrays-in-c-example/

Copy multidimensional array
https://stackoverflow.com/questions/2225850/c-c-how-to-copy-a-multidimensional-char-array-without-nested-loops
*/

void test_led_matrix__intialize_buffer(void)
{
    uint8_t actual_led_matrix_buffer[16][64];
    memset(actual_led_matrix_buffer, 0xff, (16 * 64));    

    led_matrix__init(actual_led_matrix_buffer);

    uint8_t expected_led_matrix_buffer[16][64];
    memset(expected_led_matrix_buffer, 0, 16 * 64);

    TEST_ASSERT_EQUAL_INT8_ARRAY(expected_led_matrix_buffer[0], actual_led_matrix_buffer[0], (64));
    TEST_ASSERT_EQUAL_INT8_ARRAY(expected_led_matrix_buffer[12], actual_led_matrix_buffer[12], (64));
    //TEST_ASSERT_EQUAL_INT8_ARRAY(expected_led_matrix_buffer[6][34], actual_led_matrix_buffer[6][34], 1);
}

void test_led_matrix_draw_Pixel_packing_the_buffer_at_0_0(void)
{
    drawPixel(0, 0, 7);
    TEST_ASSERT_EQUAL_HEX8(0x07, led_matrix_buffer[0][0]);
}

void test_led_matrix_draw_Pixel_packing_the_buffer_at_16_0(void)
{
    drawPixel(16, 0, 7);
    TEST_ASSERT_EQUAL_HEX8(0X38, led_matrix_buffer[0][0]);
}

void test_led_matrix_draw_Pixel_packing_the_buffer_at_5_4(void)
{
    drawPixel(5, 4, 3);
    TEST_ASSERT_EQUAL_HEX8(0X03, led_matrix_buffer[5][4]);
}

void test_led_matrix_draw_Pixel_packing_the_buffer_at_24_56(void)
{
    drawPixel(24, 56, 5);
    TEST_ASSERT_EQUAL_HEX8(0X28, led_matrix_buffer[8][56]);
}

void test_led_matrix_draw_Pixel_fails_at_out_of_bound_pixel(void)
{
    bool isSuccess = led_matrix__drawPixel(32, 64, 5);
    TEST_ASSERT_FALSE(isSuccess);
}

void test_glow_all_pixels(void)
{
    led_matrix__drawAllPixels(7);

    uint8_t expected_led_matrix_buffer[16][64];
    memset(expected_led_matrix_buffer, 0x3f, (16 * 64));

    for (int i = 0; i < LED_MATRIX_ROWS; i++)
    {
        TEST_ASSERT_EQUAL_INT8_ARRAY(expected_led_matrix_buffer[i], led_matrix_buffer[i], (64));
    }
}