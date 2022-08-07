/**
 * @author Peter Christoffersen
 * @brief Utility functions
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <pico/stdlib.h>



/**
 * Return the earliest of two timestamps
 */
static inline absolute_time_t earliest_time(absolute_time_t t1, absolute_time_t t2)
{
    if (absolute_time_diff_us(t1, t2)<0)
        return t1;
    else
        return t2;
}


