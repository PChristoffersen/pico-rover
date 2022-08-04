#include "bno0055.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/i2c.h>

#include "../boardconfig.h"
#include "../i2c_bus.h"


#define BNO055_ADDRESS_PRI  0x29
#define BNO055_ADDRESS_ALT 0x28
#define BNO055_ADDRESS BNO055_ADDRESS_ALT

#define BNO055_ID 0xA0


/*********************************************************
 *  Page 0 Registers 
 *********************************************************/
#define BNO055_REG_CHIP_ID       0x00
#define BNO055_REG_ACC_ID        0x01
#define BNO055_REG_MAG_ID        0x02
#define BNO055_REG_GYR_ID        0x03
#define BNO055_REG_SW_REV_ID_LSB 0x04
#define BNO055_REG_SW_REV_ID_MSB 0x05
#define BNO055_REG_BL_REV_ID     0x06
#define BNO055_REG_PAGE_ID       0x07

/* Accel data register */
#define BNO055_REG_ACCEL_DATA_X_LSB 0X08
#define BNO055_REG_ACCEL_DATA_X_MSB 0X09
#define BNO055_REG_ACCEL_DATA_Y_LSB 0X0A
#define BNO055_REG_ACCEL_DATA_Y_MSB 0X0B
#define BNO055_REG_ACCEL_DATA_Z_LSB 0X0C
#define BNO055_REG_ACCEL_DATA_Z_MSB 0X0D

/* Mag data register */
#define BNO055_REG_MAG_DATA_X_LSB 0X0E
#define BNO055_REG_MAG_DATA_X_MSB 0X0F
#define BNO055_REG_MAG_DATA_Y_LSB 0X10
#define BNO055_REG_MAG_DATA_Y_MSB 0X11
#define BNO055_REG_MAG_DATA_Z_LSB 0X12
#define BNO055_REG_MAG_DATA_Z_MSB 0X13

/* Gyro data registers */
#define BNO055_REG_GYRO_DATA_X_LSB 0X14
#define BNO055_REG_GYRO_DATA_X_MSB 0X15
#define BNO055_REG_GYRO_DATA_Y_LSB 0X16
#define BNO055_REG_GYRO_DATA_Y_MSB 0X17
#define BNO055_REG_GYRO_DATA_Z_LSB 0X18
#define BNO055_REG_GYRO_DATA_Z_MSB 0X19

/* Euler data registers */
#define BNO055_REG_EULER_H_LSB 0X1A
#define BNO055_REG_EULER_H_MSB 0X1B
#define BNO055_REG_EULER_R_LSB 0X1C
#define BNO055_REG_EULER_R_MSB 0X1D
#define BNO055_REG_EULER_P_LSB 0X1E
#define BNO055_REG_EULER_P_MSB 0X1F

/* Quaternion data registers */
#define BNO055_REG_QUATERNION_DATA_W_LSB 0X20
#define BNO055_REG_QUATERNION_DATA_W_MSB 0X21
#define BNO055_REG_QUATERNION_DATA_X_LSB 0X22
#define BNO055_REG_QUATERNION_DATA_X_MSB 0X23
#define BNO055_REG_QUATERNION_DATA_Y_LSB 0X24
#define BNO055_REG_QUATERNION_DATA_Y_MSB 0X25
#define BNO055_REG_QUATERNION_DATA_Z_LSB 0X26
#define BNO055_REG_QUATERNION_DATA_Z_MSB 0X27

/* Linear acceleration data registers */
#define BNO055_REG_LINEAR_ACCEL_DATA_X_LSB 0X28
#define BNO055_REG_LINEAR_ACCEL_DATA_X_MSB 0X29
#define BNO055_REG_LINEAR_ACCEL_DATA_Y_LSB 0X2A
#define BNO055_REG_LINEAR_ACCEL_DATA_Y_MSB 0X2B
#define BNO055_REG_LINEAR_ACCEL_DATA_Z_LSB 0X2C
#define BNO055_REG_LINEAR_ACCEL_DATA_Z_MSB 0X2D

/* Gravity data registers */
#define BNO055_REG_GRAVITY_DATA_X_LSB 0X2E
#define BNO055_REG_GRAVITY_DATA_X_MSB 0X2F
#define BNO055_REG_GRAVITY_DATA_Y_LSB 0X30
#define BNO055_REG_GRAVITY_DATA_Y_MSB 0X31
#define BNO055_REG_GRAVITY_DATA_Z_LSB 0X32
#define BNO055_REG_GRAVITY_DATA_Z_MSB 0X33

