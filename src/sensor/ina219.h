/**
 * @author Peter Christoffersen
 * @brief Board health monitor 
 * @date 2022-08-02
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

    class INA219 {
        public:
            using callback_type = Callback<float,float,float>; // voltage, current, power;
            using addr_type = uint8_t;

            enum class Address: addr_type {
                INA0 = 0x40,
                INA1 = 0x41,
                INA2 = 0x42,
                INA3 = 0x43,
            };


            INA219(Address addr);
            INA219(const INA219&) = delete; // No copy constructor
            INA219(INA219&&) = delete; // No move constructor

            void init();

            bool present() const { return m_present; }

            absolute_time_t update();

            float get_shunt_voltage() const { MUTEX_GUARD(m_mutex); return m_shunt_v; }
            float get_bus_voltage() const   { MUTEX_GUARD(m_mutex); return m_bus_v; }
            float get_current() const       { MUTEX_GUARD(m_mutex); return m_current; }
            float get_power() const         { MUTEX_GUARD(m_mutex); return m_power; }

            void add_callback(callback_type::call_type cb) { m_callback.add(cb); }

        private:
            static constexpr int64_t UPDATE_INTERVAL = 250000; // 250ms
            static constexpr float SHUNT_RESISTOR = 0.1f; // 0.1 Ohm shunt resistor

            const addr_type m_address;
            bool m_present;

            absolute_time_t m_last_update;
            mutable mutex_t m_mutex;

            // Readings
            float m_shunt_v;
            float m_bus_v;
            float m_current;
            float m_power;

            callback_type m_callback;

            bool write_reg(uint8_t reg, uint16_t value);
            bool read_reg(uint8_t reg, uint16_t &value);
    };

}
