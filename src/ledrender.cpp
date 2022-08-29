#include "ledrender.h"

#include <limits>
#include <robot.h>
#include <led/animation_knightrider.h>
#include <led/animation_colorcycle.h>
#include <led/animation_blink.h>
#include <led/animation_solid.h>

LEDRender::LEDRender(LED::StripBase &strip, Robot &robot):
    m_strip { strip },
    m_robot { robot }, 
    m_mode  { std::numeric_limits<uint>::max() }
{
}




void LEDRender::init()
{
    set_mode(0);
}


void LEDRender::update_connected(bool connected)
{
    /*
    if (connected) {
        m_animation = std::make_unique<LED::Animation::KnightRider>(m_strip);
        m_animation->begin();
    }
    else {
        set_mode(99);
        m_animation = std::make_unique<LED::Animation::Blink>(m_strip, LED::Color::MAGENTA*0.1, 100000);
        m_animation->begin();
    }
    */
}


void LEDRender::set_mode(uint mode) 
{
    if (mode==m_mode)
        return;

    if (m_animation) {
        m_animation->stop();
    }

    m_mode = mode;
    switch (mode) {
        case 0:
            m_animation = std::make_unique<LED::Animation::Solid>(m_strip, LED::Color::BLACK);
            m_animation->start();
            break;
        case 1:
            m_animation = std::make_unique<LED::Animation::ColorCycle>(m_strip);
            m_animation->start();
            break;
        case 2:
            m_animation = std::make_unique<LED::Animation::KnightRider>(m_strip);
            m_animation->start();
            break;
        case 99:
            m_animation = std::make_unique<LED::Animation::Blink>(m_strip, LED::Color::MAGENTA*0.1, 100000);
            m_animation->start();
            break;
    }
}

