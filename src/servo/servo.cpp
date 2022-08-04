#include "servo.h"

#include <pico/stdlib.h>
#include <hardware/clocks.h>
#include <hardware/pwm.h>

#include "../boardconfig.h"


/* PWM Parameters to setup the hardware to pulse every 20ms
 * 
 * We setup the hardware so the channel level directly corresponds to 
 * the servo pulse length in microseconds.
 */
static constexpr uint16_t SERVO_PWM_WRAP = 20000u;
static constexpr float SERVO_PWM_DIV = 1000000.0f;


Servo::Servo(uint pin) :
    m_pin { pin },
    m_enabled  { false },
    m_value { INITIAL_POSITION }
{
}


void Servo::init()
{
    gpio_init(m_pin);
    gpio_set_dir(m_pin, GPIO_OUT);
    gpio_put(m_pin, 0);

    m_slice = pwm_gpio_to_slice_num(m_pin);
    m_channel = pwm_gpio_to_channel(m_pin);

    pwm_set_wrap(m_slice,  SERVO_PWM_WRAP);
    pwm_set_clkdiv(m_slice, clock_get_hz(clk_sys)/SERVO_PWM_DIV);

    pwm_set_chan_level(m_slice, m_channel, 0);

    gpio_set_function(m_pin, GPIO_FUNC_PWM);
    pwm_set_enabled(m_slice, true);
}



void Servo::set_enabled(bool enabled)
{
    if (m_enabled==enabled) return;
    m_enabled = enabled;
    if (enabled) {
        pwm_set_chan_level(m_slice, m_channel, m_value);
    }
    else {
        pwm_set_chan_level(m_slice, m_channel, 0);
    }

}


void Servo::put(uint16_t us) 
{
    if (m_value!=us) {
        m_value = us;
        if (m_enabled) {
            pwm_set_chan_level(m_slice, m_channel, m_value);
        }
    }
}
