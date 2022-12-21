/**
 * @author Peter Christoffersen
 * @brief Board health monitor 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <functional>
#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <rtos.h>
#include <util/locking.h>
#include <util/callback.h>

namespace Sensor {

    class PicoADC {
        public:
            using callback_type = Callback<float>;

            PicoADC(uint battery_pin, float battery_r1, float battery_r2);
            PicoADC(const PicoADC&) = delete; // No copy constructor
            PicoADC(PicoADC&&) = delete; // No move constructor

            void init();
            void start();

            float get_battery() const { SEMAPHORE_GUARD(m_sem); return m_battery_voltage; }
            float get_vsys() const    { SEMAPHORE_GUARD(m_sem); return m_vsys_voltage; }
            float get_temp() const    { SEMAPHORE_GUARD(m_sem); return m_temp; }

            void add_battery_cb(callback_type::call_type cb) { m_battery_cb.add(cb); }
            void add_vsys_cb(callback_type::call_type cb) { m_vsys_cb.add(cb); }
            void add_temp_cb(callback_type::call_type cb) { m_temp_cb.add(cb); }

        private:
            static constexpr uint    TASK_STACK_SIZE { configMINIMAL_STACK_SIZE };
            static constexpr int64_t UPDATE_INTERVAL_MS = 250u;
            static constexpr float   ADC_REF         = 3.3f;
            static constexpr uint    ADC_RESOLUTION  = (1u<<12);
            static constexpr uint    VSYS_PIN        = 29;
            static constexpr float   VSYS_R1         = 200000.0f;
            static constexpr float   VSYS_R2         = 100000.0f;
            static constexpr uint    TEMP_ADC        = 4;
            static constexpr float   BATTERY_MIN     = 3.0f;

            const uint m_battery_pin;
            const float m_battery_r1;
            const float m_battery_r2;

            StaticSemaphore_t m_sem_buf;
            SemaphoreHandle_t m_sem;

            StaticTask_t m_task_buf;
            StackType_t  m_task_stack[TASK_STACK_SIZE];
            TaskHandle_t m_task;

            float m_battery_voltage;
            float m_vsys_voltage;
            float m_temp;
        
            callback_type m_battery_cb;
            callback_type m_vsys_cb;
            callback_type m_temp_cb;

            void _handle_battery(float adc_voltage);
            void _handle_vsys(float adc_voltage);
            void _handle_temp(float adc_voltage);

            inline void run();
    };

}
