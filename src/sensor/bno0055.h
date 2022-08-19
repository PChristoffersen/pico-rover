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

            euler_type heading() const { MUTEX_GUARD(m_mutex); return m_heading; }
            euler_type pitch() const   { MUTEX_GUARD(m_mutex); return m_pitch; }
            euler_type roll() const    { MUTEX_GUARD(m_mutex); return m_roll; }

            absolute_time_t update();
        
        private:
            static constexpr uint32_t RESET_DELAY_MS { 650 };
            static constexpr int64_t MAX_RESET_TIME_US { 800000ll };
            static constexpr int64_t INTERVAL { 500000 };
            static constexpr int64_t CALI_INTERVAL { 5000000ll };

            const addr_type m_address;
            bool m_present;

            uint16_t m_sw_rev;
            uint8_t m_bl_rev;

            uint8_t m_page_id;
            uint8_t m_mag_calib;
            uint8_t m_accel_calib;
            uint8_t m_gyro_calib;

            mutable mutex_t m_mutex;
            euler_type m_heading;
            euler_type m_pitch;
            euler_type m_roll;

            absolute_time_t m_last_update;
            absolute_time_t m_last_cali_update;

            inline bool write_reg8(uint8_t reg, uint8_t value);
            bool read_reg8(uint8_t reg, uint8_t &value);
            inline bool read(uint8_t reg, uint8_t *data, size_t len);

            bool reset();
            void write_page_id(uint8_t page_id);

            void update_calib();
    };

}
