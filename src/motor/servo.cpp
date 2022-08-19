#include "servo.h"

#include <pico/stdlib.h>
#include <hardware/clocks.h>
#include <hardware/pwm.h>

#include <util/debug.h>
#include <util/locking.h>


namespace Motor {



Servo::Servo(id_type id, uint pin) :
    m_id { id },
    m_pin { pin },
    m_slice { 0 },
    m_channel { 0 },
    m_enabled  { false },
    m_value { INITIAL_POSITION }
{
    mutex_init(&m_mutex);
}


void Servo::init()
{
    #ifndef DEBUG_USE_SERVO_PINS
    gpio_init(m_pin);
    gpio_set_dir(m_pin, GPIO_OUT);
    gpio_put(m_pin, 0);
    
    gpio_set_function(m_pin, GPIO_FUNC_PWM);
    #endif

    m_slice = pwm_gpio_to_slice_num(m_pin);
    m_channel = pwm_gpio_to_channel(m_pin);

    pwm_set_wrap(m_slice,  PWM_WRAP);
    pwm_set_clkdiv(m_slice, clock_get_hz(clk_sys)/PWM_DIV);

    pwm_set_chan_level(m_slice, m_channel, 0);

    pwm_set_enabled(m_slice, true);
}



void Servo::set_enabled(bool enabled)
{
    MUTEX_GUARD(m_mutex);
    if (m_enabled==enabled) return;
    m_enabled = enabled;
    #ifndef DEBUG_USE_SERVO_PINS
    if (enabled) {
        pwm_set_chan_level(m_slice, m_channel, m_value);
    }
    else {
        pwm_set_chan_level(m_slice, m_channel, 0);
    }
    #endif
}


void Servo::put(uint16_t us) 
{
    MUTEX_GUARD(m_mutex);
    if (m_value!=us) {
        m_value = us;
        if (m_enabled) {
            pwm_set_chan_level(m_slice, m_channel, m_value);
        }
    }
}

}
