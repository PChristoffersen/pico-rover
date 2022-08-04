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
#include <pico/mutex.h>



/**
 * Return the smallest of two timestamps
 */
static inline absolute_time_t smallest_time(absolute_time_t t1, absolute_time_t t2)
{
    if (absolute_time_diff_us(t1, t2)<0)
        return t1;
    else
        return t2;
}



class mutex_guard_t {
    public:
        mutex_guard_t(mutex_t &mutex) : m_mutex { mutex } 
        { 
            mutex_enter_blocking(&m_mutex);            
        }
        ~mutex_guard_t() {
            mutex_exit(&m_mutex);
        }
    private:
        mutex_t &m_mutex;
};
#define MUTEX_GUARD(mutex) mutex_guard_t _guard(mutex);
