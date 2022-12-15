#pragma once

#include "../animation.h"

namespace LED::Animation {

    class KnightRider : public Periodic {
        public:
            KnightRider(layer_type &layer, uint start, uint stop) : 
                Periodic { "Anim:Knight", layer, INTERVAL },
                m_start { start },
                m_stop { stop },
                m_pos { 0 }, 
                m_dir { 1 }
            {
            }

        protected:

            virtual void do_update() override 
            {
                m_layer.fill(Color::RGB::BLACK);
                m_layer[m_start+m_pos] = COLOR;
                m_layer.setDirty(true);

                m_pos += m_dir;
                if (m_pos>=static_cast<int>(m_stop-m_start-1)) {
                    m_pos = m_stop-m_start-1;
                    m_dir = -m_dir;
                }
                else if (m_pos<=0) {
                    m_pos = 0;
                    m_dir = -m_dir;
                }
            }

        private:
            static constexpr interval_type INTERVAL { 50 };
            static constexpr Color::RGBA COLOR { Color::RGB::RED * 0.1 };
            const uint m_start;
            const uint m_stop;
            int m_pos;
            int m_dir;
    };

};
