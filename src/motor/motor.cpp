#include "motor.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <hardware/pwm.h>
#include <hardware/clocks.h>

#include "../boardconfig.h"
#include "../util.h"

#define CLAMP(x, lower, upper) (MIN(upper, MAX(x, lower)))

#define MAX_ENCODER_STEP_RATE 10000 

// Whether to use IN/IN or PH/EN mode (See https://www.pololu.com/product/4036) 
#define MOTOR_IN_IN_MODE 0


float Motor::m_supply_voltage = MOTOR_SUPPLY_MAX_VOLTAGE;
uint Motor::m_encoder_program_offset = 0;


Motor::Motor(uint in1_pin, uint in2_pin, uint enca_pin, uint encb_pin) :
    m_in1_pin { in1_pin },
    m_in2_pin { in2_pin },
    m_enca_pin { enca_pin },
    m_enabled { false },
    m_duty { 0.0f }
{
}


void Motor::global_init()
{
    static bool initialized = false;
    if (initialized)
        return;

    gpio_init(MOTOR_ENABLE_PIN);
    gpio_set_dir(MOTOR_ENABLE_PIN, GPIO_OUT);
    gpio_put(MOTOR_ENABLE_PIN, 0);

    m_encoder_program_offset = pio_add_program(MOTOR_ENCODER_PIO, &quadrature_encoder_program);

    initialized = true;
}


void Motor::init()
{
    static uint next_encoder_idx = 0;

    mutex_init(&m_mutex);

    global_init();

    float div = clock_get_hz(clk_sys)/MOTOR_PWM_FREQUENCY/MOTOR_PWM_WRAP;

    gpio_init(m_in1_pin);
    gpio_set_dir(m_in1_pin, GPIO_OUT);
    gpio_put(m_in1_pin, 0);
    gpio_init(m_in2_pin);
    gpio_set_dir(m_in2_pin, GPIO_OUT);
    gpio_put(m_in2_pin, 0);

    gpio_set_function(m_in1_pin, GPIO_FUNC_PWM);
    #if MOTOR_IN_IN_MODE
    gpio_set_function(m_in2_pin, GPIO_FUNC_PWM);
    #endif

    m_slice = pwm_gpio_to_slice_num(m_in1_pin);
    pwm_set_wrap(m_slice, MOTOR_PWM_WRAP);
    pwm_set_clkdiv(m_slice, div);

    pwm_set_chan_level(m_slice, PWM_CHAN_A, 0);
    pwm_set_chan_level(m_slice, PWM_CHAN_B, 0);
    pwm_set_enabled(m_slice, false);

    gpio_init(m_enca_pin);
    gpio_set_dir(m_enca_pin, GPIO_IN);
    gpio_init(m_encb_pin);
    gpio_set_dir(m_encb_pin, GPIO_IN);
    quadrature_encoder_program_init(MOTOR_ENCODER_PIO, next_encoder_idx++, m_encoder_program_offset, m_enca_pin, MAX_ENCODER_STEP_RATE);
}



void Motor::update_duty()
{
    // Calculate PWM level based on supply voltage and requested duty
    int pwm = m_duty*MOTOR_PWM_WRAP*MOTOR_TARGET_VOLTAGE/m_supply_voltage;

    #if MOTOR_IN_IN_MODE
    if (duty>=0) {
        // Forward
        printf("Set Duty: %5d >  %5d  %5d\n", duty, pwm, MOTOR_PWM_WRAP-pwm);
        pwm_set_chan_level(m->slice, pwm_gpio_to_channel(m->in1_pin), 0);
        pwm_set_chan_level(m->slice, pwm_gpio_to_channel(m->in2_pin), pwm);
    }
    else {
        // Reverse
        int pwm = MOTOR_PWM_MAX+duty;
        printf("Set Duty: %5d >  %5d  %5d\n", duty, pwm, MOTOR_PWM_WRAP-pwm);
        pwm_set_chan_level(m->slice, pwm_gpio_to_channel(m->in1_pin), MOTOR_PWM_WRAP-pwm);
        pwm_set_chan_level(m->slice, pwm_gpio_to_channel(m->in2_pin), 0);
    }
    #else
    if (m_duty>=0) {
        printf("Set Duty: %5d %f >\n", pwm, m_duty);
        pwm_set_chan_level(m_slice, pwm_gpio_to_channel(m_in1_pin), pwm);
        gpio_put(m_in2_pin, 1);
    }
    else {
        printf("Set Duty: %5d %f<\n", pwm, m_duty);
        pwm_set_chan_level(m_slice, pwm_gpio_to_channel(m_in1_pin), -pwm);
        gpio_put(m_in2_pin, 0);
    }
    #endif
}


void Motor::set_enabled(bool enabled)
{
    MUTEX_GUARD(m_mutex);

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

}


void Motor::put(float duty)
{
    MUTEX_GUARD(m_mutex);

    duty = CLAMP(duty, -1.0f, 1.0f);

    if (m_duty==duty) 
        return;

    m_duty = duty;
    if (!m_enabled) 
        return;

    update_duty();
}


void Motor::set_supply_voltage(float voltage)
{
    m_supply_voltage = voltage;
}
