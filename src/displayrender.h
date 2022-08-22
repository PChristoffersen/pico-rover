/**
 * @author Peter Christoffersen
 * @brief Display update
 * @date 2022-08-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <pico/stdlib.h>

#include <oled/display.h>
#include <sensor/ina219.h>
#include <sensor/pico_adc.h>
#include <radio/frsky_receiverlistener.h>

class DisplayRender {
    public:
        DisplayRender(OLED::Display &display, class Robot &robot);
        DisplayRender(const DisplayRender&) = delete; // No copy constructor
        DisplayRender(DisplayRender&&) = delete; // No move constructor

        void init();
        void begin();

        void off();

        absolute_time_t update();
        void update_armed(bool armed);

    private:
        static constexpr int64_t INTERVAL_STAGGER = 10000ll;
        static constexpr int64_t BATTERY_INTERVAL = 200000ll;
        static constexpr int64_t RADIO_INTERVAL   = 500000ll;

        OLED::Display &m_display;
        OLED::Framebuffer &m_framebuffer;

        class Robot &m_robot;

        absolute_time_t m_battery_last_update;
        uint m_battery_last_level;
        bool m_battery_show;

        absolute_time_t m_radio_last_update;

        inline absolute_time_t update_battery();
        inline absolute_time_t update_radio();

};
