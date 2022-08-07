/**
 * @author Peter Christoffersen
 * @brief OLED Font
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>
#include <stdint.h>

namespace OLED {

    class Font {
        public:
            using column_type = uint8_t;
            using char_type = uint8_t;

            struct Glyph {
                uint16_t data_offset;
                uint advance;
                uint width;
                uint height;
                int off_x;
                int off_y;
                bool have_bitmap() const { return width > 0; }
            };

            constexpr Font(uint size, uint ascent, uint descent, char_type first_char, char_type last_char, const column_type *data, const Glyph *glyphs) : 
                m_size { size },
                m_ascent { ascent },
                m_descent { descent },
                m_first_char { first_char },
                m_last_char { last_char },
                m_data { data }, 
                m_glyphs { glyphs }
            {
            }

            bool have_glyph(char_type ch) const { return ch>=m_first_char && ch<m_last_char; }

            const Glyph &glyph(char_type ch) const
            {
                assert(ch>=m_first_char && ch<m_last_char);
                return m_glyphs[ch-m_first_char];
            }

            const column_type *data(const Glyph &glyph) const {
                assert(glyph.have_bitmap());
                return m_data+glyph.data_offset;
            }

        protected:
            uint m_size;
            uint m_ascent;
            uint m_descent;
            char_type m_first_char;
            char_type m_last_char;
            const column_type *m_data;
            const Glyph *m_glyphs;
    };


}