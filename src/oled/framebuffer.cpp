#include "framebuffer.h"

#include <algorithm>

OLEDFramebuffer::OLEDFramebuffer() 
{
    m_buffer.fill(0x00);
}


void OLEDFramebuffer::draw_hline(int x, int y, uint w)
{
    if (x>=(int)width() || y>=(int)height() || y<0 || w==0 || (x+(int)w)<0)
        return;

    int x1 = std::max<int>(x, 0);
    int x2 = std::min<int>(x+w, width()-1);

    auto buf = page(y/8);
    const uint8_t val = 1 << (y&0b111);
    for (int i=x1; i<x2; ++i) {
        buf[i] |= val;
    }
}


void OLEDFramebuffer::draw_vline(int x, int y, uint h)
{
    if (x>=(int)width() || y>=(int)height() || x<0 || h==0 || (y+(int)h)<0)
        return;

    int y1 = std::max<int>(y, 0);
    int y2 = std::min<int>(y+h, height()-1);

    int pg = y1/8;
    auto buf = page(pg);
    for (int i=y1; i<y2; ++i) {
        if (pg!=i/8) {
            pg = i/8;
            buf = page(pg);
        }
        buf[x] |= 1<<(i&0b111);
    }
}



void OLEDFramebuffer::draw_rect(int x, int y, uint w, uint h)
{
    draw_hline(x, y, w);
    draw_hline(x, y+h-1, w);
    draw_vline(x, y+1, h-2);
    draw_vline(x+w-1, y+1, h-2);
}
