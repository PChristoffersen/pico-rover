#include "ina219.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/i2c.h>

#include <i2c_bus.h>
#include "ina219_regs.h"

namespace INA219 {


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



Sensor::Sensor(addr_type addr, UBaseType_t task_priority) :
    m_address { addr },
    m_task_priority { task_priority },
    m_present { false },
    m_task { nullptr },
    m_shunt_v { 0.0f },
    m_bus_v { 0.0f },
    m_current { 0.0f },
    m_power { 0.0f }
{
    m_sem = xSemaphoreCreateMutexStatic(&m_sem_buf);
    assert(m_sem);
    xSemaphoreGive(m_sem);
}


bool Sensor::write_reg(uint8_t reg, uint16_t value)
{
    uint8_t rval[3] = { reg, static_cast<uint8_t>(value>>8), static_cast<uint8_t>(value&0xFF)};
    int res = i2c_write_blocking(i2c_default, m_address, rval, sizeof(rval), false);
    if (res!=sizeof(rval)) {
        return false;
    }
    return true;
}


bool Sensor::read_reg(uint8_t reg, uint16_t &value)
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



inline void Sensor::run()
{
    TickType_t last_time = xTaskGetTickCount();
    uint16_t shunt_reg = 0;
    uint16_t bus_reg = 0;

    while (true) {
        i2c_bus_acquire_blocking();
        read_reg(INA219_REG_SHUNTVOLTAGE, shunt_reg);
        read_reg(INA219_REG_BUSVOLTAGE, bus_reg);
        i2c_bus_release();


        // Calculate
        xSemaphoreTake(m_sem, portMAX_DELAY);
        m_shunt_v = static_cast<int16_t>((int16_t)((~shunt_reg) + 1) * -1) * 0.01f;  // Shunt voltage is 10 uV per bit
        m_bus_v = _ina219_bus_voltage(bus_reg) * 4 * 0.001f; // Bus voltage is 4 mV per bit
        m_current = m_shunt_v / SHUNT_RESISTOR;
        m_power = m_current * m_bus_v;
        xSemaphoreGive(m_sem);

        #if 0
        printf("Shunt voltage : %f V\n", m_shunt_v);
        printf("  Bus voltage : %f V\n", m_bus_v);
        printf("      Current : %f mA\n", m_current);
        printf("        Power : %f mW\n", m_power);
        printf("\n");
        #endif

        on_data(m_bus_v, m_current, m_power);


        xTaskDelayUntil(&last_time, pdMS_TO_TICKS(UPDATE_INTERVAL_MS));
    }
}



void Sensor::init()
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

    i2c_bus_release();

    m_task = xTaskCreateStatic([](auto arg){ reinterpret_cast<Sensor*>(arg)->run(); }, "INA219", TASK_STACK_SIZE, this, m_task_priority, m_task_stack, &m_task_buf);
    assert(m_task);
    vTaskSuspend(m_task);
}


void Sensor::start()
{
    vTaskResume(m_task);
}

}
