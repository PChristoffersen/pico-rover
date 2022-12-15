/**
 * @author Peter Christoffersen
 * @brief GPIO LED control 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>
#include <pico/stdlib.h>
#include <rtos.h>

namespace LED {

    class GPIOLed {
        public:
            static constexpr int64_t DEFAULT_BLINK_INTERVAL_MS = 500u;

            GPIOLed(uint pin);
            GPIOLed(const GPIOLed&) = delete; // No copy constructor
            GPIOLed(GPIOLed&&) = delete; // No move constructor

            void init();

            void on() { set(true); }
            void off() { set(false); }
            void blink();

            bool get_blinking() const { return m_blinking; }
            void set_interval(int64_t interval) { m_interval = interval; }

        private:
            static constexpr uint TASK_STACK_SIZE { configMINIMAL_STACK_SIZE };
            StaticTask_t m_task_buf;
            StackType_t m_task_stack[TASK_STACK_SIZE];
            TaskHandle_t m_task;

            StaticSemaphore_t m_mutex_buf;
            SemaphoreHandle_t m_mutex;

            const uint m_pin;
            bool m_blinking;
            bool m_state;
            TickType_t m_interval;
            
            void set(bool on);
            inline void run();
    };

}
