#include "led.h"

#include <stdio.h>
#include <pico/stdlib.h>

#include "../boardconfig.h"



LED::LED(uint pin) :
    m_pin { pin },
    m_blinking { false }
{
}


void LED::init()
{
    gpio_init(m_pin);
    gpio_set_dir(m_pin, GPIO_OUT);
    gpio_put(m_pin, true);
}


void LED::on()
{
    gpio_put(m_pin, 1);
    m_blinking = false;
}


void LED::off()
{
    gpio_put(m_pin, false);
    m_blinking = false;
}


void LED::blink()
{
    m_blink_last = get_absolute_time();
    m_blinking = true;
}


absolute_time_t LED::update()
{
    if (!m_blinking)
        return make_timeout_time_ms(60000);

    static bool state = false;
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(m_blink_last, now)>BLINK_INTERVAL) {
        state = !state;
        if (state) {
            gpio_put(m_pin, 1);
        }
        else {
            gpio_put(m_pin, 0);
        }
        m_blink_last = now;
    }
    return delayed_by_us(m_blink_last, BLINK_INTERVAL);
}
