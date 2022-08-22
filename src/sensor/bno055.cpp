#include "bno0055.h"

#include <stdio.h>
#include <math.h>
#include <pico/stdlib.h>
#include <hardware/i2c.h>

#include <boardconfig.h>
#include <util/i2c_bus.h>
#include "bno055_regs.h"


namespace Sensor {


BNO055::BNO055(Address addr):
    m_address { static_cast<addr_type>(addr) },
    m_present { false }
{
    mutex_init(&m_mutex);
}

inline bool BNO055::write_reg8(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = { reg, value };
    if (i2c_write_blocking(i2c_default, m_address, buf, sizeof(buf), false) < static_cast<int>(sizeof(buf))) return false;
    return true;
}



bool BNO055::read_reg8(uint8_t reg, uint8_t &value)
{
    if (i2c_write_blocking(i2c_default, m_address, &reg, sizeof(reg), false) < static_cast<int>(sizeof(reg))) return false;
    if (i2c_read_blocking(i2c_default, m_address, &value, sizeof(value), false) < static_cast<int>(sizeof(value))) return false;
    return true;
}


inline bool BNO055::read(uint8_t reg, uint8_t *data, size_t len)
{
    if (i2c_write_blocking(i2c_default, m_address, &reg, sizeof(reg), false)<static_cast<int>(sizeof(reg))) return false;
    if (i2c_read_blocking(i2c_default, m_address, data, len, false)<static_cast<int>(len)) return false;
    return true;
}


bool BNO055::reset()
{
    // Reset the chip
    write_reg8(BNO055_SYS_RST_REG, BNO055_SYS_RST_MSK);
    auto start = get_absolute_time();
    sleep_ms(RESET_DELAY_MS);

    uint8_t reg = 0x00;
    while (true) {
        if (read_reg8(BNO055_CHIP_ID_REG, reg)) {
            if (reg==BNO055_CHIP_ID)
                break;
        }
        else if (absolute_time_diff_us(start, get_absolute_time()) > MAX_RESET_TIME_US) {
            assert(false);
            return false;
        }
        sleep_ms(10);
    }
    return true;
}


void BNO055::write_page_id(uint8_t page_id)
{
    write_reg8(BNO055_PAGE_ID_REG, page_id);
    m_page_id = page_id;
}


void BNO055::update_calib()
{
    uint8_t reg;
    read_reg8(BNO055_CALIB_STAT_ADDR, reg);

    mutex_enter_blocking(&m_mutex);
    m_mag_calib = (reg & BNO055_MAG_CALIB_STAT_MSK) >> BNO055_MAG_CALIB_STAT_POS;
    m_accel_calib = (reg & BNO055_ACCEL_CALIB_STAT_MSK) >> BNO055_ACCEL_CALIB_STAT_POS;
    m_gyro_calib = (reg & BNO055_GYRO_CALIB_STAT_MSK) >> BNO055_GYRO_CALIB_STAT_POS;
    mutex_exit(&m_mutex);
}


void BNO055::init()
{
    i2c_bus_acquire_blocking();

    // Set page to zero
    // The BNO005 chip appears to take a long time to come out of reset
    // and it messes up the i2c bus if requested while booting
    // until we are reasonably sure the chip is present or missing.
    auto start = get_absolute_time();
    auto since_boot = to_ms_since_boot(start);
    if (since_boot<RESET_DELAY_MS) {
        sleep_until(delayed_by_ms(start, RESET_DELAY_MS-since_boot));
    }
    while (true) {
        if (write_reg8(BNO055_PAGE_ID_REG, BNO055_PAGE_ZERO)) {
            break;
        }
        else if (absolute_time_diff_us(start, get_absolute_time()) > MAX_RESET_TIME_US) {
            m_present = false;
            i2c_bus_release();
            return;
        }
        sleep_ms(10);
    }

    // Verify chip id
    uint8_t reg = 0x00;
    if (!read_reg8(BNO055_CHIP_ID_REG, reg) || reg!=BNO055_CHIP_ID) {
        m_present = false;
        i2c_bus_release();
        return;
    }
    m_present = true;

    uint8_t buf[8];
    read(0, buf, sizeof(buf));
    m_sw_rev = static_cast<uint16_t>(buf[BNO055_SW_REV_ID_MSB_REG])<<8 | static_cast<uint16_t>(buf[BNO055_SW_REV_ID_LSB_REG]);
    m_bl_rev = buf[BNO055_BL_REV_ID_REG];
    #if 0
    printf("  ChipID: %02x\n", buf[BNO055_CHIP_ID_REG]);
    printf("   AccID: %02x\n", buf[BNO055_ACCEL_REV_ID_REG]);
    printf("   MagID: %02x\n", buf[BNO055_MAG_REV_ID_REG]);
    printf("   GyrID: %02x\n", buf[BNO055_GYRO_REV_ID_REG]);
    printf("  SW Rev: %04x\n", m_sw_rev);
    printf("  BL Rev: %02x\n", m_bl_rev);
    #endif

    write_page_id(BNO055_PAGE_ZERO);

    // Switch to config mode
    write_reg8(BNO055_OPERATION_MODE_REG, BNO055_OPERATION_MODE_CONFIG);

    // Set to normal power mode 
    write_reg8(BNO055_POWER_MODE_REG, BNO055_POWER_MODE_NORMAL);
    //write_reg8(BNO055_POWER_MODE_REG, BNO055_POWER_MODE_SUSPEND);
    sleep_ms(10);

    // Set axis mapping
    reg = 0x00;
    read_reg8(BNO055_AXIS_MAP_CONFIG_ADDR, reg);
    bno055_set_masked(reg, BNO055_REMAP_X_Y, BNO055_REMAP_AXIS_VALUE_MSK, BNO055_REMAP_AXIS_VALUE_POS);
    write_reg8(BNO055_AXIS_MAP_CONFIG_ADDR, reg);
    reg = 0x00;
    read_reg8(BNO055_AXIS_MAP_SIGN_ADDR, reg);
    //bno055_set_masked(reg, BNO055_SIGN)
    write_reg8(BNO055_AXIS_MAP_SIGN_ADDR, reg);



    // Set units
    reg = 0x00;
    read_reg8(BNO055_UNIT_SEL_ADDR, reg);
    bno055_set_masked(reg, BNO055_EULER_UNIT_RAD, BNO055_EULER_UNIT_MSK, BNO055_EULER_UNIT_POS);
    bno055_set_masked(reg, BNO055_GYRO_UNIT_DPS, BNO055_GYRO_UNIT_MSK, BNO055_GYRO_UNIT_POS);
    bno055_set_masked(reg, BNO055_ACCEL_UNIT_MSQ, BNO055_ACCEL_UNIT_MSK, BNO055_ACCEL_UNIT_POS);
    bno055_set_masked(reg, BNO055_TEMP_UNIT_CELSIUS, BNO055_TEMP_UNIT_MSK, BNO055_TEMP_UNIT_POS);
    write_reg8(BNO055_UNIT_SEL_ADDR, reg);

    write_reg8(BNO055_OPERATION_MODE_REG, BNO055_OPERATION_MODE_NDOF);
    sleep_ms(20);

    update_calib();

    i2c_bus_release();

    m_last_update = get_absolute_time();
    m_last_cali_update = m_last_update;

}



absolute_time_t BNO055::update()
{
    auto now = get_absolute_time();
    if (absolute_time_diff_us(m_last_update, now)>INTERVAL) {
        uint8_t data[3*sizeof(uint16_t)];

        if (!i2c_bus_try_acquire()) {
            return delayed_by_us(now, 100);
        }

        if (absolute_time_diff_us(m_last_cali_update, now)>CALI_INTERVAL) {
            update_calib();
            m_last_cali_update = now;
        }
        read(BNO055_EULER_H_LSB_ADDR, data, sizeof(data));
        i2c_bus_release();

        mutex_enter_blocking(&m_mutex);
        m_heading = static_cast<euler_type>(bno055_data_to_int16(data, BNO055_EULER_H_LSB_ADDR, BNO055_EULER_H_LSB_VALUEH_REG))/BNO055_EULER_DIV_RAD;
        m_pitch   = static_cast<euler_type>(bno055_data_to_int16(data, BNO055_EULER_H_LSB_ADDR, BNO055_EULER_P_LSB_VALUEP_REG))/BNO055_EULER_DIV_RAD;
        m_roll    = static_cast<euler_type>(bno055_data_to_int16(data, BNO055_EULER_H_LSB_ADDR, BNO055_EULER_R_LSB_VALUER_REG))/BNO055_EULER_DIV_RAD;
        mutex_exit(&m_mutex);

        m_last_update = delayed_by_us(m_last_update, INTERVAL);
    }
    return delayed_by_us(m_last_update, INTERVAL);
}


void BNO055::print() const 
{
    MUTEX_GUARD(m_mutex);
    printf("BNO055:   calib=%u,%u,%u  heading=%5.2f (%5.1f)   pitch=%5.2f (%5.1f)    roll=%5.2f (%5.1f)\n", m_mag_calib, m_accel_calib, m_gyro_calib, m_heading, m_heading*180.0/M_PI, m_pitch, m_pitch*180.0/M_PI, m_roll, m_roll*180.0/M_PI);
}

}
