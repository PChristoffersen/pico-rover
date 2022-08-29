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
#include <pico/critical_section.h>
#include <rtos.h>

class semaphore_guard_t {
    public:
        semaphore_guard_t(SemaphoreHandle_t sem) : m_sem { sem } 
        { 
            xSemaphoreTake(m_sem, portMAX_DELAY);
        }
        ~semaphore_guard_t() {
            xSemaphoreGive(m_sem);
        }
    private:
        SemaphoreHandle_t m_sem;
};
#define SEMAPHORE_GUARD(sem) semaphore_guard_t _guard(sem);


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


class critical_section_guard_t {
    public:
        critical_section_guard_t(critical_section_t &lock) : m_lock { lock } 
        { 
            critical_section_enter_blocking(&m_lock);
        }
        ~critical_section_guard_t() {
            critical_section_exit(&m_lock);
        }
    private:
        critical_section_t &m_lock;
};
#define CRITICAL_SECTION_GUARD(lock) critical_section_guard_t _cguard(lock);
