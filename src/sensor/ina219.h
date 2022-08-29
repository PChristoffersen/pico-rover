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

            float get_shunt_voltage() const { SEMAPHORE_GUARD(m_sem); return m_shunt_v; }
            float get_bus_voltage() const   { SEMAPHORE_GUARD(m_sem); return m_bus_v; }
            float get_current() const       { SEMAPHORE_GUARD(m_sem); return m_current; }
            float get_power() const         { SEMAPHORE_GUARD(m_sem); return m_power; }

            void add_callback(callback_type::call_type cb) { m_callback.add(cb); }

        private:
            static constexpr uint    TASK_STACK_SIZE    { configMINIMAL_STACK_SIZE };
            static constexpr int64_t UPDATE_INTERVAL_MS { 250u };
            static constexpr float SHUNT_RESISTOR       { 0.1f }; // 0.1 Ohm shunt resistor

            const addr_type m_address;
            bool m_present;

            StaticSemaphore_t m_sem_buf;
            SemaphoreHandle_t m_sem;

            StaticTask_t m_task_buf;
            StackType_t  m_task_stack[TASK_STACK_SIZE];
            TaskHandle_t m_task;

            // Readings
            float m_shunt_v;
            float m_bus_v;
            float m_current;
            float m_power;

            callback_type m_callback;

            bool write_reg(uint8_t reg, uint16_t value);
            bool read_reg(uint8_t reg, uint16_t &value);

            inline void run();
    };

}
