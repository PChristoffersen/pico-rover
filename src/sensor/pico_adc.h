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
            absolute_time_t update();

            float get_battery() const { MUTEX_GUARD(m_mutex); return m_battery_voltage; }
            float get_vsys() const    { MUTEX_GUARD(m_mutex); return m_vsys_voltage; }
            float get_temp() const    { MUTEX_GUARD(m_mutex); return m_temp; }

            void add_battery_cb(callback_type::call_type cb) { m_battery_cb.add(cb); }
            void add_vsys_cb(callback_type::call_type cb) { m_vsys_cb.add(cb); }
            void add_temp_cb(callback_type::call_type cb) { m_temp_cb.add(cb); }

        private:
            static constexpr int64_t UPDATE_INTERVAL = 250000u; // 250ms
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

            mutable mutex_t m_mutex;

            absolute_time_t m_last_update;

            float m_battery_voltage;
            float m_vsys_voltage;
            float m_temp;
        
            callback_type m_battery_cb;
            callback_type m_vsys_cb;
            callback_type m_temp_cb;

            void _handle_battery(float adc_voltage);
            void _handle_vsys(float adc_voltage);
            void _handle_temp(float adc_voltage);

    };

}
