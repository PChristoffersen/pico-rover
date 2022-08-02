#include "ina219.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/i2c.h>


#define INA219_I2C_ADDRESS1     (0x40) // I2C ADDRESS 1  A0 = 0  A1 = 0
#define INA219_I2C_ADDRESS2     (0x41) // I2C ADDRESS 2  A0 = 1  A1 = 0
#define INA219_I2C_ADDRESS3     (0x44) // I2C ADDRESS 3  A0 = 0  A1 = 1
#define INA219_I2C_ADDRESS4     (0x45) // I2C ADDRESS 4  A0 = 1  A1 = 1

#define INA219_REG_CONFIG       (0x00) // config register address
#define INA219_REG_SHUNTVOLTAGE (0x01) // shunt voltage register
#define INA219_REG_BUSVOLTAGE   (0x02) // bus voltage register
#define INA219_REG_POWER        (0x03) // power register
#define INA219_REG_CURRENT      (0x04) // current register
#define INA219_REG_CALIBRATION  (0x05) // calibration register



typedef struct {
    uint8_t addr;
} ina219_t;


static ina219_t g_main_sensor = {
    .addr = INA219_I2C_ADDRESS1,
};


static bool _ina219_write_reg(ina219_t *dev, uint8_t reg, uint16_t value)
{
    uint8_t rval[2] = { (value>>8), (value&0xFF)};
    i2c_write_blocking(i2c_default, dev->addr, &reg, sizeof(reg), true);
    i2c_write_blocking(i2c_default, dev->addr, rval, sizeof(rval), false);
    return true;
}


static bool _ina219_read_reg(ina219_t *dev, uint8_t reg, uint16_t *value)
{
    uint8_t rval[2] = { 0x00, 0x00 };
    i2c_write_blocking(i2c_default, dev->addr, &reg, sizeof(reg), true);
    i2c_read_blocking(i2c_default, dev->addr, rval, sizeof(rval), false);
    *value = (rval[0]<<8) | (rval[1]);
    return true;
}


static bool _ina219_detect(ina219_t *dev)
{
    uint16_t reg = 0x0000;
    bool success = _ina219_read_reg(dev, INA219_REG_CONFIG, &reg);
    printf("INA219 Detect : %04x %d\n", reg, success);
    return success;
}




static void _ina219_init(ina219_t *dev)
{
}


void ina219_init()
{
    printf("Detecting ina219\n");
    bool found = _ina219_detect(&g_main_sensor);
    if (found) {
        printf("Found\n");
    }

}


void ina219_update()
{

}
