#pragma once

#include <array>
#include "animation.h"


namespace LED::Animation {

    class ColorCycle : public Periodic {
        public:
            ColorCycle(StripBase &strip) : 
                Periodic { "Anim:Cycle", strip, INTERVAL },
                m_state { 0 } 
            {
            }

        protected:

            virtual void do_update() override 
            {
                const uint len = m_strip.length();
                for (auto i = 0u; i<len; ++i) {
                    m_strip[len-i-1] = COLORS[((m_state+i)/4) % (count_of(COLORS))];
                }
                m_strip.show();

                m_state = (m_state+1) % (count_of(COLORS)*m_strip.length());
            }

        private:
            static constexpr interval_type INTERVAL { 100000 };
            static constexpr float BRIGHTNESS { 0.1f };
            static constexpr Color COLORS[] {
                Color::RED * BRIGHTNESS,
                Color::GREEN * BRIGHTNESS,
                Color::BLUE * BRIGHTNESS,
                Color::WHITE * BRIGHTNESS,
                Color::YELLOW * BRIGHTNESS,
                Color::MAGENTA * BRIGHTNESS,
                Color::CYAN * BRIGHTNESS,
                //Color::BLACK,
            };
            uint m_state;


    };

};
