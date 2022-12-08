#pragma once

#include <array>
#include "../animation.h"


namespace LED::Animation {

    class Chase : public Periodic {
        public:
            Chase(StripBase &strip) : 
                Periodic { "Anim:Chase", strip, INTERVAL },
                m_pos { 0 },
                m_color_num { 0 }
            {
            }

        protected:

            virtual void do_update() override 
            {
                const uint len = m_strip.length();

                m_strip.fill(Color::BLACK);
                m_strip[m_pos] = COLORS[m_color_num];
                m_strip[m_pos+len/2] = COLORS[m_color_num];
                m_strip.show();

                m_pos++;
                if (m_pos >= len/2) {
                    m_pos = 0;
                    m_color_num = (m_color_num+1) % count_of(COLORS);
                }
            }

        private:
            static constexpr interval_type INTERVAL { 50 };
            static constexpr Color COLORS[] {
                Color::RED,
                Color::GREEN,
                Color::BLUE,
            };
            uint m_pos;
            uint m_color_num;

    };

};