/* Temperature data register */
#define BNO055_REG_TEMP 0X34

/* Status registers */
#define BNO055_REG_CALIB_STAT      0X35
#define BNO055_REG_SELFTEST_RESULT 0X36
#define BNO055_REG_INTR_STAT       0X37

#define BNO055_REG_SYS_CLK_STAT 0X38
#define BNO055_REG_SYS_STAT 0X39
#define BNO055_REG_SYS_ERR 0X3A

/* Unit selection register */
#define BNO055_REG_UNIT_SEL 0X3B

/* Mode registers */
#define BNO055_REG_OPR_MODE 0X3D
#define BNO055_REG_PWR_MODE 0X3E

#define BNO055_REG_SYS_TRIGGER 0X3F
#define BNO055_REG_TEMP_SOURCE 0X40

/* Axis remap registers */
#define BNO055_REG_AXIS_MAP_CONFIG 0X41
#define BNO055_REG_AXIS_MAP_SIGN 0X42

/* SIC registers */
#define BNO055_REG_SIC_MATRIX_0_LSB 0X43
#define BNO055_REG_SIC_MATRIX_0_MSB 0X44
#define BNO055_REG_SIC_MATRIX_1_LSB 0X45
#define BNO055_REG_SIC_MATRIX_1_MSB 0X46
#define BNO055_REG_SIC_MATRIX_2_LSB 0X47
#define BNO055_REG_SIC_MATRIX_2_MSB 0X48
#define BNO055_REG_SIC_MATRIX_3_LSB 0X49
#define BNO055_REG_SIC_MATRIX_3_MSB 0X4A
#define BNO055_REG_SIC_MATRIX_4_LSB 0X4B
#define BNO055_REG_SIC_MATRIX_4_MSB 0X4C
#define BNO055_REG_SIC_MATRIX_5_LSB 0X4D
#define BNO055_REG_SIC_MATRIX_5_MSB 0X4E
#define BNO055_REG_SIC_MATRIX_6_LSB 0X4F
#define BNO055_REG_SIC_MATRIX_6_MSB 0X50
#define BNO055_REG_SIC_MATRIX_7_LSB 0X51
#define BNO055_REG_SIC_MATRIX_7_MSB 0X52
#define BNO055_REG_SIC_MATRIX_8_LSB 0X53
#define BNO055_REG_SIC_MATRIX_8_MSB 0X54

/* Accelerometer Offset registers */
#define BNO055_REG_ACCEL_OFFSET_X_LSB 0X55
#define BNO055_REG_ACCEL_OFFSET_X_MSB 0x5B
#define BNO055_REG_MAG_OFFSET_X_MSB 0X5C
#define BNO055_REG_MAG_OFFSET_Y_LSB 0X5D
#define BNO055_REG_MAG_OFFSET_Y_MSB 0X5E
#define BNO055_REG_MAG_OFFSET_Z_LSB 0X5F
#define BNO055_REG_MAG_OFFSET_Z_MSB 0X60

/* Gyroscope Offset register s*/
#define BNO055_REG_GYRO_OFFSET_X_LSB 0X61
#define BNO055_REG_GYRO_OFFSET_X_MSB 0X62
#define BNO055_REG_GYRO_OFFSET_Y_LSB 0X63
#define BNO055_REG_GYRO_OFFSET_Y_MSB 0X64
#define BNO055_REG_GYRO_OFFSET_Z_LSB 0X65
#define BNO055_REG_GYRO_OFFSET_Z_MSB 0X66

/* Radius registers */
#define BNO055_REG_ACCEL_RADIUS_LSB 0X67
#define BNO055_REG_ACCEL_RADIUS_MSB 0X68
#define BNO055_REG_MAG_RADIUS_LSB 0X69
#define BNO055_REG_MAG_RADIUS_MSB 0X6


/*********************************************************
 *  Page 0 Registers 
 *********************************************************/
#define BNO055_REG_UNIQUE_ID 0x50



/*********************************************************
 *  Register values
 *********************************************************/

