#pragma once

#include <array>
#include "animation.h"


namespace LED::Animation {

    class Blink : public Periodic {
        public:
            Blink(StripBase &strip, Color color, interval_type interval) : 
                Periodic { "Anim:Blink", strip, interval },
                m_color { color },
                m_state { false } 
            {
            }

        protected:

            virtual void do_update() override 
            {
                if (m_state)
                    m_strip.fill(m_color);
                else
                    m_strip.fill(Color::BLACK);
                m_strip.show();

                m_state = !m_state;
            }

        private:
            Color m_color;
            bool m_state;


    };

};
