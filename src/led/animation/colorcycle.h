#pragma once

#include <array>
#include "../animation.h"


namespace LED::Animation {

    template<size_t NLEDS>
    class ColorCycle : public Periodic<NLEDS> {
        public:
            using interval_type = typename Periodic<NLEDS>::interval_type;
            using layer_type = typename Periodic<NLEDS>::layer_type;
            using Periodic<NLEDS>::m_layer;

            ColorCycle(layer_type &layer) : 
                Periodic<NLEDS> { layer, INTERVAL },
                m_hue { 0 } 
            {
            }

        protected:

            virtual void do_update() override 
            {
                const uint len = m_layer.size();
                for (auto i=0u; i<len; i++) {
                    m_layer[i] = Color::HSV { static_cast<std::uint8_t>(m_hue + HUE_SPACE*i), 255u, 255u };
                }
                m_layer.dirty();
                m_hue+= HUE_ADD;
            }

        private:
            static constexpr interval_type INTERVAL { pdMS_TO_TICKS(10) };
            static constexpr uint8_t HUE_SPACE { 4u };
            static constexpr uint8_t HUE_ADD { 1u };
            uint8_t m_hue;


    };

};
