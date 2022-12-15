#pragma once

#include <array>
#include "../animation.h"


namespace LED::Animation {

    class Solid : public Base {
        public:
            Solid(layer_type &layer, Color::RGBA color) : 
                Base { layer },
                m_color { color }
            {
            }

            virtual void start() override 
            {
                m_layer.fill(m_color);
                m_layer.setDirty(true);
            }
            virtual void stop() override
            {                
            }

        private:
            Color::RGBA m_color;

    };

};
