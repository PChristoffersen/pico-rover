/**
 * @author Peter Christoffersen
 * @brief Board health monitor 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <pico/stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


void board_adc_init();
void board_adc_update();

float board_adc_battery_get();
float board_adc_vsys_get();
float board_adc_temp_get();

#ifdef __cplusplus
}
#endif

