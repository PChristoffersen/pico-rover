/**
 * @author Peter Christoffersen
 * @brief Servo control 
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

typedef enum {
    SERVO_1 = 0u,
    SERVO_2 = 1u,
} servo_t;

void servo_init();
void servo_set_enabled(servo_t servo, bool enabled);
void servo_put(servo_t servo, uint16_t us);

#ifdef __cplusplus
}
#endif

