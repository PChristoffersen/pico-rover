/**
 * @author Peter Christoffersen
 * @brief Servo control 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <array>
#include <stdio.h>
#include <pico/stdlib.h>
#include <rtos.h>

#include <boardconfig.h>

namespace Motor {
    class Servo {
        public:
            using array_type = std::array<Servo,SERVO_COUNT>;
            using id_type = uint;
            using value_t = uint16_t;

            static constexpr value_t INITIAL_POSITION = 1500u;

            Servo(id_type id, uint pin, value_t initial = INITIAL_POSITION);
            Servo(const Servo&) = delete; // No copy constructor
            Servo(Servo&&) = delete; // No move constructor

            id_type id() const { return m_id; }

            void init();

            void set_enabled(bool enabled);

            void put(value_t us);

        private:
            /* PWM Parameters to setup the hardware to pulse every 20ms
             * 
             * We setup the hardware so the channel level directly corresponds to 
             * the servo pulse length in microseconds.
             */
            static constexpr uint16_t PWM_WRAP = 20000u;
            static constexpr float PWM_DIV = 1000000.0f;

            const id_type m_id;
            const uint m_pin;

            uint m_slice;
            uint m_channel;

            StaticSemaphore_t m_mutex_buf;
            SemaphoreHandle_t m_mutex;

            bool m_enabled;
            value_t m_value;
    };

}
