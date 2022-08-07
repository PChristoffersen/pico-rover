#include "framebuffer.h"

#include <algorithm>


namespace OLED {

static inline void _pixel_op(uint8_t &pix, uint8_t val, bool clear)
{
    if (clear)
        pix ^= ~val;
    else
        pix |= val;
}


Framebuffer::Framebuffer() 
{
    clear();
}



void Framebuffer::clear() 
{
    m_buffer.fill(0x00);
    m_dirty = true;
    m_dirty_region.expand(0, 0, width(), height());
}




void Framebuffer::draw_hline(int x, int y, uint w, bool clear)
{
    if (x>=(int)width() || y>=(int)height() || y<0 || w==0 || (x+(int)w)<0)
        return;

    const int x1 = std::max<int>(x, 0);
    const int x2 = std::min<int>(x+w-1, width()-1);

    auto buf = page(y/8);
    const uint8_t val = 1 << (y&0b111);
    for (int i=x1; i<=x2; ++i) {
        _pixel_op(buf[i], val, clear);
    }

    m_dirty = true;
    m_dirty_region.expand(x1, y, x2, y);
}


void Framebuffer::draw_vline(int x, int y, uint h, bool clear)
{
    if (x>=(int)width() || y>=(int)height() || x<0 || h==0 || (y+(int)h)<0)
        return;

    const int y1 = std::max<int>(y, 0);
    const int y2 = std::min<int>(y+h-1, height()-1);

    int pg = y1/8;
    auto buf = page(pg);
    for (int i=y1; i<=y2; ++i) {
        if (pg!=i/8) {
            pg = i/8;
            buf = page(pg);
        }
        _pixel_op(buf[x], 1<<(i&0b111), clear);
    }

    m_dirty = true;
    m_dirty_region.expand(x, y1, x, y2);
}



void Framebuffer::draw_rect(int x, int y, uint w, uint h, bool clear)
{
    draw_hline(x,     y,      w,   clear);
    draw_hline(x,     y+h-1,  w,   clear);
    draw_vline(x,     y+1,    h-2, clear);
    draw_vline(x+w-1, y+1,    h-2, clear);
}


void Framebuffer::fill_rect(int x, int y, uint w, uint h, bool clear)
{
    if (w==0 || h==0 || x>=(int)width() || y>=(int)height() || (x+(int)w)<0 || (y+(int)h)<0)
        return;

    const int x1 = std::max<int>(x, 0);
    const int x2 = std::min<int>(x+w-1, width()-1);
    const int y1 = std::max<int>(y, 0);
    const int y2 = std::min<int>(y+h-1, height()-1);
    int p1 = y1/8;
    int p2 = y2/8;

    m_dirty = true;
    m_dirty_region.expand(x1, y1, x2, y2);

    // If start and end page is the same?
    if (p1==p2) {
        auto buf = page(p1);
        uint8_t val = 0x00;
        for (int b=(y1&0b111); b<=(y2&0b111); ++b) {
            val |= (1<<b);
        }
        for (int i=x1; i<=x2; ++i) {
            _pixel_op(buf[i], val, clear);
        }
        return;
    }

    // If start is not aligned
    if (y1&0b111) {
        // Fill top
        auto buf = page(p1);
        uint val = 0x00;
        for (int bit=y1&0b11; bit<8; ++bit) {
            val |= (1<<bit);
        }
        for (int i=x1; i<=x2; ++i) {
            _pixel_op(buf[i], val, clear);
        }
        p1++;
    }

    // If end is aligned
    if ((y2&0b111) != 0b111) {
        // Fill bottom
        auto buf = page(p2);
        uint val = 0x00;
        for (int bit=0; bit<=(y2&0b111); ++bit) {
            val |= (1<<bit);
        }
        for (int i=x1; i<=x2; ++i) {
            _pixel_op(buf[i], val, clear);
        }
        p2--;
    }

    // Fill center
    for (int pg = p1; pg<=p2; ++pg) {
        auto buf = page(pg);
        for (int i=x1; i<=x2; ++i) {
            _pixel_op(buf[i], 0xFF, clear);
        }
    }



}



void Framebuffer::draw_bitmap(int x, int y, const uint8_t *bitmap, uint w, uint h, bool clear)
{
    if (w==0 || h==0 || x>=(int)width() || y>=(int)height() || (x+(int)w)<0 || (y+(int)h)<0)
        return;

    assert((h&0b111)==0); // Bitmap height must be multiple of 8

    int x1 = std::max<int>(x, 0);
    int x2 = std::min<int>(x+w-1, width()-1);
    int y1 = std::max<int>(y, 0);
    int y2 = std::min<int>(y+h-1, height()-1);

    if ((y & 0b111)==0) { 
        // Bytealigned target
        bitmap += (y1-y)*w/8;
        for (int pg=y1/8; pg<=y2/8; ++pg) {
            auto buf = page(pg);
            for (int i=x1; i<=x2; ++i) {
                _pixel_op(buf[i], bitmap[i-x1], clear);
            }
            bitmap+=w;
        }

    }
    else {
        // TODO
        assert(false);
    }

    m_dirty = true;
    m_dirty_region.expand(x1, y1, x2, y2);
}




void Framebuffer::draw_text(int x, int y, const char *text, const Font &font, bool clear)
{
    int pos = x;
    while (*text) {
        uint8_t ch = (uint8_t)*text;

        if (!font.have_glyph(ch)) {
            text++;
            continue;
        }


        const auto &glyph = font.glyph(ch);

        if (glyph.have_bitmap()) {
            printf("Draw: %d %d\n", pos+glyph.off_x, y+glyph.off_y);
            draw_bitmap(pos+glyph.off_x, y+glyph.off_y*8, font.data(glyph), glyph.width, glyph.height*8, clear);
        }

        pos += glyph.advance;

        text++;
    }
}




void Framebuffer::print(uint n_pgs) 
{
    printf("     ");
    for (uint c=0; c<columns(); ++c) {
        printf("%d", c%10);
    }
    printf("\n");
    for (uint pg=0; pg<n_pgs; ++pg) {
        auto buf = page(pg);
        for (uint bit=0; bit<8; ++bit) {
            printf("%2u: |", pg*8+bit);
            for (uint c=0; c<columns(); ++c) {
                bool dirty = m_dirty 
                    && (pg>=m_dirty_region.p1 && pg<=m_dirty_region.p2)
                    && (c>=m_dirty_region.c1 && c<=m_dirty_region.c2);
                if (buf[c]&(1<<bit)) 
                    printf(dirty?"@":"*");
                else
                    printf(dirty?".":" ");
            }
            printf("|\n");
        }
    }
    printf("Dirty: %d %d %d %d\n", m_dirty_region.c1, m_dirty_region.c2, m_dirty_region.p1, m_dirty_region.p2);
}


}
