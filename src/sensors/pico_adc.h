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

namespace Sensor {

    class PicoADC {
        public:
            using callback_t = std::function<void(float value)>;

            PicoADC(uint battery_pin, float battery_r1, float battery_r2);
            PicoADC(const PicoADC&) = delete; // No copy constructor
            PicoADC(PicoADC&&) = delete; // No move constructor

            void init();
            absolute_time_t update();

            float get_battery() const { MUTEX_GUARD(m_mutex); return m_battery_voltage; }
            float get_vsys() const    { MUTEX_GUARD(m_mutex); return m_vsys_voltage; }
            float get_temp() const    { MUTEX_GUARD(m_mutex); return m_temp; }

            void set_battery_cb(callback_t cb) { m_battery_cb = cb; }
            void set_vsys_cb(callback_t cb) { m_vsys_cb = cb; }
            void set_temp_cb(callback_t cb) { m_temp_cb = cb; }

        private:
            static constexpr int64_t UPDATE_INTERVAL = 250000u;
            static constexpr float   ADC_REF         = 3.3f;
            static constexpr uint    ADC_RESOLUTION  = (1u<<12);
            static constexpr uint    VSYS_PIN        = 29;
            static constexpr float   VSYS_R1         = 200000.0f;
            static constexpr float   VSYS_R2         = 100000.0f;
            static constexpr uint    TEMP_ADC        = 4;
            static constexpr float   BATTERY_MIN     = 3.0f;

            uint m_battery_pin;
            float m_battery_r1;
            float m_battery_r2;

            mutable mutex_t m_mutex;

            absolute_time_t m_last_update;

            float m_battery_voltage;
            float m_vsys_voltage;
            float m_temp;
        
            callback_t m_battery_cb;
            callback_t m_vsys_cb;
            callback_t m_temp_cb;

            void _handle_battery(float adc_voltage);
            void _handle_vsys(float adc_voltage);
            void _handle_temp(float adc_voltage);

    };

}
