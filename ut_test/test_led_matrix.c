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
#include "Mockgpio.h"
#include "Mockdelay.h"

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
    led_matrix__drawPixel(0, 0, white_7);
    TEST_ASSERT_EQUAL_HEX8(0b00000111, led_matrix_buffer[0][0]);
}

void test_led_matrix_draw_Pixel_packing_the_buffer_at_16_0(void)
{
    led_matrix__drawPixel(16, 0, red_4);
    TEST_ASSERT_EQUAL_HEX8(0b00100000, led_matrix_buffer[0][0]);
}

void test_led_matrix_draw_Pixel_packing_the_buffer_at_5_4(void)
{
    drawPixel(5, 4, cyan_3);
    TEST_ASSERT_EQUAL_HEX8(0X03, led_matrix_buffer[5][4]);
}

void test_led_matrix_draw_Pixel_packing_the_buffer_at_24_56(void)
{
    drawPixel(24, 56, magenta_5);
    TEST_ASSERT_EQUAL_HEX8(0b00101000, led_matrix_buffer[8][56]);
}
void test_led_matrix_draw_Pixel_packing_the_buffer_at_15_63(void)
{
    drawPixel(15, 63, yellow_6);
    TEST_ASSERT_EQUAL_HEX8(0b00000110, led_matrix_buffer[15][63]);
}
void test_led_matrix_draw_Pixel_packing_the_buffer_at_31_63(void)
{
    drawPixel(31, 63, blue_1);
    TEST_ASSERT_EQUAL_HEX8(0b00001000, led_matrix_buffer[15][63]);
}
void test_led_matrix_draw_Pixel_packing_the_buffer_at_15_63_and_31_63(void)
{
    drawPixel(15, 63, yellow_6);
    drawPixel(31, 63, blue_1);
    TEST_ASSERT_EQUAL_HEX8(0b0001110, led_matrix_buffer[15][63]);
}

void test_led_matrix_draw_Pixel_fails_at_out_of_bound_pixel(void)
{
    bool isSuccess = led_matrix__drawPixel(32, 64, 5);
    TEST_ASSERT_FALSE(isSuccess);
}

void test_glow_all_pixels(void)
{
    led_matrix__turnOnAllPixels(7);

    uint8_t expected_led_matrix_buffer[16][64];
    memset(expected_led_matrix_buffer, 0x3f, (16 * 64));

    for (int i = 0; i < LED_MATRIX_ROWS; i++)
    {
        TEST_ASSERT_EQUAL_INT8_ARRAY(expected_led_matrix_buffer[i], led_matrix_buffer[i], (64));
    }
}

void test_turnoff_all_pixels(void)
{
    led_matrix__turnOnAllPixels(7);
    led_matrix__turnOffAllPixels();

    uint8_t expected_led_matrix_buffer[16][64];
    memset(expected_led_matrix_buffer, 0, (16 * 64));

    for (int i = 0; i < LED_MATRIX_ROWS; i++)
    {
        TEST_ASSERT_EQUAL_INT8_ARRAY(expected_led_matrix_buffer[i], led_matrix_buffer[i], (64));
    }
}

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

void test_select_next_row_0(void)
{
    gpio__reset_Expect(P_addrA);
    gpio__reset_Expect(P_addrB);
    gpio__reset_Expect(P_addrC);
    gpio__reset_Expect(P_addrD);

    led_matrix__selectRow(0);
}

void test_select_next_row_7(void)
{
    gpio__set_Expect(P_addrA);
    gpio__set_Expect(P_addrB);
    gpio__set_Expect(P_addrC);
    gpio__reset_Expect(P_addrD);

    led_matrix__selectRow(7);
}

void test_select_next_row_15(void)
{
    gpio__set_Expect(P_addrA);
    gpio__set_Expect(P_addrB);
    gpio__set_Expect(P_addrC);
    gpio__set_Expect(P_addrD);

    led_matrix__selectRow(15);
}

void test_select_next_row_16(void)
{
    gpio__reset_Expect(P_addrA);
    gpio__reset_Expect(P_addrB);
    gpio__reset_Expect(P_addrC);
    gpio__reset_Expect(P_addrD);

    led_matrix__selectRow(16);
}

void test_select_next_row_20(void)
{
    gpio__reset_Expect(P_addrA);
    gpio__reset_Expect(P_addrB);
    gpio__set_Expect(P_addrC);
    gpio__reset_Expect(P_addrD);

    led_matrix__selectRow(20);
}

void test_de_assert_output_enable_latch(void)
{
    gpio__set_Expect(P_OE);
    gpio__set_Expect(P_LATCH);
    led_matrix__de_assert_output_enable_latch();
}

void test_assert_output_disable_latch(void)
{
    gpio__reset_Expect(P_OE);
    gpio__reset_Expect(P_LATCH);
    led_matrix__assert_output_disable_latch();
}

void test_clockIn_Pixel_data(void)
{
    //For data
    gpio__reset_Expect(P_B1);
    gpio__reset_Expect(P_G1);
    gpio__reset_Expect(P_R1);
    gpio__reset_Expect(P_B2);
    gpio__reset_Expect(P_G2);
    gpio__reset_Expect(P_R2);

    //For clock
    gpio__reset_Expect(P_CLOCK);
    gpio__set_Expect(P_CLOCK);

    led_matrix__clock_in_pixel_data(0b00000000);
}

void test_clockIn_Pixel_data_2(void)
{
    //For data
    gpio__reset_Expect(P_B1);
    gpio__set_Expect(P_G1);
    gpio__reset_Expect(P_R1);
    gpio__set_Expect(P_B2);
    gpio__reset_Expect(P_G2);
    gpio__set_Expect(P_R2);

    //For clock
    gpio__reset_Expect(P_CLOCK);
    gpio__set_Expect(P_CLOCK);

    led_matrix__clock_in_pixel_data(0b00101010);
}

void test_clockIn_Pixel_data_3(void)
{
    //For data
    gpio__set_Expect(P_B1);
    gpio__set_Expect(P_G1);
    gpio__set_Expect(P_R1);
    gpio__set_Expect(P_B2);
    gpio__set_Expect(P_G2);
    gpio__set_Expect(P_R2);

    //For clock
    gpio__reset_Expect(P_CLOCK);
    gpio__set_Expect(P_CLOCK);

    led_matrix__clock_in_pixel_data(0b00111111);
}