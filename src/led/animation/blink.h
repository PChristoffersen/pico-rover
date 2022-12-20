#pragma once

#include <array>
#include "../animation.h"


namespace LED::Animation {

    template<size_t NLEDS>
    class Blink : public Periodic<NLEDS> {
        public:
            using interval_type = typename Periodic<NLEDS>::interval_type;
            using layer_type = typename Periodic<NLEDS>::layer_type;
            using Periodic<NLEDS>::m_layer;

            Blink(layer_type &layer, Color::RGB color, interval_type interval) : 
                Periodic<NLEDS> { layer, interval },
                m_color { color },
                m_state { false } 
            {
            }

        protected:

            virtual void do_update() override 
            {
                if (m_state)
                    m_layer.fill(m_color);
                else
                    m_layer.fill(Color::RGBA::TRANSPARENT);
                m_layer.dirty();

                m_state = !m_state;
            }

        private:
            Color::RGB m_color;
            bool m_state;


    };

};
