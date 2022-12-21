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

namespace SSD1306 {

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

            constexpr Font(uint size, uint ascent, uint descent, char_type first_char, char_type last_char) : 
                m_size { size },
                m_ascent { ascent },
                m_descent { descent },
                m_first_char { first_char },
                m_last_char { last_char }
            {
            }

            bool have_glyph(char_type ch) const { return ch>=m_first_char && ch<m_last_char; }

            uint width(const char *text) const 
            {
                uint w = 0;
                while (*text) {
                    const Glyph *gl;
                    if (glyph(*text, gl)) {
                        w += gl->advance;
                    }
                    text++;
                }
                return w;
            }

            virtual bool glyph(char_type ch, const Glyph *&glyph) const = 0;
            virtual bool glyph(char_type ch, const Glyph *&glyph, const column_type *&data) const = 0;

        protected:
            uint m_size;
            uint m_ascent;
            uint m_descent;
            char_type m_first_char;
            char_type m_last_char;
    };


    class FixedFont : public Font {
        public:
             constexpr FixedFont(uint size, uint ascent, uint descent, char_type first_char, char_type last_char, uint width, uint height, const column_type *data) : 
                Font { size, ascent, descent, first_char, last_char },
                m_glyph { 0x0000, width, width, height, 0, 0 },
                m_data { data }
            {
            }

            virtual bool glyph(char_type ch, const Glyph *&glyph) const 
            {
                if (ch<m_first_char && ch>m_last_char)
                    return false;

                glyph = &m_glyph;
                return true;
            }

            virtual bool glyph(char_type ch, const Glyph *&glyph, const column_type *&data) const 
            {
                if (ch<m_first_char && ch>m_last_char)
                    return false;
                glyph = &m_glyph;
                data = m_data+(ch-m_first_char)*m_glyph.width*m_glyph.height;
                return true;
            }


        private:
            const Glyph m_glyph;
            const column_type *m_data;
    };

    class DynamicFont : public Font {
        public:
             constexpr DynamicFont(uint size, uint ascent, uint descent, char_type first_char, char_type last_char, const Glyph *glyphs, const column_type *data) : 
                Font { size, ascent, descent, first_char, last_char },
                m_glyphs { glyphs },
                m_data { data }
            {
            }

            virtual bool glyph(char_type ch, const Glyph *&glyph) const 
            {
                if (ch<m_first_char && ch>m_last_char)
                    return false;

                glyph = &m_glyphs[ch-m_first_char];
                return true;
            }

            virtual bool glyph(char_type ch, const Glyph *&glyph, const column_type *&data) const 
            {
                if (ch<m_first_char && ch>m_last_char)
                    return false;

                glyph = &m_glyphs[ch-m_first_char];
                if (glyph->width && glyph->height) {
                    data = m_data+glyph->data_offset;
                }
                else {
                    data = nullptr;
                }
                return true;
            }


        private:
            const Glyph *m_glyphs;
            const column_type *m_data;
    };

}