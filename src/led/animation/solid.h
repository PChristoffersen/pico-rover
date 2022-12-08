#pragma once

#include <array>
#include "../animation.h"


namespace LED::Animation {

    class Solid : public Base {
        public:
            Solid(StripBase &strip, Color color) : 
                Base { strip },
                m_color { color }
            {
            }

            virtual void start() override 
            {
                m_strip.fill(m_color);
                m_strip.show();
            }
            virtual void stop() override
            {                
            }

        private:
            Color m_color;

    };

};
