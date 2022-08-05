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
#include <stdio.h>
#include <pico/stdlib.h>

class OLEDFramebuffer {
    public:
        using column_type = uint8_t;

        static constexpr uint COLUMNS { 128 };
        static constexpr uint PAGES { 8 };

        OLEDFramebuffer();

        void clear() 
        {
            m_buffer.fill(0x00);
        }

        void draw_hline(int x, int y, uint w);
        void draw_vline(int x, int y, uint h);
        void draw_rect(int x, int y, uint w, uint h);

        constexpr uint width() const { return COLUMNS; }
        constexpr uint height() const { return PAGES*8; }

        constexpr uint columns() const { return COLUMNS; }
        constexpr uint pages() const { return PAGES; }
        constexpr uint size() const { return COLUMNS*PAGES; }

        const column_type *data() const { return m_buffer.data(); }
        column_type *page(uint page) { return m_buffer.data()+COLUMNS*page; }


    private:
        using buffer_type = std::array<column_type, COLUMNS*PAGES>;

        buffer_type m_buffer;

};
