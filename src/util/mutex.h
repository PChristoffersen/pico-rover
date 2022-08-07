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
