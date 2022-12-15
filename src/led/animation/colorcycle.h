#pragma once

#include <array>
#include "../animation.h"


namespace LED::Animation {

    class ColorCycle : public Periodic {
        public:
            ColorCycle(layer_type &layer) : 
                Periodic { "Anim:Cycle", layer, INTERVAL },
                m_hue { 0 } 
            {
            }

        protected:

            virtual void do_update() override 
            {
                const uint len = m_layer.size();
                for (auto i=0u; i<len; i++) {
                    m_layer[i] = Color::HSV { static_cast<std::uint8_t>(m_hue + i), 255u, 255u };
                }
                m_layer.setDirty(true);
                m_hue++;
            }

        private:
            static constexpr interval_type INTERVAL { 50 };
            static constexpr float BRIGHTNESS { 0.5f };
            uint8_t m_hue;


    };

};
