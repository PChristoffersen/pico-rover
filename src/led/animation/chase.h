#pragma once

#include <array>
#include "../animation.h"


namespace LED::Animation {

    class Chase : public Periodic {
        public:
            Chase(layer_type &layer) : 
                Periodic { "Anim:Chase", layer, INTERVAL },
                m_pos { 0 },
                m_color_num { 0 }
            {
            }

        protected:

            virtual void do_update() override 
            {
                const uint len = m_layer.size();

                m_layer.fill(Color::RGB::BLACK);
                m_layer[m_pos] = COLORS[m_color_num];
                m_layer[m_pos+len/2] = COLORS[m_color_num];
                m_layer.setDirty(true);

                m_pos++;
                if (m_pos >= len/2) {
                    m_pos = 0;
                    m_color_num = (m_color_num+1) % count_of(COLORS);
                }
            }

        private:
            static constexpr interval_type INTERVAL { 50 };
            static constexpr Color::RGBW COLORS[] {
                Color::RGB::RED,
                Color::RGB::GREEN,
                Color::RGB::BLUE,
            };
            uint m_pos;
            uint m_color_num;

    };

};
