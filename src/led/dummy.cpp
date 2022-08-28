#include "dummy.h"

#include <stdio.h>
#include <pico/stdlib.h>

namespace LED {

Dummy::Dummy() :
    m_blinking { false },
    m_interval { DEFAULT_BLINK_INTERVAL }
{
}


void Dummy::init()
{
}


void Dummy::on()
{
    m_blinking = false;
}


void Dummy::off()
{
    m_blinking = false;
}


bool Dummy::blink()
{
    if (m_blinking) 
        return true;
    m_blink_last = get_absolute_time();
    m_blinking = true;
    return false;
}


absolute_time_t Dummy::update()
{
    return make_timeout_time_ms(60000);
}


}