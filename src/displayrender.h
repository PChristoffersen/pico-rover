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

#include <oled/display.h>
#include <sensor/ina219.h>
#include <sensor/pico_adc.h>
#include <rtos.h>

class DisplayRender {
    public:
        DisplayRender(OLED::Display &display, class Robot &robot);
        DisplayRender(const DisplayRender&) = delete; // No copy constructor
        DisplayRender(DisplayRender&&) = delete; // No move constructor

        void init();

        void off();

        void update_armed(bool armed);

    private:
        static constexpr uint TASK_STACK_SIZE    { configMINIMAL_STACK_SIZE };
        static constexpr uint UPDATE_INTERVAL_MS { 100u };

        static constexpr uint BATTERY_INTERVAL_MS { 200u };
        static constexpr uint RADIO_INTERVAL_MS   { 500u };

        OLED::Display &m_display;
        OLED::Framebuffer &m_framebuffer;

        class Robot &m_robot;

        StaticSemaphore_t m_sem_buf;
        SemaphoreHandle_t m_sem;

        StaticTask_t m_task_buf;
        StackType_t  m_task_stack[TASK_STACK_SIZE];
        TaskHandle_t m_task;

        uint m_battery_last_level;
        bool m_battery_show;

        inline void update_battery();
        inline void update_radio();

        inline void run();

};
