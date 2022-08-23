#include "framebuffer.h"

#include <algorithm>


namespace OLED {

static inline void _pixel_op(uint8_t &pix, uint8_t val, Framebuffer::DrawOp op)
{
    switch (op) {
    case Framebuffer::DrawOp::ADD:
        pix |= val;
        break;
    case Framebuffer::DrawOp::SUBTRACT:
        pix &= ~val;
        break;
    case Framebuffer::DrawOp::INVERT:
        pix ^= val;
        break;
    }
}


Framebuffer::Framebuffer() :
    m_dirty { false }
{
    m_buffer.fill(0x00);
}



void Framebuffer::clear() 
{
    m_buffer.fill(0x00);
    m_dirty = true;
    m_dirty_region.expand(0, 0, width(), height());
}




void Framebuffer::draw_hline(int x, int y, uint w, DrawOp op)
{
    if (x>=(int)width() || y>=(int)height() || y<0 || w==0 || (x+(int)w)<0)
        return;

    const int x1 = std::max<int>(x, 0);
    const int x2 = std::min<int>(x+w-1, width()-1);

    auto buf = page(y/PAGE_BITS);
    const uint8_t val = 1 << (y&PAGE_MASK);
    for (int i=x1; i<=x2; ++i) {
        _pixel_op(buf[i], val, op);
    }

    m_dirty = true;
    m_dirty_region.expand(x1, y, x2, y);
}


void Framebuffer::draw_vline(int x, int y, uint h, DrawOp op)
{
    if (x>=(int)width() || y>=(int)height() || x<0 || h==0 || (y+(int)h)<0)
        return;

    const int y1 = std::max<int>(y, 0);
    const int y2 = std::min<int>(y+h-1, height()-1);

    int pg = y1/PAGE_BITS;
    auto buf = page(pg);
    for (int i=y1; i<=y2; ++i) {
        if (pg!=(int)(i/PAGE_BITS)) {
            pg = i/PAGE_BITS;
            buf = page(pg);
        }
        _pixel_op(buf[x], 1<<(i&PAGE_MASK), op);
    }

    m_dirty = true;
    m_dirty_region.expand(x, y1, x, y2);
}



void Framebuffer::draw_rect(int x, int y, uint w, uint h, DrawOp op)
{
    draw_hline(x,     y,      w,   op);
    draw_hline(x,     y+h-1,  w,   op);
    draw_vline(x,     y+1,    h-2, op);
    draw_vline(x+w-1, y+1,    h-2, op);
}


void Framebuffer::fill_rect(int x, int y, uint w, uint h, DrawOp op)
{
    if (w==0 || h==0 || x>=(int)width() || y>=(int)height() || (x+(int)w)<0 || (y+(int)h)<0)
        return;

    const int x1 = std::max<int>(x, 0);
    const int x2 = std::min<int>(x+w-1, width()-1);
    const int y1 = std::max<int>(y, 0);
    const int y2 = std::min<int>(y+h-1, height()-1);
    int p1 = y1/PAGE_BITS;
    int p2 = y2/PAGE_BITS;

    m_dirty = true;
    m_dirty_region.expand(x1, y1, x2, y2);

    // If start and end page is the same?
    if (p1==p2) {
        auto buf = page(p1);
        uint8_t val = 0x00;
        for (int b=(y1&PAGE_MASK); b<=(y2&PAGE_MASK); ++b) {
            val |= (1<<b);
        }
        for (int i=x1; i<=x2; ++i) {
            _pixel_op(buf[i], val, op);
        }
        return;
    }

    // If start is not aligned
    if (y1&PAGE_MASK) {
        // Fill top
        auto buf = page(p1);
        uint val = 0x00;
        for (int bit=y1&PAGE_MASK; bit<(int)PAGE_BITS; ++bit) {
            val |= (1<<bit);
        }
        for (int i=x1; i<=x2; ++i) {
            _pixel_op(buf[i], val, op);
        }
        p1++;
        
    }

    // If end is aligned
    if ((y2&PAGE_MASK) != PAGE_MASK) {
        // Fill bottom
        auto buf = page(p2);
        uint val = 0x00;
        for (int bit=0; bit<=(y2&PAGE_MASK); ++bit) {
            val |= (1<<bit);
        }
        for (int i=x1; i<=x2; ++i) {
            _pixel_op(buf[i], val, op);
        }
        p2--;
    }

    // Fill center
    for (int pg = p1; pg<=p2; ++pg) {
        auto buf = page(pg);
        for (int i=x1; i<=x2; ++i) {
            _pixel_op(buf[i], 0xFF, op);
        }
    }



}



void Framebuffer::draw_bitmap(int x, int y, const uint8_t *bitmap, uint w, uint h, DrawOp op)
{
    if (w==0 || h==0 || x>=(int)width() || y>=(int)height() || (x+(int)w)<0 || (y+(int)h)<0)
        return;

    assert((h&PAGE_MASK)==0); // Bitmap height must be multiple of PAGE_BITS

    int x1 = std::max<int>(x, 0);
    int x2 = std::min<int>(x+w-1, width()-1);
    int y1 = std::max<int>(y, 0);
    int y2 = std::min<int>(y+h-1, height()-1);

    if ((y & PAGE_MASK)==0) { 
        // Bytealigned target
        bitmap += (y1-y)*w/PAGE_BITS;
        for (int pg=y1/PAGE_BITS; pg<=(int)(y2/PAGE_BITS); ++pg) {
            auto buf = page(pg);
            for (int i=x1; i<=x2; ++i) {
                _pixel_op(buf[i], bitmap[i-x1], op);
            }
            bitmap+=w;
        }
    }
    else {
        int shift = y & PAGE_MASK;
        int y1 = y & ~PAGE_MASK;

        int off = 0;
        for (int y_ = y1; y_ < y2; y_+=(PAGE_MASK+1)) {
            int pg = y_/(PAGE_MASK+1);
            if (pg>=0 && pg<static_cast<int>(pages())) {
                auto buf = page(pg);
                if (y_ == y1) {
                    for (int i=x1; i<=x2; ++i) {
                        _pixel_op(buf[i], bitmap[off+i-x1]<<shift, op);
                    }
                }
                else if (y_+shift-y < static_cast<int>(h)) {
                    for (int i=x1; i<=x2; ++i) {
                        uint8_t v = bitmap[off+i-x1-w]>>(PAGE_MASK+1-shift);
                        v |= (bitmap[off+i-x1]<<shift);
                        _pixel_op(buf[i], v, op);
                    }
                }
                else {
                    for (int i=x1; i<=x2; ++i) {
                        _pixel_op(buf[i], bitmap[off+i-x1-w]>>(PAGE_MASK+1-shift), op);
                    }
                }
            }
            off+=w;
        }

    }

    m_dirty = true;
    m_dirty_region.expand(x1, y1, x2, y2);
}




void Framebuffer::draw_text(int x, int y, const char *text, const Font &font, DrawOp op)
{
    int pos = x;
    while (*text) {
        uint8_t ch = (uint8_t)*text;

        if (!font.have_glyph(ch)) {
            text++;
            continue;
        }

        const Font::Glyph *glyph = nullptr;
        const column_type *glyph_data = nullptr;
        if (font.glyph(ch, glyph, glyph_data)) {
            if (glyph_data) {
                draw_bitmap(pos+glyph->off_x, y+glyph->off_y*PAGE_BITS, glyph_data, glyph->width, glyph->height*PAGE_BITS, op);
            }
            pos += glyph->advance;
        }
        text++;
    }
}



#ifndef NDEBUG
void Framebuffer::print(uint n_pgs) 
{
    printf("     ");
    for (uint c=0; c<columns(); ++c) {
        printf("%d", c%10);
    }
    printf("\n");
    for (uint pg=0; pg<n_pgs; ++pg) {
        auto buf = page(pg);
        for (uint bit=0; bit<PAGE_BITS; ++bit) {
            printf("%2u: |", pg*PAGE_BITS+bit);
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
#endif

}
