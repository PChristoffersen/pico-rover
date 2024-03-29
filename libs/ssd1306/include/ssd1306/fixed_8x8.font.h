#pragma once

#include "font.h"

namespace SSD1306::Resource::Font {

    static constexpr ::SSD1306::Font::column_type _Fixed_8x8_data[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x5f, 0x5f, 0x00, 0x00, 0x00, 
        0x00, 0x07, 0x07, 0x00, 0x07, 0x07, 0x00, 0x00, 
        0x14, 0x7f, 0x7f, 0x14, 0x7f, 0x7f, 0x14, 0x00, 
        0x24, 0x2e, 0x2a, 0x6b, 0x6b, 0x3a, 0x12, 0x00, 
        0x46, 0x66, 0x30, 0x18, 0x0c, 0x66, 0x62, 0x00, 
        0x30, 0x7a, 0x4f, 0x5d, 0x37, 0x7a, 0x48, 0x00, 
        0x00, 0x04, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x1c, 0x3e, 0x63, 0x41, 0x00, 0x00, 
        0x00, 0x00, 0x41, 0x63, 0x3e, 0x1c, 0x00, 0x00, 
        0x08, 0x2a, 0x3e, 0x1c, 0x1c, 0x3e, 0x2a, 0x08, 
        0x00, 0x08, 0x08, 0x3e, 0x3e, 0x08, 0x08, 0x00, 
        0x00, 0x00, 0x80, 0xe0, 0x60, 0x00, 0x00, 0x00, 
        0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 
        0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 
        0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x01, 0x00, 
        0x3e, 0x7f, 0x59, 0x4d, 0x47, 0x7f, 0x3e, 0x00, 
        0x00, 0x40, 0x42, 0x7f, 0x7f, 0x40, 0x40, 0x00, 
        0x72, 0x7b, 0x49, 0x49, 0x49, 0x4f, 0x46, 0x00, 
        0x41, 0x41, 0x49, 0x49, 0x49, 0x7f, 0x36, 0x00, 
        0x1e, 0x1e, 0x10, 0x10, 0x7f, 0x7f, 0x10, 0x00, 
        0x27, 0x67, 0x45, 0x45, 0x45, 0x7d, 0x39, 0x00, 
        0x3e, 0x7f, 0x49, 0x49, 0x49, 0x79, 0x30, 0x00, 
        0x01, 0x01, 0x61, 0x71, 0x19, 0x0f, 0x07, 0x00, 
        0x36, 0x7f, 0x49, 0x49, 0x49, 0x7f, 0x36, 0x00, 
        0x06, 0x4f, 0x49, 0x49, 0x49, 0x7f, 0x3e, 0x00, 
        0x00, 0x00, 0x00, 0x66, 0x66, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x80, 0xe6, 0x66, 0x00, 0x00, 0x00, 
        0x00, 0x08, 0x1c, 0x36, 0x63, 0x41, 0x00, 0x00, 
        0x00, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00, 
        0x00, 0x00, 0x41, 0x63, 0x36, 0x1c, 0x08, 0x00, 
        0x00, 0x02, 0x03, 0x59, 0x5d, 0x07, 0x02, 0x00, 
        0x3e, 0x7f, 0x41, 0x5d, 0x5d, 0x5f, 0x5e, 0x00, 
        0x7c, 0x7e, 0x13, 0x11, 0x13, 0x7e, 0x7c, 0x00, 
        0x7f, 0x7f, 0x49, 0x49, 0x49, 0x7f, 0x36, 0x00, 
        0x3e, 0x7f, 0x41, 0x41, 0x41, 0x63, 0x22, 0x00, 
        0x7f, 0x7f, 0x41, 0x41, 0x63, 0x3e, 0x1c, 0x00, 
        0x7f, 0x7f, 0x49, 0x49, 0x49, 0x41, 0x41, 0x00, 
        0x7f, 0x7f, 0x09, 0x09, 0x09, 0x01, 0x01, 0x00, 
        0x3e, 0x7f, 0x41, 0x41, 0x51, 0x73, 0x32, 0x00, 
        0x7f, 0x7f, 0x08, 0x08, 0x08, 0x7f, 0x7f, 0x00, 
        0x00, 0x41, 0x41, 0x7f, 0x7f, 0x41, 0x41, 0x00, 
        0x20, 0x60, 0x40, 0x40, 0x40, 0x7f, 0x3f, 0x00, 
        0x7f, 0x7f, 0x08, 0x1c, 0x36, 0x63, 0x41, 0x00, 
        0x7f, 0x7f, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 
        0x7f, 0x7f, 0x0e, 0x1c, 0x0e, 0x7f, 0x7f, 0x00, 
        0x7f, 0x7f, 0x06, 0x0c, 0x18, 0x7f, 0x7f, 0x00, 
        0x3e, 0x7f, 0x41, 0x41, 0x41, 0x7f, 0x3e, 0x00, 
        0x7f, 0x7f, 0x09, 0x09, 0x09, 0x0f, 0x06, 0x00, 
        0x3e, 0x7f, 0x41, 0x71, 0x61, 0xff, 0xbe, 0x00, 
        0x7f, 0x7f, 0x09, 0x19, 0x39, 0x6f, 0x46, 0x00, 
        0x26, 0x6f, 0x49, 0x49, 0x49, 0x7b, 0x32, 0x00, 
        0x01, 0x01, 0x01, 0x7f, 0x7f, 0x01, 0x01, 0x01, 
        0x7f, 0x7f, 0x40, 0x40, 0x40, 0x7f, 0x7f, 0x00, 
        0x1f, 0x3f, 0x60, 0x60, 0x60, 0x3f, 0x1f, 0x00, 
        0x3f, 0x7f, 0x60, 0x30, 0x60, 0x7f, 0x3f, 0x00, 
        0x63, 0x77, 0x1c, 0x08, 0x1c, 0x77, 0x63, 0x00, 
        0x47, 0x4f, 0x68, 0x38, 0x18, 0x0f, 0x07, 0x00, 
        0x41, 0x61, 0x71, 0x59, 0x4d, 0x47, 0x43, 0x00, 
        0x00, 0x00, 0x7f, 0x7f, 0x41, 0x41, 0x00, 0x00, 
        0x01, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x00, 
        0x00, 0x00, 0x41, 0x41, 0x7f, 0x7f, 0x00, 0x00, 
        0x08, 0x0c, 0x06, 0x03, 0x06, 0x0c, 0x08, 0x00, 
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
        0x00, 0x00, 0x00, 0x03, 0x07, 0x04, 0x00, 0x00, 
        0x20, 0x74, 0x54, 0x54, 0x54, 0x7c, 0x78, 0x00, 
        0x7f, 0x7f, 0x48, 0x48, 0x48, 0x78, 0x30, 0x00, 
        0x38, 0x7c, 0x44, 0x44, 0x44, 0x6c, 0x28, 0x00, 
        0x30, 0x78, 0x48, 0x48, 0x48, 0x7f, 0x7f, 0x00, 
        0x38, 0x7c, 0x54, 0x54, 0x54, 0x5c, 0x18, 0x00, 
        0x00, 0x48, 0x7e, 0x7f, 0x49, 0x03, 0x02, 0x00, 
        0x98, 0xbc, 0xa4, 0xa4, 0xa4, 0xfc, 0x7c, 0x00, 
        0x7f, 0x7f, 0x04, 0x04, 0x04, 0x7c, 0x78, 0x00, 
        0x00, 0x00, 0x44, 0x7d, 0x7d, 0x40, 0x00, 0x00, 
        0x40, 0xc0, 0x80, 0x80, 0x80, 0xfd, 0x7d, 0x00, 
        0x7f, 0x7f, 0x10, 0x18, 0x3c, 0x64, 0x40, 0x00, 
        0x00, 0x00, 0x41, 0x7f, 0x7f, 0x40, 0x00, 0x00, 
        0x7c, 0x7c, 0x18, 0x78, 0x1c, 0x7c, 0x78, 0x00, 
        0x7c, 0x7c, 0x04, 0x04, 0x04, 0x7c, 0x78, 0x00, 
        0x38, 0x7c, 0x44, 0x44, 0x44, 0x7c, 0x38, 0x00, 
        0xfc, 0xfc, 0x24, 0x24, 0x24, 0x3c, 0x18, 0x00, 
        0x18, 0x3c, 0x24, 0x24, 0x24, 0xfc, 0xfc, 0x00, 
        0x7c, 0x7c, 0x04, 0x04, 0x04, 0x0c, 0x08, 0x00, 
        0x48, 0x5c, 0x54, 0x54, 0x54, 0x74, 0x24, 0x00, 
        0x00, 0x04, 0x04, 0x3f, 0x7f, 0x44, 0x44, 0x00, 
        0x3c, 0x7c, 0x40, 0x40, 0x40, 0x7c, 0x7c, 0x00, 
        0x1c, 0x3c, 0x60, 0x60, 0x60, 0x3c, 0x1c, 0x00, 
        0x3c, 0x7c, 0x60, 0x30, 0x60, 0x7c, 0x3c, 0x00, 
        0x44, 0x6c, 0x38, 0x10, 0x38, 0x6c, 0x44, 0x00, 
        0x9c, 0xbc, 0xa0, 0xa0, 0xa0, 0xfc, 0x7c, 0x00, 
        0x44, 0x64, 0x74, 0x54, 0x5c, 0x4c, 0x44, 0x00, 
        0x00, 0x08, 0x08, 0x3e, 0x77, 0x41, 0x41, 0x00, 
        0x00, 0x00, 0x00, 0x77, 0x77, 0x00, 0x00, 0x00, 
        0x00, 0x41, 0x41, 0x77, 0x3e, 0x08, 0x08, 0x00, 
        0x02, 0x03, 0x01, 0x03, 0x02, 0x03, 0x01, 0x00, 
    };

    static constexpr ::SSD1306::FixedFont Fixed_8x8 {
        8,
        8,
        0,
        0x20u,
        0x7fu,
        8,
        1,
        _Fixed_8x8_data
    };

}
