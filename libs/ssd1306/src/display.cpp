#include <ssd1306/display.h>

#if !PICO_NO_HARDWARE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pico/stdlib.h>
#include <hardware/i2c.h>

#include <i2c_bus.h>
#include "ssd1306_regs.h"


namespace SSD1306 {


Display::Display(addr_type addr) : 
    m_address { addr },
    m_present { false }
{

}


void Display::send_cmds(const uint8_t *cmds, uint len)
{
    uint8_t buf[len*2];
    for (uint i=0; i<len; ++i) {
        buf[2*i] = SSD1306_CONTROL_COMMAND;
        buf[2*i+1] = cmds[i];
    }
    i2c_write_blocking(i2c_default, m_address, buf, sizeof(buf), false);
}


void Display::send_data_sync()
{
    const uint8_t cmds[] = {
        SSD1306_SET_COL_ADDR,
        0,              // Start col
        static_cast<uint8_t>(m_framebuffer.columns() - 1), // End col

        SSD1306_SET_PAGE_ADDR,
        0,                  // Start page
        static_cast<uint8_t>(m_framebuffer.pages() - 1), // End page
    };
    send_cmds(cmds, sizeof(cmds));

    size_t sz = m_framebuffer.size()+1;
    uint8_t *data = new uint8_t[sz];
    data[0] = SSD1306_CONTROL_DATA_STREAM;

    memcpy(data+1, m_framebuffer.data(), m_framebuffer.size());
    i2c_write_blocking(i2c_default, m_address, data, sz, false);

    delete[] data;
}


void Display::send_data_async() {
    auto &dirty = m_framebuffer.dirty_region();
    const uint8_t cmds[] = {
        SSD1306_SET_COL_ADDR,
        dirty.c1,   // Start col
        dirty.c2, // End col

        SSD1306_SET_PAGE_ADDR,
        dirty.p1,   // Start page
        dirty.p2, // End page
    };

    send_cmds(cmds, sizeof(cmds));

    i2c_dma_buffer_reset();

    uint8_t hdr = SSD1306_CONTROL_DATA_STREAM;
    i2c_dma_buffer_append(&hdr, 1);
    for (uint p=dirty.p1; p<=dirty.p2; p++) {
        auto page = m_framebuffer.page(p);
        i2c_dma_buffer_append(page+dirty.c1, dirty.c2-dirty.c1+1);
    }
    i2c_bus_write_dma(m_address);
}



void Display::init()
{
    i2c_bus_acquire_blocking();

    for (uint i=0; i<50; i++) {
        uint8_t data = 0x00;
        int res = i2c_read_blocking(i2c_default, m_address, &data, sizeof(data), false);
        if (res < static_cast<int>(sizeof(data))) {
            printf("No Display!!!: %d\n", res);
            m_present = false;
            busy_wait_ms(100);
        }
        else {
            m_present = true;
            break;
        }
    }
    if (!m_present) {
        printf("No Display!!!\n");
        m_present = false;
        i2c_bus_release();
        return;
    }


    uint8_t com;
    uint8_t contrast;

    if (m_framebuffer.height()==32) {
        com = 0x02;
        contrast = 0x8F;
    }
    else if (m_framebuffer.height()==64) {
        com = 0x12;
        contrast = 0xcf;
    }
    else {
        // Unsupported frame buffer height
        assert(false);
        m_present = false;
        i2c_bus_release();
        return;
    }


    uint8_t cmds[] = {
        SSD1306_SET_DISP_OFF, // set display off

        /* memory mapping */
        SSD1306_SET_MEM_ADDR, // set memory address mode
        SSD1306_MEMORYMODE_HORZONTAL, // horizontal addressing mode

        /* resolution and layout */
        SSD1306_SET_DISP_START_LINE, // set display start line to 0

        SSD1306_SET_SEG_REMAP_SEG127, // set segment re-map
        // column address 127 is mapped to SEG0

        SSD1306_SET_MUX_RATIO, // set multiplex ratio
        static_cast<uint8_t>(m_framebuffer.height() - 1), // our display width 

        SSD1306_SET_COM_OUT_DIR_REMAP, // set COM (common) output scan direction
        // scan from bottom up, COM[N-1] to COM0

        SSD1306_SET_DISP_OFFSET, // set display offset
        0x00, // no offset

        SSD1306_SET_COM_PIN_CFG, // set COM (common) pins hardware configuration
        com, // manufacturer magic number

        /* timing and driving scheme */
        SSD1306_SET_DISP_CLK_DIV, // set display clock divide ratio
        0x80, // div ratio of 1, standard freq

        SSD1306_SET_PRECHARGE, // set pre-charge period
        0xF1, // Vcc internally generated on our board

        SSD1306_SET_VCOM_DESEL, // set VCOMH deselect level
        0x30, // 0.83xVcc

        /* display */
        SSD1306_SET_CONTRAST, // set contrast control
        contrast,

        SSD1306_SET_ENTIRE_ON, // set entire display on to follow RAM content

        SSD1306_SET_NORM_INV, // set normal (not inverted) display

        SSD1306_SET_CHARGE_PUMP, // set charge pump
        0x14, // Vcc internally generated on our board

        SSD1306_SET_SCROLL_OFF, // deactivate horizontal scrolling if set
        // this is necessary as memory writes will corrupt if scrolling was enabled
    };
    send_cmds(cmds, sizeof(cmds));

    m_framebuffer.clear();
    send_data_sync();
    m_framebuffer.clear_dirty();

    uint8_t cmds2[] = {
        SSD1306_SET_DISP_ON, // turn display on
    };
    send_cmds(cmds2, sizeof(cmds2));

    i2c_bus_release();
}



void Display::update()
{ 
    if (!m_framebuffer.is_dirty() || !m_present) 
        return;
    i2c_bus_acquire_blocking();
    send_data_async();
    m_framebuffer.clear_dirty();
}


void Display::update_sync()
{
    if (!m_framebuffer.is_dirty() || !m_present) 
        return;
    i2c_bus_acquire_blocking();
    send_data_sync();
    m_framebuffer.clear_dirty();
    i2c_bus_release();
}


}

#endif
