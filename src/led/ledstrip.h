/**
 * @author Peter Christoffersen
 * @brief WS281x LED Strip control 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>

#include "color.h"
#include "../boardconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

void led_strip_init();
void led_strip_set_correction(color_correction_t correction);
void led_strip_show();
void led_strip_fill(color_t color);

#ifdef __cplusplus
}
#endif


