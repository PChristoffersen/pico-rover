#include "board_adc.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <hardware/adc.h>

#include "../boardconfig.h"
#include "../radio/receiver.h"




#define UPDATE_INTERVAL 250000u

#define ADC_REF 3.3f
#define ADC_RESOLUTION (1u<<12)

#define VSYS_PIN 29
#define VSYS_ADC 3
#define VSYS_R1 200000.0f
#define VSYS_R2 100000.0f

#define TEMP_ADC 4

static absolute_time_t g_last_update;

static mutex_t g_mutex;

static float g_voltage_battery = 0.0f;
static float g_voltage_vsys = 0.0f;
static float g_temp = 0.0f;


void board_adc_init()
{
    adc_init();

    adc_set_temp_sensor_enabled(true);
    adc_gpio_init(BATTERY_SENSE_PIN);
    adc_gpio_init(VSYS_PIN);

    adc_select_input(BATTERY_SENSE_ADC);
    adc_set_round_robin((1<<BATTERY_SENSE_ADC)|(1<<VSYS_ADC)|(1<<TEMP_ADC));

    g_last_update = get_absolute_time();
    mutex_init(&g_mutex);
}


static void _handle_battery(float adc_voltage)
{
    float voltage = adc_voltage * (BATTERY_SENSE_R1+BATTERY_SENSE_R2) / BATTERY_SENSE_R2; 
    if (voltage<BATTERY_MIN_VOLTAGE) {
        voltage = 0.0f;
    }
    //printf("Battery: %f V  (%f)\n", voltage, adc_voltage);

    // Store value
    mutex_enter_blocking(&g_mutex);
    g_voltage_battery = voltage;
    mutex_exit(&g_mutex);

    // Push radio telemetry event
    radio_telemetry_t event;
    radio_telemetry_cells(&event, 0x00, 0, 2, voltage/2.0f, voltage/2.0f);
    radio_receiver_telemetry_push(&event);
}


static void _handle_vsys(float adc_voltage)
{
    float voltage = adc_voltage * (VSYS_R1+VSYS_R2) / VSYS_R2; 
    //printf("VSys: %f V\n", vsys_voltage);

    // Store value
    mutex_enter_blocking(&g_mutex);
    g_voltage_vsys = voltage;
    mutex_exit(&g_mutex);

    // Push radio telemetry event
    radio_telemetry_t event;
    radio_telemetry_a3(&event, 0, voltage);
    radio_receiver_telemetry_push(&event);
}


static void _handle_temp(float adc_voltage)
{
    float tempC = 27.0f - (adc_voltage - 0.706f) / 0.001721f;
    //printf("Temp: %.1f C\n", tempC);

    // Store value
    mutex_enter_blocking(&g_mutex);
    g_temp = tempC;
    mutex_exit(&g_mutex);

    // Push radio telemetry event
    radio_telemetry_t event;
    radio_telemetry_temperature1(&event, 0, tempC);
    radio_receiver_telemetry_push(&event);
}


void board_adc_update()
{
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(g_last_update, now) > UPDATE_INTERVAL) {
        uint input = adc_get_selected_input();
        uint16_t result = adc_read();
        float adc_voltage =  (ADC_REF * result) / ADC_RESOLUTION;

        switch (input) {
        case BATTERY_SENSE_ADC:
            _handle_battery(adc_voltage);
            break;
        case VSYS_ADC:
            _handle_vsys(adc_voltage);
            break;
        case TEMP_ADC:
            _handle_temp(adc_voltage);
            break;

        }

        g_last_update = now;
    }

}


float board_adc_battery_get()
{
    float value;
    mutex_enter_blocking(&g_mutex);
    value = g_voltage_battery;
    mutex_exit(&g_mutex);
    return value;
}


float board_adc_vsys_get()
{
    float value;
    mutex_enter_blocking(&g_mutex);
    value = g_voltage_vsys;
    mutex_exit(&g_mutex);
    return value;
}


float board_adc_temp_get()
{
    float value;
    mutex_enter_blocking(&g_mutex);
    value = g_temp;
    mutex_exit(&g_mutex);
    return value;
}
