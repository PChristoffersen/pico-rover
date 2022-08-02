/**
 * @author Peter Christoffersen
 * @brief Motor control 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <pico/stdlib.h>

#include "quadrature_encoder.pio.h"
#include "../boardconfig.h"


typedef enum {
    MOTOR_1 = 0u,
    MOTOR_2 = 1u,
    MOTOR_3 = 2u,
    MOTOR_4 = 3u,
} motor_t;


void motor_init();
void motor_setup();
void motor_set_supply_voltage(float voltage);
void motor_set_drivers_enabled(bool enabled);

void motor_set_enabled(motor_t motor, bool enabled);
void motor_put(motor_t motor, float duty);


static inline void motor_encoder_fetch_request(motor_t motor)
{
    quadrature_encoder_request_count(MOTOR_ENCODER_PIO, motor);
}

static inline int32_t motor_encoder_fetch(motor_t motor)
{
    return quadrature_encoder_fetch_count(MOTOR_ENCODER_PIO, motor);
}

static inline int32_t motor_encoder_get(motor_t motor)
{
    return quadrature_encoder_get_count(MOTOR_ENCODER_PIO, motor);
}



#ifdef __cplusplus
}
#endif

