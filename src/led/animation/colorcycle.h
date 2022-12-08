#pragma once

#include <array>
#include "../animation.h"


namespace LED::Animation {

    class ColorCycle : public Periodic {
        public:
            ColorCycle(StripBase &strip) : 
                Periodic { "Anim:Cycle", strip, INTERVAL },
                m_hue { 0 } 
            {
            }

        protected:

            virtual void do_update() override 
            {
                const uint len = m_strip.length();
                for (auto i=0u; i<len; i++) {
                    m_strip[i] = Color::HSV { static_cast<std::uint8_t>(m_hue + i), 255u, 255u };
                }
                m_strip.show();

                m_hue++;
            }

        private:
            static constexpr interval_type INTERVAL { 50 };
            static constexpr float BRIGHTNESS { 0.5f };
            uint8_t m_hue;


    };

};
