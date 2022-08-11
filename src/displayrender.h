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
#include <sensors/ina219.h>
#include <radio/frsky_receiver.h>

class DisplayRender {
    public:
        using battery_sensor_type = Sensor::INA219;
        using radio_receiver_type = Radio::FrSky::Receiver;

        DisplayRender(OLED::Display &display);

        void init();
        void begin();

        void off();

        absolute_time_t update_battery(battery_sensor_type &sensor);
        absolute_time_t update_radio(radio_receiver_type &receiver);

    private:
        static constexpr int64_t INTERVAL_STAGGER = 100000ll;
        static constexpr int64_t BATTERY_INTERVAL = 500000ll;
        static constexpr int64_t RADIO_INTERVAL   = 500000ll;

        OLED::Display &m_display;
        OLED::Framebuffer &m_framebuffer;
        
        absolute_time_t m_battery_last_update;
        uint m_battery_last_level;
        bool m_battery_show;

        absolute_time_t m_radio_last_update;
};
