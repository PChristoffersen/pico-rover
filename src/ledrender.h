#pragma once

#include <memory>
#include <array>
#include <pico/stdlib.h>
#include <led/strip.h>
#include <led/animation.h>

class LEDRender {
    public:
        enum class Mode : uint {
            BLACK,
            BLINK,
            CHASE,
            COLOR_CYCLE,
            KNIGHT_RIDER,
            _LAST
        };

        LEDRender(LED::StripBase &strip, class Robot &robot);
        LEDRender(const LEDRender&) = delete; // No copy constructor
        LEDRender(LEDRender&&) = delete; // No move constructor

        void init();

        void update_connected(bool connected);

        void set_mode(Mode mode);

    private:
        static constexpr size_t NUM_ANIMATIONS { static_cast<size_t>(Mode::_LAST) };
        using animations_type = std::array<std::unique_ptr<LED::Animation::Base>, NUM_ANIMATIONS>;

        LED::StripBase &m_strip;

        class Robot &m_robot;

        Mode m_mode;

        animations_type m_animations;
};