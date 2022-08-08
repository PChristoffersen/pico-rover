/**
 * @author Peter Christoffersen
 * @brief Board health monitor 
 * @date 2022-08-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <util/mutex.h>


class INA219 {
    public:
        using callback_t = void (*)(float voltage, float current, float power);
        using addr_t = uint8_t;

        enum class Address: addr_t {
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

        void set_callback(callback_t cb) { m_callback = cb; }

    private:
        static constexpr int64_t UPDATE_INTERVAL = 250000;
        static constexpr float SHUNT_RESISTOR = 0.1f; // 0.1 Ohm shunt resistor

        mutable mutex_t m_mutex;

        addr_t m_address;
        bool m_present;

        absolute_time_t m_last_update;

        // Readings
        float m_shunt_v;
        float m_bus_v;
        float m_current;
        float m_power;

        callback_t m_callback;

        bool write_reg(uint8_t reg, uint16_t value);
        bool read_reg(uint8_t reg, uint16_t &value);
};

