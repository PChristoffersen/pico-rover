

#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/test.h>
#include <FreeRTOS.h>

PICOTEST_MODULE_NAME("dummy_test", "Dummy test harness");


int main()
{
    PICOTEST_START();

    PICOTEST_START_SECTION("Dummy");

    PICOTEST_END_SECTION()

    PICOTEST_END_TEST()

    return 0;
}