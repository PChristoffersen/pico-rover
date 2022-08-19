#pragma once

#include <memory>
#include <pico/stdlib.h>
#include <led/strip.h>
#include <led/animation.h>

class LEDRender {
    public:
        LEDRender(LED::StripBase &strip, class Robot &robot);
        LEDRender(const LEDRender&) = delete; // No copy constructor
        LEDRender(LEDRender&&) = delete; // No move constructor

        void begin();
        absolute_time_t update();

        void update_connected(bool connected);

    private:
        LED::StripBase &m_strip;

        class Robot &m_robot;

        std::unique_ptr<LED::Animation::Base> m_animation;
};