#define BNO055_OPR_MODE_CONFIG       0b0000
#define BNO055_OPR_MODE_ACCONLY      0b0001
#define BNO055_OPR_MODE_MAGONLY      0b0010
#define BNO055_OPR_MODE_GYRONLY      0b0011
#define BNO055_OPR_MODE_ACCMAG       0b0100
#define BNO055_OPR_MODE_ACCGYRO      0b0101
#define BNO055_OPR_MODE_MAGGYRO      0b0110
#define BNO055_OPR_MODE_AMG          0b0111
#define BNO055_OPR_MODE_IMUPLUS      0b1000
#define BNO055_OPR_MODE_COMPASS      0b1001
#define BNO055_OPR_MODE_M4G          0b1010
#define BNO055_OPR_MODE_NDOF_FMC_OFF 0b1011
#define BNO055_OPR_MODE_NDOF         0b1100

#define BNO055_PWR_MODE_NORMAL       0b00
#define BNO055_PWR_MODE_LOWPOWER     0b01
#define BNO055_PWR_MODE_SUSPEND      0b10

#define BNO055_SYS_TRIGGER_SELF_TEST (1<<0)
#define BNO055_SYS_TRIGGER_RST_SYS   (1<<5)
#define BNO055_SYS_TRIGGER_RST_INT   (1<<6)
#define BNO055_SYS_TRIGGER_CLK_SEL   (1<<7)



static bool g_device_present = false;


static inline bool _bno055_write_reg8(uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = { reg, value };
    if (i2c_write_blocking(i2c_default, BNO055_ADDRESS, buf, sizeof(buf), false)<=0) return false;
    return true;
}

static inline bool _bno055_write_reg16(uint8_t reg, uint16_t value)
{

    uint8_t buf[2] = { reg, value>>8 };
    if (i2c_write_blocking(i2c_default, BNO055_ADDRESS, buf, sizeof(buf), false)<=0) return false;
    buf[0] = reg+1;
    buf[1] = value & 0xFF;
    if (i2c_write_blocking(i2c_default, BNO055_ADDRESS, buf, sizeof(buf), false)<=0) return false;
    return true;
}


static uint8_t _bno055_read8(uint8_t reg)
{
    uint8_t v = 0x00;
    i2c_read_blocking(i2c_default, BNO055_ADDRESS, &v, sizeof(v), false);
    return v;
}


static int _bno055_read(uint8_t reg, uint8_t *buf, size_t len)
{
    int res;
    res = i2c_write_blocking(i2c_default, BNO055_ADDRESS, &reg, sizeof(reg), false);
    if (res<=0)
        return res;

    res = i2c_read_blocking(i2c_default, BNO055_ADDRESS, buf, len, false);
    return res;
}



static bool _bno055_detect()
{
    uint8_t chipId = _bno055_read8(BNO055_REG_CHIP_ID);
    return chipId == BNO055_ID;
}


void bno055_init()
{
    i2c_bus_acquire_blocking();
    if (!_bno055_detect()) {
        g_device_present = false;
        i2c_bus_release();
    }
    g_device_present = true;

    printf("Detected BNO055\n");

    uint8_t buf[8];

    _bno055_read(0, buf, sizeof(buf));
    printf("  ChipID: %02x\n", buf[BNO055_REG_CHIP_ID]);
    printf("   AccID: %02x\n", buf[BNO055_REG_ACC_ID]);
    printf("   MagID: %02x\n", buf[BNO055_REG_MAG_ID]);
    printf("   GyrID: %02x\n", buf[BNO055_REG_GYR_ID]);
    printf("  SW Rev: %04x\n", (uint16_t)buf[BNO055_REG_SW_REV_ID_LSB] | (uint16_t)buf[BNO055_REG_SW_REV_ID_MSB]<<8);
    printf("  BL Rev: %02x\n", (uint16_t)buf[BNO055_REG_BL_REV_ID]);


    // Switch to config mode
    _bno055_write_reg8(BNO055_REG_OPR_MODE, BNO055_OPR_MODE_CONFIG);

    // Reset the chip
    printf("Reset chip\n");
    _bno055_write_reg8(BNO055_REG_SYS_TRIGGER, BNO055_SYS_TRIGGER_RST_SYS);
    sleep_ms(50);
    while (_bno055_read8(BNO055_REG_CHIP_ID) != BNO055_ID) {
        sleep_ms(10);
    }
    sleep_ms(50);
    printf("\n");
    printf("Reset complete\n");

    // Set to normal power mode 
    _bno055_write_reg8(BNO055_REG_PWR_MODE, BNO055_PWR_MODE_SUSPEND);
    sleep_ms(10);



    i2c_bus_release();

#if 0
    while (true) {
        bno055_update();
        sleep_ms(1000);


    }
    #endif
}


void bno055_update()
{
    if (!i2c_bus_acquire_timeout_us(50)) 
        return;



    i2c_bus_release();
}
