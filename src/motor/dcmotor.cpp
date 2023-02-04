#include "dcmotor.h"

#include <algorithm>
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <hardware/pwm.h>
#include <hardware/clocks.h>

#include "../boardconfig.h"
#include <util/battery.h>

namespace Motor {

float DCMotor::m_supply_voltage = battery_max();
uint DCMotor::m_enable_count = 0u;
bool DCMotor::m_global_enabled = false;



DCMotor::DCMotor(id_type id, uint in1_pin, uint in2_pin, PIO enc_pio, uint enca_pin, uint encb_pin, bool invert) :
    m_id { id },
    m_in1_pin { in1_pin },
    m_in2_pin { in2_pin },
    m_invert  { invert },
    m_enabled { false },
    m_duty { 0.0f },
    m_encoder { id, enc_pio, enca_pin, encb_pin, invert }
{
}


void DCMotor::global_init()
{
    static bool initialized = false;
    if (initialized)
        return;

    m_enable_count = 0;

    gpio_init(MOTOR_ENABLE_PIN);
    gpio_set_dir(MOTOR_ENABLE_PIN, GPIO_OUT);
    gpio_put(MOTOR_ENABLE_PIN, 0);

    initialized = true;
}


void DCMotor::global_enable(bool enable)
{
    taskENTER_CRITICAL();
    if (enable) {
        m_enable_count++;
        if (m_global_enabled) {
            gpio_put(MOTOR_ENABLE_PIN, true);
        }
    }
    else {
        assert(m_enable_count>0);
        m_enable_count--;
        if (m_enable_count==0) {
            gpio_put(MOTOR_ENABLE_PIN, false);
        }
    }
    taskEXIT_CRITICAL();
}


void DCMotor::set_global_enable(bool enable) 
{
    taskENTER_CRITICAL();
    if (enable!=m_global_enabled) {
        m_global_enabled = enable;
        if (m_enable_count>0) {
            if (m_global_enabled) {
                gpio_put(MOTOR_ENABLE_PIN, true);
            }
            else {
                gpio_put(MOTOR_ENABLE_PIN, false);
            }
        }
    }
    taskEXIT_CRITICAL();
}


void DCMotor::init()
{
    global_init();

    float div = clock_get_hz(clk_sys)/PWM_FREQUENCY/PWM_WRAP;

    gpio_init(m_in1_pin);
    gpio_set_dir(m_in1_pin, GPIO_OUT);
    gpio_put(m_in1_pin, 0);
    gpio_init(m_in2_pin);
    gpio_set_dir(m_in2_pin, GPIO_OUT);
    gpio_put(m_in2_pin, 0);

    gpio_set_function(m_in1_pin, GPIO_FUNC_PWM);
    if constexpr (DRIVER_MODE==DriverMode::IN_IN) {
        gpio_set_function(m_in2_pin, GPIO_FUNC_PWM);
    }

    m_slice = pwm_gpio_to_slice_num(m_in1_pin);
    pwm_set_wrap(m_slice, PWM_WRAP);
    pwm_set_clkdiv(m_slice, div);

    pwm_set_chan_level(m_slice, PWM_CHAN_A, 0);
    pwm_set_chan_level(m_slice, PWM_CHAN_B, 0);
    pwm_set_enabled(m_slice, false);

    m_encoder.init();
}



void DCMotor::update_duty()
{
    // Calculate PWM level based on supply voltage and requested duty
    taskENTER_CRITICAL();
    int pwm = m_duty*PWM_WRAP*VOLTAGE_TARGET/m_supply_voltage;
    taskEXIT_CRITICAL();

    if constexpr (DRIVER_MODE==DriverMode::IN_IN) {
        // IN/IN Mode - in1 and in2 functions as described in https://www.pololu.com/product/4036
        // TODO Not implemented correctly
        if (m_duty>=0) {
            // Forward
            //printf("IN/IN Set Duty: %f >  %5d  %5lu\n", m_duty, pwm, PWM_WRAP-pwm);
            pwm_set_chan_level(m_slice, pwm_gpio_to_channel(m_in1_pin), 0);
            pwm_set_chan_level(m_slice, pwm_gpio_to_channel(m_in2_pin), pwm);
        }
        else {
            // Reverse
            //int pwm = MOTOR_PWM_MAX+pwm;
            //printf("Set Duty: %f >  %5d  %5lu\n", m_duty, pwm, PWM_WRAP-pwm);
            pwm_set_chan_level(m_slice, pwm_gpio_to_channel(m_in1_pin), PWM_WRAP-pwm);
            pwm_set_chan_level(m_slice, pwm_gpio_to_channel(m_in2_pin), 0);
        }
    }
    else {
        // PH/EN Mode - in2 constrols direction, in1 speed

        if (m_duty>=0) {
            //printf("PH/EN Set Duty: %5d %f >\n", pwm, m_duty);
            pwm_set_chan_level(m_slice, pwm_gpio_to_channel(m_in1_pin), pwm);
            gpio_put(m_in2_pin, 1);
        }
        else {
            //printf("Set Duty: %5d %f<\n", pwm, m_duty);
            pwm_set_chan_level(m_slice, pwm_gpio_to_channel(m_in1_pin), -pwm);
            gpio_put(m_in2_pin, 0);
        }
    }
}


void DCMotor::set_enabled(bool enabled)
{
    LOCK_GUARD();
    
    if (m_enabled==enabled) 
        return;

    m_enabled = enabled;

    if (enabled) {
        pwm_set_counter(m_slice, 0);
        update_duty();
        pwm_set_enabled(m_slice, true);
    }   
    else {
        // Stop PWM and set both output pins low
        //  - IN/IN mode this sets the motor in coast mode 
        //  - PH/EN set the motor to break low
        pwm_set_enabled(m_slice, false);
        gpio_put(m_in1_pin, 0);
        gpio_put(m_in2_pin, 0);
    } 
    global_enable(m_enabled);
}


void DCMotor::set_duty(duty_type duty)
{
    LOCK_GUARD();

    duty = std::clamp(duty, -1.0f, 1.0f);
    if (m_invert) {
        duty = -duty;
    }

    if (m_duty==duty) 
        return;

    m_duty = duty;
    if (!m_enabled) 
        return;

    update_duty();
}


void DCMotor::set_supply_voltage(float voltage)
{
    constexpr auto vmin { battery_min() };
    constexpr auto vmax { battery_max() };
    
    taskENTER_CRITICAL();
    m_supply_voltage = std::clamp(voltage, vmin, vmax);
    taskEXIT_CRITICAL();
}


}
