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

class LED {
    public:
        LED(uint pin);
        LED(const LED&) = delete; // No copy constructor
        LED(LED&&) = delete; // No move constructor

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

