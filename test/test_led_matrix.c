/*
To run the test 'cd' into test Framework directory and run the command as below

$ cd cd ..\UT_FrmWrk_Book\
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

#include "unity.h" // Single Unity Test Framework include
#include <stdbool.h>
#include <stdio.h>

void setUp(void)
{
    
}
void tearDown(void)
{
    
}

void test_passing(void)
{
    TEST_ASSERT_EQUAL(1, 1);
}