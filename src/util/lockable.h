#pragma once

#include <stdio.h>
#include <pico/stdio.h>
#include <rtos.h>

class Lockable {
    public:
        Lockable();
        Lockable(const Lockable&) = delete; // No copy constructor
        Lockable(Lockable&&) = delete; // No move constructor

        void lock() const 
        {
            xSemaphoreTake(m_sem, portMAX_DELAY);
        }
        void unlock() const
        {
            xSemaphoreGive(m_sem);
        }

    protected:
        SemaphoreHandle_t &sem() { return m_sem; }

        class guard {
            public:
                guard(SemaphoreHandle_t sem) : m_sem { sem } 
                {
                    xSemaphoreTake(m_sem, portMAX_DELAY);
                }
                guard(guard&) = delete;
                guard(guard &&other) = delete;
                ~guard() 
                {
                    xSemaphoreGive(m_sem);
                }
            private:
                SemaphoreHandle_t &m_sem;
        };

    private:
        StaticSemaphore_t m_sem_buf;
        mutable SemaphoreHandle_t m_sem;
};

#define LOCK_GUARD() guard _lock(sem())
