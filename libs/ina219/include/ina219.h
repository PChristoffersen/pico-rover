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
#include <FreeRTOS.h>
#include <semphr.h>

namespace INA219 {

    class Sensor {
        public:
            using addr_type = uint8_t;

            enum Address: addr_type {
                INA0 = 0x40,
                INA1 = 0x41,
                INA2 = 0x42,
                INA3 = 0x43,
            };


            Sensor(addr_type addr, UBaseType_t task_priority);
            Sensor(const Sensor&) = delete; // No copy constructor
            Sensor(Sensor&&) = delete; // No move constructor

            void init();
            void start();

            bool present() const { return m_present; }

            absolute_time_t update();

            void lock() const 
            {
                xSemaphoreTake(m_sem, portMAX_DELAY);
            }
            void unlock() const
            {
                xSemaphoreGive(m_sem);
            }
            float get_shunt_voltage() const { return m_shunt_v; }
            float get_bus_voltage() const   { return m_bus_v; }
            float get_current() const       { return m_current; }
            float get_power() const         { return m_power; }

        protected:

            virtual void on_data(float bus_v, float current, float power) {}

        private:
            static constexpr uint    TASK_STACK_SIZE    { configMINIMAL_STACK_SIZE };
            static constexpr int64_t UPDATE_INTERVAL_MS { 250u };
            static constexpr float SHUNT_RESISTOR       { 0.1f }; // 0.1 Ohm shunt resistor

            const addr_type m_address;
            const UBaseType_t m_task_priority;
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

            bool write_reg(uint8_t reg, uint16_t value);
            bool read_reg(uint8_t reg, uint16_t &value);

            inline void run();
    };

}
