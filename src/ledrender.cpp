#include "ledrender.h"

#include <limits>
#include <robot.h>
#include <led/animation_knightrider.h>
#include <led/animation_colorcycle.h>
#include <led/animation_blink.h>
#include <led/animation_solid.h>
#include <led/animation_chase.h>

LEDRender::LEDRender(LED::StripBase &strip, Robot &robot):
    m_strip { strip },
    m_robot { robot }, 
    m_mode  { Mode::BLACK }
{
    m_animations[static_cast<uint>(Mode::BLACK)] = std::make_unique<LED::Animation::Solid>(m_strip, LED::Color::BLACK);
    m_animations[static_cast<uint>(Mode::BLINK)] = std::make_unique<LED::Animation::Blink>(m_strip, LED::Color::MAGENTA*0.1, 100000);
    m_animations[static_cast<uint>(Mode::CHASE)] = std::make_unique<LED::Animation::Chase>(m_strip);
    m_animations[static_cast<uint>(Mode::COLOR_CYCLE)] = std::make_unique<LED::Animation::ColorCycle>(m_strip);
    m_animations[static_cast<uint>(Mode::KNIGHT_RIDER)] = std::make_unique<LED::Animation::KnightRider>(m_strip, 4, 24-8);
}




void LEDRender::init()
{
    m_animations[static_cast<uint>(m_mode)]->start();
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


void LEDRender::set_mode(Mode mode) 
{
    if (mode==m_mode)
        return;

    if (static_cast<size_t>(mode) >= static_cast<size_t>(Mode::_LAST)) 
        return;

    if (m_animations[static_cast<uint>(m_mode)]) {
        m_animations[static_cast<uint>(m_mode)]->stop();
    }

    m_mode = mode;
    if (m_animations[static_cast<uint>(m_mode)]) {
        m_animations[static_cast<uint>(m_mode)]->start();
    }
}

