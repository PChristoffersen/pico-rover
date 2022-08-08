#pragma once

#include <oled/font.h>

namespace OLED::Resource::Font {

    static constexpr ::OLED::Font::column_type _Fixed_5x8_data[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x5c, 0x00, 0x00, 
        0x00, 0x0c, 0x00, 0x0c, 0x00, 
        0x28, 0x7c, 0x28, 0x7c, 0x28, 
        0x00, 0x50, 0xec, 0x28, 0x00, 
        0x44, 0x2a, 0x34, 0x58, 0x24, 
        0x20, 0x58, 0x54, 0x24, 0x50, 
        0x00, 0x00, 0x06, 0x00, 0x00, 
        0x00, 0x38, 0x44, 0x00, 0x00, 
        0x00, 0x44, 0x38, 0x00, 0x00, 
        0x00, 0x54, 0x38, 0x54, 0x00, 
        0x00, 0x10, 0x38, 0x10, 0x00, 
        0x00, 0x80, 0x40, 0x00, 0x00, 
        0x08, 0x08, 0x08, 0x08, 0x00, 
        0x00, 0x00, 0x40, 0x00, 0x00, 
        0x00, 0x60, 0x18, 0x04, 0x00, 
        0x38, 0x44, 0x44, 0x38, 0x00, 
        0x00, 0x08, 0x7c, 0x00, 0x00, 
        0x48, 0x64, 0x54, 0x48, 0x00, 
        0x44, 0x54, 0x54, 0x28, 0x00, 
        0x20, 0x30, 0x28, 0x7c, 0x00, 
        0x5c, 0x54, 0x54, 0x24, 0x00, 
        0x38, 0x54, 0x54, 0x20, 0x00, 
        0x04, 0x64, 0x14, 0x0c, 0x00, 
        0x28, 0x54, 0x54, 0x28, 0x00, 
        0x08, 0x54, 0x54, 0x38, 0x00, 
        0x00, 0x00, 0x50, 0x00, 0x00, 
        0x00, 0x80, 0x50, 0x00, 0x00, 
        0x00, 0x10, 0x28, 0x44, 0x00, 
        0x00, 0x28, 0x28, 0x28, 0x00, 
        0x00, 0x44, 0x28, 0x10, 0x00, 
        0x00, 0x54, 0x14, 0x08, 0x00, 
        0x38, 0x44, 0x54, 0x54, 0x08, 
        0x78, 0x14, 0x14, 0x78, 0x00, 
        0x7c, 0x54, 0x54, 0x28, 0x00, 
        0x38, 0x44, 0x44, 0x44, 0x00, 
        0x7c, 0x44, 0x44, 0x38, 0x00, 
        0x7c, 0x54, 0x54, 0x44, 0x00, 
        0x7c, 0x14, 0x14, 0x04, 0x00, 
        0x38, 0x44, 0x44, 0x68, 0x00, 
        0x7c, 0x10, 0x10, 0x7c, 0x00, 
        0x00, 0x44, 0x7c, 0x44, 0x00, 
        0x30, 0x40, 0x40, 0x3c, 0x00, 
        0x7c, 0x10, 0x28, 0x44, 0x00, 
        0x7c, 0x40, 0x40, 0x40, 0x00, 
        0x7c, 0x10, 0x10, 0x7c, 0x00, 
        0x7c, 0x08, 0x10, 0x7c, 0x00, 
        0x38, 0x44, 0x44, 0x38, 0x00, 
        0x7c, 0x14, 0x14, 0x08, 0x00, 
        0x38, 0x44, 0x44, 0xb8, 0x00, 
        0x7c, 0x14, 0x14, 0x68, 0x00, 
        0x48, 0x54, 0x54, 0x24, 0x00, 
        0x04, 0x04, 0x7c, 0x04, 0x04, 
        0x3c, 0x40, 0x40, 0x3c, 0x00, 
        0x1c, 0x60, 0x60, 0x1c, 0x00, 
        0x1c, 0x60, 0x18, 0x60, 0x1c, 
        0x4c, 0x30, 0x10, 0x6c, 0x00, 
        0x00, 0x1c, 0x60, 0x1c, 0x00, 
        0x64, 0x54, 0x4c, 0x44, 0x00, 
        0x00, 0x7c, 0x44, 0x00, 0x00, 
        0x00, 0x0c, 0x30, 0x40, 0x00, 
        0x00, 0x44, 0x7c, 0x00, 0x00, 
        0x00, 0x08, 0x04, 0x08, 0x00, 
        0x80, 0x80, 0x80, 0x80, 0x80, 
        0x00, 0x04, 0x08, 0x00, 0x00, 
        0x00, 0x68, 0x28, 0x70, 0x00, 
        0x7e, 0x48, 0x48, 0x30, 0x00, 
        0x00, 0x30, 0x48, 0x48, 0x00, 
        0x30, 0x48, 0x48, 0x7c, 0x00, 
        0x30, 0x58, 0x58, 0x50, 0x00, 
        0x10, 0x78, 0x14, 0x04, 0x00, 
        0x10, 0xa8, 0xa8, 0x78, 0x00, 
        0x7c, 0x08, 0x08, 0x70, 0x00, 
        0x00, 0x48, 0x7a, 0x40, 0x00, 
        0x00, 0x80, 0x80, 0x7a, 0x00, 
        0x7c, 0x10, 0x28, 0x40, 0x00, 
        0x00, 0x42, 0x7e, 0x40, 0x00, 
        0x78, 0x10, 0x10, 0x78, 0x00, 
        0x78, 0x08, 0x08, 0x70, 0x00, 
        0x30, 0x48, 0x48, 0x30, 0x00, 
        0xf8, 0x48, 0x48, 0x30, 0x00, 
        0x30, 0x48, 0x48, 0xf8, 0x00, 
        0x00, 0x78, 0x10, 0x08, 0x00, 
        0x50, 0x58, 0x68, 0x28, 0x00, 
        0x08, 0x3c, 0x48, 0x48, 0x00, 
        0x38, 0x40, 0x40, 0x78, 0x00, 
        0x18, 0x60, 0x60, 0x18, 0x00, 
        0x78, 0x20, 0x20, 0x78, 0x00, 
        0x48, 0x30, 0x30, 0x48, 0x00, 
        0x18, 0xa0, 0xa0, 0x78, 0x00, 
        0x48, 0x68, 0x58, 0x48, 0x00, 
        0x00, 0x18, 0x24, 0x42, 0x00, 
        0x00, 0x00, 0x7e, 0x00, 0x00, 
        0x00, 0x42, 0x24, 0x18, 0x00, 
        0x10, 0x08, 0x10, 0x08, 0x00, 
    };

    static constexpr ::OLED::FixedFont Fixed_5x8 {
        8,
        8,
        0,
        0x20u,
        0x7fu,
        5,
        1,
        _Fixed_5x8_data
    };

}
