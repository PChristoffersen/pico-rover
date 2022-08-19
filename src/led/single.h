/**
 * @author Peter Christoffersen
 * @brief Single LED control 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>
#include <pico/stdlib.h>

namespace LED {

    class Single {
        public:
            static constexpr int64_t DEFAULT_BLINK_INTERVAL = 500000u;

            Single(uint pin);
            Single(const Single&) = delete; // No copy constructor
            Single(Single&&) = delete; // No move constructor

            void init();

            void on();
            void off();
            bool blink();

            absolute_time_t update();

            bool get_blinking() const { return m_blinking; }
            void set_interval(int64_t interval) { m_interval = interval; }

        private:

            const uint m_pin;
            bool m_blinking;
            int64_t m_interval;
            absolute_time_t m_blink_last;
    };

}
