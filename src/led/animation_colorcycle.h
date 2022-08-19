#pragma once

#include <array>
#include "animation.h"


namespace LED::Animation {

    class ColorCycle : public Periodic {
        public:
            ColorCycle(StripBase &strip) : 
                Periodic { strip, INTERVAL },
                m_state { 0 } 
            {
            }

        protected:

            virtual void do_update() override 
            {
                m_strip.fill(COLORS[m_state]);
                m_strip.show();

                m_state = (m_state+1) % count_of(COLORS);
            }

        private:
            static constexpr interval_type INTERVAL { 250000 };
            static constexpr float BRIGHTNESS { 0.1f };
            static constexpr Color COLORS[] {
                Color::RED * BRIGHTNESS,
                Color::BLUE * BRIGHTNESS,
                Color::GREEN * BRIGHTNESS,
                Color::YELLOW * BRIGHTNESS,
                Color::MAGENTA * BRIGHTNESS,
                Color::CYAN * BRIGHTNESS,
                Color::WHITE * BRIGHTNESS,
                Color::BLACK,
            };
            uint m_state;


    };

};
