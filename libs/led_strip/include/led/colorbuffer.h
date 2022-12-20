#pragma once

#include <array>
#if PICO_NO_HARDWARE
#include <iostream>
#include <iomanip>
#endif

#include <led/color.h>

namespace LED::Color {

    template<typename ColorType, size_t NCOLORS>
    class Buffer : public std::array<ColorType, NCOLORS> {
        public:

        template<typename SOURCE>
        void operator<<(const SOURCE &src)
        {
            auto dst = this->begin();
            for (const auto &s : src) {
                *dst << s;
                dst++;
            }
        }

        #if PICO_NO_HARDWARE
        template<typename ColorType_, size_t NCOLORS_>
        friend std::ostream &operator<<(std::ostream &os, const Buffer<ColorType_, NCOLORS_> &self);
        #endif
    };

    #if PICO_NO_HARDWARE
    template<typename ColorType_, size_t NCOLORS_>
    std::ostream &operator<<(std::ostream &os, const Buffer<ColorType_, NCOLORS_> &self)
    {
        for (uint i=0; i<self.size(); i++) {
            if (i==self.size()/2) {
                os << std::endl;
            }
            os << self[i] << " ";
        }
        os << std::endl;
        return os;
    }
    #endif

}
