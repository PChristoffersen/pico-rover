#include "motor.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/pwm.h>
#include <hardware/clocks.h>

#include "../boardconfig.h"



#define MAX_ENCODER_STEP_RATE 10000 

// Whether to use IN/IN or PH/EN mode (See https://www.pololu.com/product/4036) 
#define MOTOR_IN_IN_MODE 0


typedef struct {
    uint in1_pin;
    uint in2_pin;
    uint enca_pin;
    uint encb_pin;

    uint slice;

    bool enabled;
    int duty;
} motor_data_t;


static motor_data_t g_motors[] = {
    {
        .in1_pin = MOTOR1_IN1_PIN, 
        .in2_pin = MOTOR1_IN2_PIN,
        .enca_pin = MOTOR1_ENCA_PIN, 
        .encb_pin = MOTOR1_ENCB_PIN,
        .slice = 0,
        .enabled = false,
        .duty = 0,
    },
    {
        .in1_pin = MOTOR2_IN1_PIN, 
        .in2_pin = MOTOR2_IN2_PIN,
        .enca_pin = MOTOR2_ENCA_PIN, 
        .encb_pin = MOTOR2_ENCB_PIN,
        .slice = 0,
        .enabled = false,
        .duty = 0,
    },
    {
        .in1_pin = MOTOR3_IN1_PIN, 
        .in2_pin = MOTOR3_IN2_PIN,
        .enca_pin = MOTOR3_ENCA_PIN, 
        .encb_pin = MOTOR3_ENCB_PIN,
        .slice = 0,
        .enabled = false,
        .duty = 0,
    },
    {
        .in1_pin = MOTOR4_IN1_PIN, 
        .in2_pin = MOTOR4_IN2_PIN,
        .enca_pin = MOTOR4_ENCA_PIN, 
        .encb_pin = MOTOR4_ENCB_PIN,
        .slice = 0,
        .enabled = false,
        .duty = 0,

    },
};
#define N_MOTORS (sizeof(g_motors)/sizeof(g_motors[0]))



void motor_init()
{
    float div = clock_get_hz(clk_sys)/MOTOR_PWM_FREQUENCY/MOTOR_PWM_WRAP;

    gpio_init(MOTOR_ENABLE_PIN);
    gpio_set_dir(MOTOR_ENABLE_PIN, GPIO_OUT);
    gpio_put(MOTOR_ENABLE_PIN, 0);

    uint offset = pio_add_program(MOTOR_ENCODER_PIO, &quadrature_encoder_program);

    for (uint i=0; i<N_MOTORS; ++i) {
        motor_data_t *motor = &g_motors[i];
        gpio_init(motor->in1_pin);
        gpio_set_dir(motor->in1_pin, GPIO_OUT);
        gpio_put(motor->in1_pin, 0);
        gpio_init(motor->in2_pin);
        gpio_set_dir(motor->in2_pin, GPIO_OUT);
        gpio_put(motor->in2_pin, 0);

        gpio_set_function(motor->in1_pin, GPIO_FUNC_PWM);
        #if MOTOR_IN_IN_MODE
        gpio_set_function(motor->in2_pin, GPIO_FUNC_PWM);
        #endif

        motor->slice = pwm_gpio_to_slice_num(motor->in1_pin);
        pwm_set_wrap(motor->slice, MOTOR_PWM_WRAP);
        pwm_set_clkdiv(motor->slice, div);

        pwm_set_chan_level(motor->slice, PWM_CHAN_A, 0);
        pwm_set_chan_level(motor->slice, PWM_CHAN_B, 0);
        pwm_set_enabled(motor->slice, false);

        gpio_init(motor->enca_pin);
        gpio_set_dir(motor->enca_pin, GPIO_IN);
        gpio_init(motor->encb_pin);
        gpio_set_dir(motor->encb_pin, GPIO_IN);
        quadrature_encoder_program_init(MOTOR_ENCODER_PIO, i, offset, motor->enca_pin, MAX_ENCODER_STEP_RATE);
    }

    #if DEBUG_USE_MOTOR4
    printf("Debug is using motor4 IN1,IN2 as uart\n");
    #endif
}


void motor_set_drivers_enabled(bool enabled)
{
    gpio_put(MOTOR_ENABLE_PIN, enabled);
    #warning TODO - set all motor pwm to idle
}


void motor_set_supply_voltage(float voltage)
{
    #warning TODO 
}



static void _set_duty(motor_data_t *m, int duty)
{
    #if MOTOR_IN_IN_MODE
    if (duty>=0) {
        // Forward
        int pwm = duty;
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
    if (duty>=0) {
        printf("Set Duty: %5d >\n", duty);
        pwm_set_chan_level(m->slice, pwm_gpio_to_channel(m->in1_pin), duty);
        gpio_put(m->in2_pin, 1);
    }
    else {
        printf("Set Duty: %5d <\n", duty);
        pwm_set_chan_level(m->slice, pwm_gpio_to_channel(m->in1_pin), -duty);
        gpio_put(m->in2_pin, 0);
    }
    #endif
}


void motor_set_enabled(motor_t motor, bool enabled)
{
    motor_data_t *m = &g_motors[motor];
    if (m->enabled==enabled)
        return;

    m->enabled = enabled;

    if (enabled) {
        pwm_set_counter(m->slice, 0);
        _set_duty(m, m->duty);
        pwm_set_enabled(m->slice, true);
    }   
    else {
        // Stop PWM and set both output pins low
        //  - IN/IN mode this sets the motor in coast mode 
        //  - PH/EN set the motor to break low
        pwm_set_enabled(m->slice, false);
        gpio_put(m->in1_pin, 0);
        gpio_put(m->in2_pin, 0);
    } 
}


void motor_put(motor_t motor, float duty)
{
    motor_data_t *m = &g_motors[motor];

    int pwm_duty = duty*MOTOR_PWM_MAX;

    if (m->duty==pwm_duty)
        return;

    m->duty = pwm_duty;
    if (!m->enabled) 
        return;

    _set_duty(m, pwm_duty);
}
