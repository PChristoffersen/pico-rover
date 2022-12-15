#pragma once

#include <array>

#include "color.h"

namespace LED::Color {

    template<typename ColorType, size_t NCOLORS>
    class Buffer : public std::array<ColorType, NCOLORS> {
        public:

        template<typename SRC>
        void paint(const SRC &src, size_t off = 0) 
        {
            auto dst = this->begin()+off;
            for (const auto &s : src) {
                *dst << s;
            }
        }

    };

}
