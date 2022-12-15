#pragma once

#include <pico/stdlib.h>

#include "colorbuffer.h"

namespace LED::Color {

    class LayerBase {
        public:
            LayerBase() : LayerBase(0) {}
            LayerBase(size_t offset);

            bool dirty() const { return m_dirty; }
            void setDirty(bool dirty) { m_dirty = dirty; }

            bool visible() const { return m_visible; }
            void setVisible(bool visible) { m_visible = visible; }

            size_t offset() const { return m_offset; }

        private:
            bool m_dirty;
            bool m_visible;
            size_t m_offset;
    };

    template<typename ColorType, size_t NCOLORS>
    class Layer : public LayerBase, public Buffer<ColorType, NCOLORS> {
        public:
            Layer() : LayerBase { } {}
            Layer(size_t offset) : LayerBase { offset } {}

    };

}
