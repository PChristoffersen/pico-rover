#pragma once

#include <pico/stdlib.h>

#include "colorbuffer.h"

namespace LED::Color {

    class LayerBase {
        public:
            LayerBase() : 
                m_dirty { false },
                m_visible { false }
            {}

            bool is_dirty() const { return m_dirty; }
            void set_dirty(bool dirty) { m_dirty = dirty; }
            void dirty() { set_dirty(true); }
            void clear_dirty() { set_dirty(false); }

            bool is_visible() const { return m_visible; }
            void set_visible(bool visible) { m_visible = visible; }

        private:
            bool m_dirty;
            bool m_visible;
    };

    template<typename ColorType, size_t NCOLORS>
    class Layer : public LayerBase, public Buffer<ColorType, NCOLORS> {
        public:
            Layer() : LayerBase { } {}

    };

}
