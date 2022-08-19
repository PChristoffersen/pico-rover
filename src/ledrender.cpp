#include "ledrender.h"

#include <robot.h>
#include <led/animation_knightrider.h>
#include <led/animation_colorcycle.h>
#include <led/animation_blink.h>
#include <led/animation_solid.h>

LEDRender::LEDRender(LED::StripBase &strip, Robot &robot):
    m_strip { strip },
    m_robot { robot }
{
}


void LEDRender::begin()
{
    if (m_animation) {
        m_animation->begin();
    }
}

absolute_time_t LEDRender::update()
{
    if (m_animation) {
        return m_animation->update();
    }
    return make_timeout_time_ms(60000);
}


void LEDRender::update_connected(bool connected)
{
    if (connected) {
        m_animation = std::make_unique<LED::Animation::ColorCycle>(m_strip);
        m_animation->begin();
    }
    else {
        m_animation = std::make_unique<LED::Animation::Blink>(m_strip, LED::Color::MAGENTA*0.1, 100000);
        m_animation->begin();
    }
}
