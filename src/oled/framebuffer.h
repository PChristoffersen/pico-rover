/**
 * @author Peter Christoffersen
 * @brief OLED 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <array>
#include <algorithm>
#include <stdio.h>
#if 0
#include <pico/stdlib.b>
#else
#include <stdint.h>
#include <assert.h>
#endif

#include "image.h"
#include "font.h"


namespace OLED {

    class Framebuffer {
        public:
            using column_type = uint8_t;

            static constexpr uint COLUMNS { 128 };
            static constexpr uint PAGES { 8 };

            struct Region {
                uint8_t c1;
                uint8_t p1;
                uint8_t c2; 
                uint8_t p2;
                Region() { reset(); }
                void reset() { c1 = COLUMNS; p1 = PAGES; c2 = p2 = 0u; }
                void expand(int x1, int y1, int x2, int y2)
                {
                    int _p1 = y1/8;
                    int _p2 = y2/8;
                    if (x1<c1) c1 = std::max<int>(x1, 0);
                    if (x2>c2) c2 = std::min<int>(x2, COLUMNS-1);
                    if (_p1<p1) p1 = std::max<int>(_p1, 0);
                    if (_p2>p2) p2 = std::min<int>(_p2, PAGES-1);
                }
            };

            Framebuffer();

            void clear();

            void draw_hline(int x, int y, uint w, bool clear = false);
            void draw_vline(int x, int y, uint h, bool clear = false);
            void draw_rect(int x, int y, uint w, uint h, bool clear = false);
            void fill_rect(int x, int y, uint w, uint h, bool clear = false);
            void draw_bitmap(int x, int y, const uint8_t *bitmap, uint w, uint h, bool clear = false);
            void draw_bitmap(int x, int y, const Image &image, bool clear = false)
            {
                draw_bitmap(x, y, image.buffer(), image.width(), image.height(), clear);
            }
            void draw_text(int x, int y, const char *text, const Font &font, bool clear = false);



            constexpr uint width() const { return COLUMNS; }
            constexpr uint height() const { return PAGES*8; }

            constexpr uint columns() const { return COLUMNS; }
            constexpr uint pages() const { return PAGES; }
            constexpr uint size() const { return COLUMNS*PAGES; }

            const column_type *data() const { return m_buffer.data(); }
            column_type *page(uint page) { assert(page<PAGES); return m_buffer.data()+COLUMNS*page; }

            const Region &dirty_region() const { return m_dirty_region; }
            bool is_dirty() const { return m_dirty; }
            void clear_dirty() { m_dirty = false; m_dirty_region.reset(); }


            void print(uint n_pgs = PAGES);
        private:
            using buffer_type = std::array<column_type, COLUMNS*PAGES>;

            buffer_type m_buffer;

            Region m_dirty_region;
            bool m_dirty;

    };

}
