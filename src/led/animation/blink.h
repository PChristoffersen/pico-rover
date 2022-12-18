#pragma once

#include <array>
#include "../animation.h"


namespace LED::Animation {

    class Blink : public Periodic {
        public:
            Blink(layer_type &layer, Color::RGB color, interval_type interval) : 
                Periodic { layer, interval },
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
