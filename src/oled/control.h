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
#include <rtos.h>

#include <sensor/sensors.h>
#include <sensor/pico_adc.h>
#include <radio/radio.h>
#include <rtos.h>

#include <ssd1306/display.h>
#include <ssd1306/font.h>
#include <ssd1306/image.h>

class Robot;

namespace OLED {
    class Control {
        public:
            using Display = SSD1306::Display;
            using Font = SSD1306::Font;
            using Image = SSD1306::Image;


            Control(Robot &robot);
            Control(const Control&) = delete; // No copy constructor
            Control(Control&&) = delete; // No move constructor

            void init();
            void start();

            void off();

            Display &display() { return m_display; }

        private:
            using framebuffer_type = Display::framebuffer_type;
            static constexpr uint TASK_STACK_SIZE    { configMINIMAL_STACK_SIZE };
            static constexpr uint UPDATE_INTERVAL_MS { 100u };

            static constexpr uint START_DELAY_MS { 5000u };
            static constexpr uint BATTERY_INTERVAL_MS { 200u };
            static constexpr uint RADIO_INTERVAL_MS   { 500u };
            static constexpr uint ARMED_INTERVAL_MS   { 100u };

            struct State {
                bool armed;
                uint battery_level;
                State();
            };

            Display m_display;
            framebuffer_type &m_framebuffer;

            Robot &m_robot;

            StaticSemaphore_t m_sem_buf;
            SemaphoreHandle_t m_sem;

            StaticTask_t m_task_buf;
            StackType_t  m_task_stack[TASK_STACK_SIZE];
            TaskHandle_t m_task;

            State m_state_drawn;

            bool m_battery_show;

            void update_armed(bool force);
            void update_battery(bool force);
            void update_radio(bool force);

            inline void run();

    };

}
