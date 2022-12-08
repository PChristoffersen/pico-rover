#pragma once

#include "../animation.h"

namespace LED::Animation {

    class KnightRider : public Periodic {
        public:
            KnightRider(StripBase &strip, uint start, uint stop) : 
                Periodic { "Anim:Knight", strip, INTERVAL },
                m_start { start },
                m_stop { stop },
                m_pos { 0 }, 
                m_dir { 1 }
            {
            }

        protected:

            virtual void do_update() override 
            {
                m_strip.fill(Color::BLACK);
                m_strip[m_start+m_pos] = COLOR;
                m_strip.show();
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
            static constexpr interval_type INTERVAL { 150000 };
            static constexpr Color COLOR { Color::RED * 0.1 };
            const uint m_start;
            const uint m_stop;
            int m_pos;
            int m_dir;
    };

};
