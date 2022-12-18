#pragma once

#include "../animation.h"

namespace LED::Animation {

    class Indicators : public Periodic {
        public:
            static constexpr interval_type INTERVAL { 150 };
            static constexpr size_t OFF_INTERVALS { 3 };
            static constexpr size_t ON_INTERVALS { 2 };
            static constexpr size_t LED_COUNT { 4 };
            static constexpr size_t TOTAL_INTERVALS { OFF_INTERVALS + LED_COUNT + ON_INTERVALS };
            static constexpr Color::RGBA COLOR { 0xFF, 0x80, 0x00, 0xFF };

            enum class Mode {
                NONE,
                LEFT, 
                RIGHT, 
                HAZARD
            };

            Indicators(layer_type &layer) : 
                Periodic { layer, INTERVAL },
                m_mode { Mode::NONE },
                m_state { 0 }
            {
            }

            Mode get_mode() const { return m_mode; }
            void set_mode(Mode mode) 
            {
                if (mode==m_mode)
                    return;
                m_mode = mode;
                switch (m_mode) {
                    case Mode::NONE:
                        m_layer.fill(Color::RGBA::TRANSPARENT);
                        m_layer.dirty();
                        m_layer.set_visible(false);
                        m_state = 0;
                        break;
                    case Mode::LEFT:
                        m_layer.set_visible(true);
                        right(0);
                        break;
                    case Mode::RIGHT:
                        m_layer.set_visible(true);
                        left(0);
                        break;
                    case Mode::HAZARD:
                        m_layer.set_visible(true);
                        break;

                }
            }

            uint get_state() const { return m_state; }

        protected:

            void left(uint pos) 
            {
                auto lsz = m_layer.size();
                for (uint i=0; i<LED_COUNT; i++) {
                    if (i+OFF_INTERVALS>=pos) {
                        m_layer[lsz/2-LED_COUNT+i] = Color::RGBA::TRANSPARENT;
                        m_layer[LED_COUNT-i-1+lsz/2] = Color::RGBA::TRANSPARENT;
                    }
                    else {
                        m_layer[lsz/2-LED_COUNT+i] = COLOR;
                        m_layer[LED_COUNT-i-1+lsz/2] = COLOR;
                    }
                }
            }

            void right(uint pos)
            {
                auto lsz = m_layer.size();
                for (uint i=0; i<LED_COUNT; i++) {
                    if (i+OFF_INTERVALS>=pos) {
                        m_layer[LED_COUNT-i-1] = Color::RGBA::TRANSPARENT;
                        m_layer[lsz-LED_COUNT+i] = Color::RGBA::TRANSPARENT;
                    }
                    else {
                        m_layer[LED_COUNT-i-1] = COLOR;
                        m_layer[lsz-LED_COUNT+i] = COLOR;
                    }
                }
            }


            virtual void do_update() override 
            {
                switch (m_mode) {
                    case Mode::NONE:
                        return;
                    case Mode::LEFT:
                        left(m_state);
                        break;
                    case Mode::RIGHT:
                        right(m_state);
                        break;
                    case Mode::HAZARD:
                        left(m_state);
                        right(m_state);
                        break;
                }
                m_layer.dirty();
                m_state++;
                if (m_state>TOTAL_INTERVALS) {
                    m_state = 0;
                }
            }

        private:
            Mode m_mode;
            uint m_state;
    };

};
