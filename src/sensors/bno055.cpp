#include "bno0055.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/i2c.h>

#include "../boardconfig.h"
#include "../i2c_bus.h"
#include "bno055_regs.h"





BNO055::BNO055(Address addr):
    m_address { static_cast<addr_t>(addr) },
    m_present { false }
{

}

inline bool BNO055::write_reg8(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = { reg, value };
    if (i2c_write_blocking(i2c_default, m_address, buf, sizeof(buf), false) < static_cast<int>(sizeof(buf))) return false;
    return true;
}



inline bool BNO055::write_reg16(uint8_t reg, uint16_t value)
{
    uint8_t buf[2] = { reg, static_cast<uint8_t>(value>>8) };
    if (i2c_write_blocking(i2c_default, m_address, buf, sizeof(buf), false) < static_cast<int>(sizeof(buf))) return false;
    buf[0] = reg+1;
    buf[1] = static_cast<uint8_t>(value & 0xFF);
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



void BNO055::init()
{
    i2c_bus_acquire_blocking();

    // Set page to zero
    if (!write_reg8(BNO055_PAGE_ID_REG, BNO055_PAGE_ZERO)) {
        m_present = false;
        i2c_bus_release();
        return;
    }

    // Verify chip id
    uint8_t reg = 0x00;
    if (!read_reg8(BNO055_CHIP_ID_REG, reg) || reg!=BNO055_CHIP_ID) {
        m_present = false;
        i2c_bus_release();
        return;
    }
    m_present = true;

    printf("Detected BNO055\n");

    uint8_t buf[8];
    read(0, buf, sizeof(buf));
    m_sw_rev = static_cast<uint16_t>(buf[BNO055_SW_REV_ID_MSB_REG])<<8 | static_cast<uint16_t>(buf[BNO055_SW_REV_ID_LSB_REG]);
    m_bl_rev = (uint16_t)buf[BNO055_BL_REV_ID_REG];
    printf("  ChipID: %02x\n", buf[BNO055_CHIP_ID_REG]);
    printf("   AccID: %02x\n", buf[BNO055_ACCEL_REV_ID_REG]);
    printf("   MagID: %02x\n", buf[BNO055_MAG_REV_ID_REG]);
    printf("   GyrID: %02x\n", buf[BNO055_GYRO_REV_ID_REG]);
    printf("  SW Rev: %04x\n", m_sw_rev);
    printf("  BL Rev: %02x\n", m_bl_rev);


    // Switch to config mode
    write_reg8(BNO055_OPERATION_MODE_REG, BNO055_OPERATION_MODE_CONFIG);

    // Reset the chip
    printf("Reset chip\n");
    write_reg8(BNO055_SYS_RST_REG, BNO055_SYS_RST_MSK);
    sleep_ms(50);
    while (true) {
        if (read_reg8(BNO055_CHIP_ID_REG, reg)) {
            if (reg==BNO055_CHIP_ID)
                break;
        }
        sleep_ms(10);
    }
    sleep_ms(50);
    printf("\n");
    printf("Reset complete\n");

    // Set to normal power mode 
    //write_reg8(BNO055_POWER_MODE_REG, BNO055_POWER_MODE_NORMAL);
    write_reg8(BNO055_POWER_MODE_REG, BNO055_POWER_MODE_SUSPEND);
    sleep_ms(10);

    i2c_bus_release();

    #if 0
    while (true) {
        bno055_update();
        sleep_ms(1000);


    }
    #endif

}

absolute_time_t BNO055::update()
{
    return make_timeout_time_ms(500);
}
