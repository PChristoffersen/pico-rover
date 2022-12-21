/**
 * @author Peter Christoffersen
 * @brief OLED Image
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>
#include <stdint.h>

namespace SSD1306 {

    class Image {
        public:
            using column_type = uint8_t;
        
            constexpr uint width() const { return m_width; }
            constexpr uint height() const { return m_height; }
            constexpr const column_type *buffer() const { return m_buffer; }
                
            constexpr Image(uint width, uint height) : m_width { width }, m_height { height }, m_buffer { nullptr } {}
            constexpr Image(uint width, uint height, const column_type *buffer) : m_width { width }, m_height { height }, m_buffer { buffer } {}
        protected:
            uint m_width;
            uint m_height;
            const column_type *m_buffer;
    };


}