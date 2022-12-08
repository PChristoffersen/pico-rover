#pragma once

#include <memory>
#include <array>
#include <pico/stdlib.h>

#include <boardconfig.h>
#include "strip.h"
#include "single.h"
#include "animation.h"

class Robot;

namespace LED {

    class Control {
        public:
            #ifdef RASPBERRYPI_PICO_W
            using led_type = LED::Dummy;
            #else
            using led_type = LED::Single;
            #endif
            using strip_type = Strip<LED_STRIP_PIXEL_COUNT>;

            enum class Mode : uint {
                BLACK,
                BLINK,
                CHASE,
                COLOR_CYCLE,
                KNIGHT_RIDER,
                _LAST
            };

            Control(Robot &robot);
            Control(const Control&) = delete; // No copy constructor
            Control(Control&&) = delete; // No move constructor

            void init();

            void update_connected(bool connected);

            void set_mode(Mode mode);


            led_type &builtin() { return m_builtin; };
            strip_type &strip() { return m_strip; }

        private:
            static constexpr size_t NUM_ANIMATIONS { static_cast<size_t>(Mode::_LAST) };
            using animations_type = std::array<std::unique_ptr<Animation::Base>, NUM_ANIMATIONS>;

            led_type   m_builtin;
            strip_type m_strip;

            Robot &m_robot;

            Mode m_mode;

            animations_type m_animations;
    };

}