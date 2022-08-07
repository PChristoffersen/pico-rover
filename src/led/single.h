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
            Single(uint pin);
            Single(const Single&) = delete; // No copy constructor
            Single(Single&&) = delete; // No move constructor

            void init();

            void on();
            void off();
            void blink();

            absolute_time_t update();

        private:
            static constexpr int64_t BLINK_INTERVAL = 500000u;

            uint m_pin;
            bool m_blinking;
            absolute_time_t m_blink_last;
    };

}
