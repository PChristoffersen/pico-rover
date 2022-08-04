/**
 * @author Peter Christoffersen
 * @brief Debug macros
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <pico/stdlib.h>

void debug_init();


#if 0
#define debug_pin_init() {}
#define debug_pin0(state) {}
#define debug_pin1(state) {}
#define debug_pin2(state) {}
#define debug_pin3(state) {}
#else

#include "../boardconfig.h"
#define DEBUG_PIN0 RADIO_RECEIVER_TX_PIN
#define DEBUG_PIN1 RADIO_RECEIVER_RX_PIN
#define DEBUG_PIN2 SERVO1_PIN
#define DEBUG_PIN3 SERVO2_PIN

static inline void debug_pin_init()
{
    #ifdef DEBUG_PIN0
    gpio_init(DEBUG_PIN0);
    gpio_set_dir(DEBUG_PIN0, GPIO_OUT);
    gpio_put(DEBUG_PIN0, false);
    #endif
    #ifdef DEBUG_PIN1
    gpio_init(DEBUG_PIN1);
    gpio_set_dir(DEBUG_PIN1, GPIO_OUT);
    gpio_put(DEBUG_PIN1, false);
    #endif
    #ifdef DEBUG_PIN2
    gpio_init(DEBUG_PIN2);
    gpio_set_dir(DEBUG_PIN2, GPIO_OUT);
    gpio_put(DEBUG_PIN2, false);
    #endif
    #ifdef DEBUG_PIN3
    gpio_init(DEBUG_PIN3);
    gpio_set_dir(DEBUG_PIN3, GPIO_OUT);
    gpio_put(DEBUG_PIN3, false);
    #endif

}


static inline void debug_pin0(bool state)
{
    #ifdef DEBUG_PIN0
    gpio_put(DEBUG_PIN0, state);
    #endif
}
static inline void debug_pin1(bool state)
{
    #ifdef DEBUG_PIN1
    gpio_put(DEBUG_PIN1, state);
    #endif
}
static inline void debug_pin2(bool state)
{
    #ifdef DEBUG_PIN2
    gpio_put(DEBUG_PIN2, state);
    #endif
}
static inline void debug_pin3(bool state)
{
    #ifdef DEBUG_PIN3
    gpio_put(DEBUG_PIN3, state);
    #endif
}

#endif