/**
 * @author Peter Christoffersen
 * @brief Radio receiver 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>
#include <pico/stdlib.h>

#include "telemetry.h"

#ifdef __cplusplus
extern "C" {
#endif

void radio_receiver_init();
void radio_receiver_begin();
bool radio_receiver_update();
bool radio_receiver_telemetry_push(const radio_telemetry_t *event);

#ifdef __cplusplus
}
#endif


