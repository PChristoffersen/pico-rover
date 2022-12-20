#pragma once

#include <array>
#include "../animation.h"


namespace LED::Animation {

    template<size_t NLEDS>
    class Chase : public Periodic<NLEDS> {
        public:
            using interval_type = typename Periodic<NLEDS>::interval_type;
            using layer_type = typename Periodic<NLEDS>::layer_type;
            using Periodic<NLEDS>::m_layer;

            Chase(layer_type &layer) : 
                Periodic<NLEDS> { layer, INTERVAL },
                m_pos { 0 },
                m_color_num { 0 }
            {
            }

        protected:

            virtual void do_update() override 
            {
                const uint len = m_layer.size();

                m_layer.fill(Color::RGBA::TRANSPARENT);
                m_layer[m_pos] = COLORS[m_color_num];
                m_layer[m_pos+len/2] = COLORS[m_color_num];
                m_layer.dirty();

                m_pos++;
                if (m_pos >= len/2) {
                    m_pos = 0;
                    m_color_num = (m_color_num+1) % count_of(COLORS);
                }
            }

        private:
            static constexpr interval_type INTERVAL { pdMS_TO_TICKS(50) };
            static constexpr Color::RGBW COLORS[] {
                Color::RGB::RED,
                Color::RGB::GREEN,
                Color::RGB::BLUE,
            };
            uint m_pos;
            uint m_color_num;

    };

};
