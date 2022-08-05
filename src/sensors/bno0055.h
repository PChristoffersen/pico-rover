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


class BNO055 {
    public:
        using addr_t = uint8_t;

        enum class Address: addr_t {
            PRIMARY   = 0x28,
            ALTERNATE = 0x29,
        };

        BNO055(Address addr = Address::PRIMARY);

        void init();

        bool present() const { return m_present; }

        uint16_t sw_rev() const { return m_sw_rev; }
        uint8_t bl_rev() const { return m_bl_rev; }

        absolute_time_t update();
    
    private:
        addr_t m_address;
        bool m_present;

        uint16_t m_sw_rev;
        uint8_t m_bl_rev;

        inline bool write_reg8(uint8_t reg, uint8_t value);
        inline bool write_reg16(uint8_t reg, uint16_t value);
        bool read_reg8(uint8_t reg, uint8_t &value);
        inline bool read(uint8_t reg, uint8_t *data, size_t len);
};

