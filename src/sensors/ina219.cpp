#include "ina219.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/i2c.h>

#include <util/i2c_bus.h>

namespace Sensor {

static constexpr uint8_t INA219_REG_CONFIG       = 0x00; // config register address
static constexpr uint8_t INA219_REG_SHUNTVOLTAGE = 0x01; // shunt voltage register
static constexpr uint8_t INA219_REG_BUSVOLTAGE   = 0x02; // bus voltage register
static constexpr uint8_t INA219_REG_POWER        = 0x03; // power register
static constexpr uint8_t INA219_REG_CURRENT      = 0x04; // current register
static constexpr uint8_t INA219_REG_CALIBRATION  = 0x05; // calibration register


static constexpr uint16_t INA219_CONFIG_RESET              = 1<<15; // Reset bit
static constexpr uint16_t INA219_CONFIG_BRNG_16V           = 0<<13; // 16V Bus Voltage Range
static constexpr uint16_t INA219_CONFIG_BRNG_32V           = 1<<13; // 32V Bus Voltage Range
static constexpr uint16_t INA219_CONFIG_PGA_1_40           = 0b00<<11; // GAIN 1 Range 40mV
static constexpr uint16_t INA219_CONFIG_PGA_2_80           = 0b01<<11; // GAIN 2 Range 80mV
static constexpr uint16_t INA219_CONFIG_PGA_3_160          = 0b10<<11; // GAIN 3 Range 160mV
static constexpr uint16_t INA219_CONFIG_PGA_4_320          = 0b11<<11; // GAIN 4 Range 320mV
static constexpr uint16_t INA219_CONFIG_BADCRES_9BIT        = (0b0000<<7); //    84us        9-bit bus res = 0..511         
static constexpr uint16_t INA219_CONFIG_BADCRES_10BIT       = (0b0001<<7); //   148us       10-bit bus res = 0..1023         
static constexpr uint16_t INA219_CONFIG_BADCRES_11BIT       = (0b0010<<7); //   276us       11-bit bus res = 0..2047
static constexpr uint16_t INA219_CONFIG_BADCRES_12BIT       = (0b0011<<7); //   532us       12-bit bus res = 0..4097
static constexpr uint16_t INA219_CONFIG_BADCRES_12BIT_      = (0b1000<<7); //   532us       12-bit bus res = 0..4097
static constexpr uint16_t INA219_CONFIG_BADCRES_12BIT_2S    = (0b1001<<7); //  1.06ms   2 x 12-bit bus samples averaged together
static constexpr uint16_t INA219_CONFIG_BADCRES_12BIT_4S    = (0b1010<<7); //  2.13ms   4 x 12-bit bus samples averaged together
static constexpr uint16_t INA219_CONFIG_BADCRES_12BIT_8S    = (0b1011<<7); //  4.26ms   8 x 12-bit bus samples averaged together
static constexpr uint16_t INA219_CONFIG_BADCRES_12BIT_16S   = (0b1100<<7); //  8.51ms  16 x 12-bit bus samples averaged together
static constexpr uint16_t INA219_CONFIG_BADCRES_12BIT_32S   = (0b1101<<7); // 17.02ms  32 x 12-bit bus samples averaged together
static constexpr uint16_t INA219_CONFIG_BADCRES_12BIT_64S   = (0b1110<<7); // 34.05ms  64 x 12-bit bus samples averaged together
static constexpr uint16_t INA219_CONFIG_BADCRES_12BIT_128S  = (0b1111<<7); // 68.10ms 128 x 12-bit bus samples averaged together
static constexpr uint16_t INA219_CONFIG_SADCRES_9BIT        = (0b0000<<3); //    84us        9-bit bus res = 0..511         
static constexpr uint16_t INA219_CONFIG_SADCRES_10BIT       = (0b0001<<3); //   148us       10-bit bus res = 0..1023         
static constexpr uint16_t INA219_CONFIG_SADCRES_11BIT       = (0b0010<<3); //   276us       11-bit bus res = 0..2047
static constexpr uint16_t INA219_CONFIG_SADCRES_12BIT       = (0b0011<<3); //   532us       12-bit bus res = 0..4097
static constexpr uint16_t INA219_CONFIG_SADCRES_12BIT_      = (0b1000<<3); //   532us       12-bit bus res = 0..4097
static constexpr uint16_t INA219_CONFIG_SADCRES_12BIT_2S    = (0b1001<<3); //  1.06ms   2 x 12-bit bus samples averaged together
static constexpr uint16_t INA219_CONFIG_SADCRES_12BIT_4S    = (0b1010<<3); //  2.13ms   4 x 12-bit bus samples averaged together
static constexpr uint16_t INA219_CONFIG_SADCRES_12BIT_8S    = (0b1011<<3); //  4.26ms   8 x 12-bit bus samples averaged together
static constexpr uint16_t INA219_CONFIG_SADCRES_12BIT_16S   = (0b1100<<3); //  8.51ms  16 x 12-bit bus samples averaged together
static constexpr uint16_t INA219_CONFIG_SADCRES_12BIT_32S   = (0b1101<<3); // 17.02ms  32 x 12-bit bus samples averaged together
static constexpr uint16_t INA219_CONFIG_SADCRES_12BIT_64S   = (0b1110<<3); // 34.05ms  64 x 12-bit bus samples averaged together
static constexpr uint16_t INA219_CONFIG_SADCRES_12BIT_128S  = (0b1111<<3); // 68.10ms 128 x 12-bit bus samples averaged together
static constexpr uint16_t INA219_CONFIG_MODE_POWERDOWN            = 0b000; // Power-down
static constexpr uint16_t INA219_CONFIG_MODE_SVOLT_TRIGGERED      = 0b001; // Shunt voltage, triggered
static constexpr uint16_t INA219_CONFIG_MODE_BVOLT_TRIGGERED      = 0b010; // Bus voltage, triggered
static constexpr uint16_t INA219_CONFIG_MODE_SANDBVOLT_TRIGGERED  = 0b011; // Shunt and bus, triggered
static constexpr uint16_t INA219_CONFIG_MODE_ADCOFF               = 0b100; // ADC off (disabled)
static constexpr uint16_t INA219_CONFIG_MODE_SVOLT_CONTINUOUS     = 0b101; // Shunt voltage, continuous
static constexpr uint16_t INA219_CONFIG_MODE_BVOLT_CONTINUOUS     = 0b110; // Bus voltage, continuous
static constexpr uint16_t INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS = 0b111; // Shunt and bus, continuous


#define INA219_VBUS_VOLTAGE(value)           (value>>3) // Bus voltage
#define INA219_VBUS_CNVR(value)          ((value>>1)&1) // Conversion Ready
#define INA219_VBUS_OVF(value)                (value&1) // Math Overflow Flag

static inline uint16_t _ina219_bus_voltage(uint16_t value)
{
    return value>>3;
}
static inline bool _ina219_conversion_ready(uint16_t value)
{
    return static_cast<bool>((value>>1)&1);
}
static inline bool _ina219_overflow(uint16_t value)
{
    return static_cast<bool>(value&1);
}



INA219::INA219(Address addr) :
    m_address { static_cast<addr_t>(addr) },
    m_present { false },
    m_shunt_v { 0.0f },
    m_bus_v { 0.0f },
    m_current { 0.0f },
    m_power { 0.0f },
    m_callback { nullptr }
{
    mutex_init(&m_mutex);
}

bool INA219::write_reg(uint8_t reg, uint16_t value)
{
    uint8_t rval[3] = { reg, static_cast<uint8_t>(value>>8), static_cast<uint8_t>(value&0xFF)};
    int res = i2c_write_blocking(i2c_default, m_address, rval, sizeof(rval), false);
    if (res!=sizeof(rval)) {
        return false;
    }
    return true;
}


bool INA219::read_reg(uint8_t reg, uint16_t &value)
{
    int res;
    uint8_t rval[2] = { 0x00, 0x00 };
    res =i2c_write_blocking(i2c_default, m_address, &reg, sizeof(reg), false);
    if (res<=0) return false;
    res =i2c_read_blocking(i2c_default, m_address, rval, sizeof(rval), false);
    if (res<=0) return false;
    value = static_cast<uint16_t>(rval[0]<<8) | (rval[1]);
    return true;
}



void INA219::init()
{
    i2c_bus_acquire_blocking();

    // Detect the chip
    uint16_t reg;
    if (!read_reg(INA219_REG_CONFIG, reg)) {
        m_present = false;
        i2c_bus_release();
        return;
    }
    m_present = true;

    // Reset the chip
    write_reg(INA219_REG_CONFIG, INA219_CONFIG_RESET);

    uint16_t config = 0;
    config |= INA219_CONFIG_BRNG_16V; // Voltage range
    config |= INA219_CONFIG_PGA_4_320; // Gain
    config |= INA219_CONFIG_BADCRES_12BIT_8S; // Bus ADC
    config |= INA219_CONFIG_SADCRES_12BIT_128S; // Shunt ADC
    config |= INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS; // Mode

    write_reg(INA219_REG_CONFIG, config);

    m_last_update = get_absolute_time();

    i2c_bus_release();
}


absolute_time_t INA219::update()
{
    if (!m_present) {
        return make_timeout_time_ms(60000);
    }
    auto now = get_absolute_time();
    if (absolute_time_diff_us(m_last_update, now)>UPDATE_INTERVAL) {
        if (!i2c_bus_acquire_timeout_us(50)) {
            return delayed_by_us(now, 100);
        }


        uint16_t shunt_reg;
        uint16_t bus_reg;

        if (!read_reg(INA219_REG_SHUNTVOLTAGE, shunt_reg)) goto bail;
        if (!read_reg(INA219_REG_BUSVOLTAGE, bus_reg)) goto bail;


        // Calculate
        mutex_enter_blocking(&m_mutex);
        m_shunt_v = static_cast<int16_t>((int16_t)((~shunt_reg) + 1) * -1) * 0.01f;  // Shunt voltage is 10 uV per bit
        m_bus_v = _ina219_bus_voltage(bus_reg) * 4 * 0.001f; // Bus voltage is 4 mV per bit
        m_current = m_shunt_v / SHUNT_RESISTOR;
        m_power = m_current * m_bus_v;
        mutex_exit(&m_mutex);

        #if 0
        printf("Shunt voltage : %f V\n", m_shunt_v);
        printf("  Bus voltage : %f V\n", m_bus_v);
        printf("      Current : %f mA\n", m_current);
        printf("        Power : %f mW\n", m_power);
        printf("\n");
        #endif

        if (m_callback) {
            m_callback(m_bus_v, m_current, m_power);
        }

      bail:
        i2c_bus_release();
        m_last_update = delayed_by_us(m_last_update, UPDATE_INTERVAL);
    }
    return delayed_by_us(m_last_update, UPDATE_INTERVAL);
}


}
