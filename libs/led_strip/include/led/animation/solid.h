#pragma once

#include <array>
#include "../animation.h"


namespace LED::Animation {

    template<size_t NLEDS>
    class Solid : public BaseTemplate<NLEDS> {
        public:
            using layer_type = typename BaseTemplate<NLEDS>::layer_type;
            using BaseTemplate<NLEDS>::m_layer;

            Solid(layer_type &layer, Color::RGBA color) : 
                BaseTemplate<NLEDS> { layer },
                m_color { color }
            {
            }

            virtual void start(TickType_t now) override 
            {
                m_layer.fill(m_color);
                m_layer.dirty();
            }
            virtual void stop() override
            {                
            }

        private:
            Color::RGBA m_color;

    };

};
