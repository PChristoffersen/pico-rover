#include "servo.h"

#include <pico/stdlib.h>
#include <hardware/clocks.h>
#include <hardware/pwm.h>

#include "../boardconfig.h"


#define SERVO_INITIAL_POS 1500u

typedef struct {
    uint pin;

    uint slice;
    uint channel;

    bool enabled;
    uint value;
} servo_data_t;


static servo_data_t g_servos[] = {
    {
        .pin = SERVO1_PIN,
        .slice = 0, .channel = 0,
        .enabled = false,
        .value = SERVO_INITIAL_POS
    },
    {
        .pin = SERVO2_PIN,
        .slice = 0, .channel = 0,
        .enabled = false,
        .value = SERVO_INITIAL_POS
    },
};
#define N_SERVOS (sizeof(g_servos)/sizeof(g_servos[0]))


/* PWM Parameters to setup the hardware to pulse every 20ms
 * 
 * We setup the hardware so the channel level directly corresponds to 
 * the servo pulse length in microseconds.
 */
#define SERVO_PWM_WRAP 20000u
#define SERVO_PWM_DIV 1000000.0f




static void __servo_init_slice(uint slice_num)
{
    pwm_set_wrap(slice_num,  SERVO_PWM_WRAP);
    pwm_set_clkdiv(slice_num, clock_get_hz(clk_sys)/SERVO_PWM_DIV);

    pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 0);

    pwm_set_enabled(slice_num, true);
}



void servo_init()
{
    gpio_init(SERVO1_PIN);
    gpio_set_dir(SERVO1_PIN, GPIO_OUT);
    gpio_put(SERVO1_PIN, 0);

    gpio_init(SERVO2_PIN);
    gpio_set_dir(SERVO2_PIN, GPIO_OUT);
    gpio_put(SERVO2_PIN, 0);

    for (uint i=0; i<N_SERVOS; i+=2) {
        g_servos[i].slice = pwm_gpio_to_slice_num(g_servos[i].pin);
        g_servos[i].channel = pwm_gpio_to_channel(g_servos[i].pin);
        g_servos[i+1].slice = pwm_gpio_to_slice_num(g_servos[i+1].pin);
        g_servos[i+1].channel = pwm_gpio_to_channel(g_servos[i+1].pin);
        assert(g_servos[i].slice==g_servos[i+1].slice);
        assert(g_servos[i].channel!=g_servos[i+1].channel);

        __servo_init_slice(g_servos[i].slice);
        gpio_set_function(g_servos[i].pin, GPIO_FUNC_PWM);
        gpio_set_function(g_servos[i+1].pin, GPIO_FUNC_PWM);
    }
}



void servo_set_enabled(servo_t servo, bool enabled)
{
    servo_data_t *s = &g_servos[servo];
    if (s->enabled==enabled) return;
    s->enabled = enabled;
    if (enabled) {
        pwm_set_chan_level(s->slice, s->channel, s->value);
    }
    else {
        pwm_set_chan_level(s->slice, s->channel, 0);
    }

}


void servo_put(servo_t servo, uint16_t us) 
{
    servo_data_t *s = &g_servos[servo];
    if (s->value!=us) {
        s->value = us;
        if (s->enabled) {
            pwm_set_chan_level(s->slice, s->channel, s->value);
        }
    }

}
