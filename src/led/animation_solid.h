#pragma once

#include <array>
#include "animation.h"


namespace LED::Animation {

    class Solid : public Base {
        public:
            Solid(StripBase &strip, Color color) : 
                Base { strip },
                m_color { color }
            {
            }

            virtual void begin() override 
            {
                m_strip.fill(m_color);
                m_strip.show();
            }

            virtual absolute_time_t update() override 
            {
                return make_timeout_time_ms(60000);
            }

        private:
            Color m_color;

    };

};
