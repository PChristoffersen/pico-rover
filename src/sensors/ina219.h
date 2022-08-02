/**
 * @author Peter Christoffersen
 * @brief Board health monitor 
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

void ina219_init();
void ina219_update();


#ifdef __cplusplus
}
#endif

