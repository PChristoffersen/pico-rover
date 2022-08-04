/**
 * @author Peter Christoffersen
 * @brief BNO055 IMU
 * @date 2022-08-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <pico/stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void bno055_init();


void bno055_update();


#ifdef __cplusplus
}
#endif

