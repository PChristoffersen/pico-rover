#include "pico_adc.h"

#include <string.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <hardware/adc.h>

namespace Sensor {

static constexpr inline uint _adc_from_pin(uint pin)
{
    return pin-26;
}


PicoADC::PicoADC(uint battery_pin, float battery_r1, float battery_r2) :
    m_battery_pin { battery_pin },
    m_battery_r1 { battery_r1 },
    m_battery_r2 { battery_r2 },

    m_sem { nullptr },

    m_battery_voltage { 0.0f },
    m_vsys_voltage { 0.0f },
    m_temp { 0.0f }
{
    m_sem = xSemaphoreCreateMutexStatic(&m_sem_buf);
    assert(m_sem);
    xSemaphoreGive(m_sem);
}



inline void PicoADC::run()
{
    TickType_t last_time = xTaskGetTickCount();

    while (true) {
        auto input = adc_get_selected_input();
        auto result = adc_read();
        float adc_voltage =  (ADC_REF * result) / ADC_RESOLUTION;

        if (input==_adc_from_pin(m_battery_pin)) {
            _handle_battery(adc_voltage);
        }
        else if (input==_adc_from_pin(VSYS_PIN)) {
            _handle_vsys(adc_voltage);
        }
        else if (input==TEMP_ADC) {
            _handle_temp(adc_voltage);
        }

        xTaskDelayUntil(&last_time, pdMS_TO_TICKS(UPDATE_INTERVAL_MS));
    }
}


void PicoADC::init()
{
    adc_init();

    adc_set_temp_sensor_enabled(true);
    adc_gpio_init(m_battery_pin);
    adc_gpio_init(VSYS_PIN);

    
    adc_select_input(_adc_from_pin(m_battery_pin));
    adc_set_round_robin((1<<_adc_from_pin(m_battery_pin))|(1<<_adc_from_pin(VSYS_PIN))|(1<<TEMP_ADC));

    m_task = xTaskCreateStatic([](auto arg){ reinterpret_cast<PicoADC*>(arg)->run(); }, "PicoADC", TASK_STACK_SIZE, this, ADC_TASK_PRIORITY, m_task_stack, &m_task_buf);
    assert(m_task);
    vTaskSuspend(m_task);
}



void PicoADC::start()
{
    vTaskResume(m_task);
}



void PicoADC::_handle_battery(float adc_voltage)
{
    float voltage = adc_voltage * (m_battery_r1+m_battery_r2) / m_battery_r2; 
    if (voltage<BATTERY_MIN) {
        voltage = 0.0f;
    }
    //printf("Battery: %f V  (%f)\n", voltage, adc_voltage);

    // Store value
    xSemaphoreTake(m_sem, portMAX_DELAY);
    m_battery_voltage = voltage;
    xSemaphoreGive(m_sem);

    m_battery_cb(voltage);
}


void PicoADC::_handle_vsys(float adc_voltage)
{
    float voltage = adc_voltage * (VSYS_R1+VSYS_R2) / VSYS_R2; 
    //printf("VSys: %f V\n", voltage);

    // Store value
    xSemaphoreTake(m_sem, portMAX_DELAY);
    m_vsys_voltage = voltage;
    xSemaphoreGive(m_sem);

    m_vsys_cb(voltage);
}

void PicoADC::_handle_temp(float adc_voltage)
{
    float tempC = 27.0f - (adc_voltage - 0.706f) / 0.001721f;
    //printf("Temp: %.1f C\n", tempC);

    // Store value
    xSemaphoreTake(m_sem, portMAX_DELAY);
    m_temp = tempC;
    xSemaphoreGive(m_sem);

    m_temp_cb(tempC);
}


}
