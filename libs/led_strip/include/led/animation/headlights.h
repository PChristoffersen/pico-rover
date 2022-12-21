#pragma once

#include <array>
#include "../animation.h"


namespace LED::Animation {

    template<size_t NLEDS>
    class Headlights : public BaseTemplate<NLEDS> {
        public:
            using layer_type = typename BaseTemplate<NLEDS>::layer_type;
            using BaseTemplate<NLEDS>::m_layer;

            Headlights(layer_type &layer) : 
                BaseTemplate<NLEDS> { layer }
            {
            }

            virtual void start(TickType_t now) override 
            {
                draw();
            }
            virtual void stop() override
            {                
            }

        private:
            static constexpr Color::RGBA COLOR_FRONT { Color::RGB::WHITE };
            static constexpr Color::RGBA COLOR_REAR { Color::RGB::RED };
            static constexpr size_t LED_COUNT { 4 };

            void draw()
            {
                auto lsz = m_layer.size();

                m_layer.fill(Color::RGBA::TRANSPARENT);
                for (uint i=0; i<LED_COUNT; i++) {
                    m_layer[i] = COLOR_FRONT;
                    m_layer[lsz/2-i-1] = COLOR_FRONT;
                    m_layer[lsz/2+i] = COLOR_REAR;
                    m_layer[lsz-i-1] = COLOR_FRONT;
                }
                m_layer.dirty();
            }
    };

};
