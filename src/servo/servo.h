/**
 * @author Peter Christoffersen
 * @brief Servo control 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>
#include <pico/stdlib.h>


class Servo {
    public:
        using value_t = uint16_t;

        Servo(uint pin);
        Servo(const Servo&) = delete; // No copy constructor
        Servo(Servo&&) = delete; // No move constructor

        void init();

        void set_enabled(bool enabled);

        void put(value_t us);

    private:
        static constexpr value_t INITIAL_POSITION = 1500u;

        uint m_pin;
        bool m_enabled;

        uint m_slice;
        uint m_channel;

        value_t m_value;
};

