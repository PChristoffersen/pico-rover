/**
 * @author Peter Christoffersen
 * @brief Single LED control 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>
#include <pico/stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void led_init();

void led_on();
void led_off();
void led_blink();
void led_update();

#ifdef __cplusplus
}
#endif

