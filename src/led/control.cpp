#include "control.h"

#include <limits>
#include "animation/knightrider.h"
#include "animation/colorcycle.h"
#include "animation/blink.h"
#include "animation/solid.h"
#include "animation/chase.h"

namespace LED {

Control::Control(Robot &robot):
    #ifdef RASPBERRYPI_PICO_W
    #else    
    m_builtin { PICO_DEFAULT_LED_PIN },
    #endif
    m_strip { LED_STRIP_PIO, LED_STRIP_PIN, LED_STRIP_IS_RGBW },
    m_robot { robot },
    m_mode  { Mode::BLACK }
{
    m_animations[static_cast<uint>(Mode::BLACK)] = std::make_unique<Animation::Solid>(m_strip, Color::BLACK);
    m_animations[static_cast<uint>(Mode::BLINK)] = std::make_unique<Animation::Blink>(m_strip, Color::MAGENTA*0.1, 100000);
    m_animations[static_cast<uint>(Mode::CHASE)] = std::make_unique<Animation::Chase>(m_strip);
    m_animations[static_cast<uint>(Mode::COLOR_CYCLE)] = std::make_unique<Animation::ColorCycle>(m_strip);
    m_animations[static_cast<uint>(Mode::KNIGHT_RIDER)] = std::make_unique<Animation::KnightRider>(m_strip, 4, 24-8);
}




void Control::init()
{
    m_builtin.init();
    m_strip.init();

    m_animations[static_cast<uint>(m_mode)]->start();
}


void Control::update_connected(bool connected)
{
    /*
    if (connected) {
        m_animation = std::make_unique<Animation::KnightRider>(m_strip);
        m_animation->begin();
    }
    else {
        set_mode(99);
        m_animation = std::make_unique<Animation::Blink>(m_strip, Color::MAGENTA*0.1, 100000);
        m_animation->begin();
    }
    */
}


void Control::set_mode(Mode mode) 
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

}