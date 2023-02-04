/**
 * @author Peter Christoffersen
 * @brief BNO055 IMU
 * @date 2022-08-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <FreeRTOS.h>
#include <semphr.h>

namespace BNO055 {

    class Sensor {
        public:
            using addr_type = uint8_t;
            using euler_type = float;
            using calib_type = uint8_t;

            enum Address: addr_type {
                PRIMARY   = 0x28,
                ALTERNATE = 0x29,
            };

            Sensor(addr_type addr, UBaseType_t task_priority);
            Sensor(const Sensor&) = delete; // No copy constructor
            Sensor(Sensor&&) = delete; // No move constructor

            void init();
            void start();

            bool present() const { return m_present; }

            uint16_t sw_rev() const { return m_sw_rev; }
            uint8_t bl_rev() const { return m_bl_rev; }

            void lock() const 
            {
                xSemaphoreTake(m_sem, portMAX_DELAY);
            }
            void unlock() const
            {
                xSemaphoreGive(m_sem);
            }
            euler_type heading() const { return m_heading; }
            euler_type pitch() const   { return m_pitch; }
            euler_type roll() const    { return m_roll; }

            calib_type mag_calib() const { return m_mag_calib; }
            calib_type accel_calib() const { return m_accel_calib; }
            calib_type gyro_calib() const { return m_gyro_calib; }


            #ifndef NDEBUG
            void print() const;
            #endif

        protected:

            virtual void on_data(TickType_t tick_delta) {}

        private:
            static constexpr uint    TASK_STACK_SIZE    { configMINIMAL_STACK_SIZE };
            static constexpr int64_t UPDATE_INTERVAL_MS { 20u };
            static constexpr int64_t CALI_INTERVAL_MS { 500ll };

            static constexpr uint32_t RESET_DELAY_MS { 650 };
            static constexpr int64_t MAX_RESET_TIME_US { 800000ll };

            const addr_type m_address;
            const UBaseType_t m_task_priority;
            bool m_present;

            uint16_t m_sw_rev;
            uint8_t m_bl_rev;

            uint8_t m_page_id;
            calib_type m_mag_calib;
            calib_type m_accel_calib;
            calib_type m_gyro_calib;

            StaticSemaphore_t m_sem_buf;
            SemaphoreHandle_t m_sem;

            StaticTask_t m_task_buf;
            StackType_t  m_task_stack[TASK_STACK_SIZE];
            TaskHandle_t m_task;

            euler_type m_heading;
            euler_type m_pitch;
            euler_type m_roll;

            inline bool write_reg8(uint8_t reg, uint8_t value);
            bool read_reg8(uint8_t reg, uint8_t &value);
            inline bool read(uint8_t reg, uint8_t *data, size_t len);

            bool reset();
            void write_page_id(uint8_t page_id);

            void update_calib();

            inline void run();
    };

}
