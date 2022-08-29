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

#include <util/callback.h>
#include <util/locking.h>

namespace Sensor {

    class BNO055 {
        public:
            using addr_type = uint8_t;
            using euler_type = float;

            enum class Address: addr_type {
                PRIMARY   = 0x28,
                ALTERNATE = 0x29,
            };

            BNO055(Address addr = Address::PRIMARY);

            void init();

            bool present() const { return m_present; }

            uint16_t sw_rev() const { return m_sw_rev; }
            uint8_t bl_rev() const { return m_bl_rev; }

            euler_type heading() const { SEMAPHORE_GUARD(m_sem); return m_heading; }
            euler_type pitch() const   { SEMAPHORE_GUARD(m_sem); return m_pitch; }
            euler_type roll() const    { SEMAPHORE_GUARD(m_sem); return m_roll; }

            #ifndef NDEBUG
            void print() const;
            #endif

        private:
            static constexpr uint    TASK_STACK_SIZE    { configMINIMAL_STACK_SIZE };
            static constexpr int64_t UPDATE_INTERVAL_MS { 25u };
            static constexpr int64_t CALI_INTERVAL_MS { 500ll };

            static constexpr uint32_t RESET_DELAY_MS { 650 };
            static constexpr int64_t MAX_RESET_TIME_US { 800000ll };

            const addr_type m_address;
            bool m_present;

            uint16_t m_sw_rev;
            uint8_t m_bl_rev;

            uint8_t m_page_id;
            uint8_t m_mag_calib;
            uint8_t m_accel_calib;
            uint8_t m_gyro_calib;

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
