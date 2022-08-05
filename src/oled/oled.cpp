#include "oled.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pico/stdlib.h>

#include "ssd1306.h"
#include "../i2c_bus.h"


OLEDDisplay::OLEDDisplay(Address addr, Type type) : 
    m_address { static_cast<addr_t>(addr) },
    m_present { false }
{

}


void OLEDDisplay::send_cmds(const uint8_t *cmds, uint len)
{
    uint8_t buf[len*2];
    for (uint i=0; i<len; ++i) {
        buf[2*i] = SSD1306_CONTROL_COMMAND;
        buf[2*i+1] = cmds[i];
    }
    i2c_write_blocking(i2c_default, m_address, buf, sizeof(buf), false);
}


void OLEDDisplay::send_data_sync()
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


void OLEDDisplay::send_data_async() {
    const uint8_t cmds[] = {
        SSD1306_SET_COL_ADDR,
        0,              // Start col
        static_cast<uint8_t>(m_framebuffer.columns() - 1), // End col

        SSD1306_SET_PAGE_ADDR,
        0,                  // Start page
        static_cast<uint8_t>(m_framebuffer.pages() - 1), // End page
    };
    send_cmds(cmds, sizeof(cmds));

    i2c_dma_buffer_reset();

    uint8_t hdr = SSD1306_CONTROL_DATA_STREAM;
    i2c_dma_buffer_append(&hdr, 1);
    i2c_dma_buffer_append(m_framebuffer.data(), m_framebuffer.size());
    i2c_bus_write_dma(m_address);
}



void OLEDDisplay::init()
{
    i2c_bus_acquire_blocking();

    uint8_t data = 0x00;
    if (i2c_read_blocking(i2c_default, m_address, &data, sizeof(data), false)<static_cast<int>(sizeof(data))) {
        m_present = false;
        i2c_bus_release();
        return;
    }
    m_present = true;


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

    uint8_t cmds2[] = {
        SSD1306_SET_DISP_ON, // turn display on
    };
    send_cmds(cmds2, sizeof(cmds2));

    i2c_bus_release();
}



void OLEDDisplay::update()
{ 
    i2c_bus_acquire_blocking();
    send_data_async(); 
}

#if 0
#define OLED_NUM_PAGES (OLED_HEIGHT / SSD1306_PAGE_HEIGHT)
#define OLED_BUF_LEN (OLED_NUM_PAGES * OLED_WIDTH)

static bool g_oled_detected = false;

typedef struct {
    uint x1;
    uint y1;
    uint x2;
    uint y2;
} region_t;


// OLED Frame buffer
static uint8_t g_oled_data[OLED_BUF_LEN];
static region_t dirty_region;


static inline void _ssd1306_send_cmds(const uint8_t *cmds, uint count)
{
    uint8_t buf[count*2];
    for (uint i=0; i<count; ++i) {
        buf[2*i] = SSD1306_CONTROL_COMMAND;
        buf[2*i+1] = cmds[i];
    }
    i2c_write_blocking(i2c_default, OLED_ADDRESS, buf, sizeof(buf), false);
}

static inline void _ssd1306_send_cmd(uint8_t cmd)
{
    _ssd1306_send_cmds(&cmd, 1);
}




void _ssd1306_update_sync() {
    const uint8_t cmds[] = {
        SSD1306_SET_COL_ADDR,
        0,              // Start col
        OLED_WIDTH - 1, // End col

        SSD1306_SET_PAGE_ADDR,
        0,                  // Start page
        OLED_NUM_PAGES - 1, // End page
    };
    _ssd1306_send_cmds(cmds, sizeof(cmds));

    uint8_t *data = new uint8_t[sizeof(g_oled_data)+1];
    data[0] = SSD1306_CONTROL_DATA_STREAM;
    memcpy(data+1, g_oled_data, sizeof(g_oled_data));

    i2c_write_blocking(i2c_default, OLED_ADDRESS, data, sizeof(data), false);
    delete[] data;
}

void _ssd1306_update_async() {
    const uint8_t cmds[] = {
        SSD1306_SET_COL_ADDR,
        0,              // Start col
        OLED_WIDTH - 1, // End col

        SSD1306_SET_PAGE_ADDR,
        0,                  // Start page
        OLED_NUM_PAGES - 1, // End page
    };
    _ssd1306_send_cmds(cmds, sizeof(cmds));

    i2c_dma_buffer_reset();

    uint8_t hdr = SSD1306_CONTROL_DATA_STREAM;
    i2c_dma_buffer_append(&hdr, 1);
    i2c_dma_buffer_append(g_oled_data, sizeof(g_oled_data));
    i2c_bus_write_dma(OLED_ADDRESS);
}



static inline void _buffer_set_pixel(uint x, uint y, bool value)
{
    if ((x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) return;

    uint off = x + y / 8;
    uint8_t byte =  1 << (y & 0b0111);

    if (value) {
        g_oled_data[off] |= byte;
    }
    else {
        g_oled_data[off] &= ~byte;
    }
}


static inline void _buffer_set_byte(uint x, uint y, uint8_t byte)
{
    if ((x >= OLED_WIDTH) || (y >= OLED_HEIGHT)) return;

    uint off = x + y / 8;
    g_oled_data[off] = byte;
}


/**
 * Try to detect if an OLED is present by reading a single byte from the address
 * If the read succedes, assume it is present.
 */
static bool _oled_detect()
{
    uint8_t rxdata = 0x00;
    int ret = i2c_read_blocking(i2c_default, OLED_ADDRESS, &rxdata, 1, false);
    return ret==1;
}


void oled_init() {

    i2c_bus_acquire_blocking();

    if (!_oled_detect()) {
        // No OLED display detected
        g_oled_detected = false;
        i2c_bus_release();
        return;
    }

    const uint8_t cmds[] = {
        SSD1306_SET_DISP_OFF, // set display off

        /* memory mapping */
        SSD1306_SET_MEM_ADDR, // set memory address mode
        SSD1306_MEMORYMODE_VERTICAL, // horizontal addressing mode

        /* resolution and layout */
        SSD1306_SET_DISP_START_LINE, // set display start line to 0

        SSD1306_SET_SEG_REMAP_SEG127, // set segment re-map
        // column address 127 is mapped to SEG0

        SSD1306_SET_MUX_RATIO, // set multiplex ratio
        OLED_HEIGHT - 1, // our display is 64 pixels high

        SSD1306_SET_COM_OUT_DIR_REMAP, // set COM (common) output scan direction
        // scan from bottom up, COM[N-1] to COM0

        SSD1306_SET_DISP_OFFSET, // set display offset
        0x00, // no offset

        SSD1306_SET_COM_PIN_CFG, // set COM (common) pins hardware configuration
        0x02, // manufacturer magic number

        /* timing and driving scheme */
        SSD1306_SET_DISP_CLK_DIV, // set display clock divide ratio
        0x80, // div ratio of 1, standard freq

        SSD1306_SET_PRECHARGE, // set pre-charge period
        0xF1, // Vcc internally generated on our board

        SSD1306_SET_VCOM_DESEL, // set VCOMH deselect level
        0x30, // 0.83xVcc

        /* display */
        SSD1306_SET_CONTRAST, // set contrast control
        0xFF,

        SSD1306_SET_ENTIRE_ON, // set entire display on to follow RAM content

        SSD1306_SET_NORM_INV, // set normal (not inverted) display

        SSD1306_SET_CHARGE_PUMP, // set charge pump
        0x14, // Vcc internally generated on our board

        SSD1306_SET_SCROLL_OFF, // deactivate horizontal scrolling if set
        // this is necessary as memory writes will corrupt if scrolling was enabled

        SSD1306_SET_DISP_ON, // turn display on
    };
    _ssd1306_send_cmds(cmds, sizeof(cmds));

    // zero the entire display
    oled_clear(false);
    _ssd1306_update_sync();

    i2c_bus_release();
}


bool oled_present()
{
    return g_oled_detected;
}


void oled_clear(bool value)
{
    memset(g_oled_data, (value ? 0xFF : 0x00), sizeof(g_oled_data));
}


void oled_draw_rect(uint x, uint y, uint w, uint h, bool value)
{
    for (uint x_ = x; x_ < w; ++x_) {
        _buffer_set_pixel(x_, y, value);
        _buffer_set_pixel(x_, y+h, value);
    }
    for (uint y_ = y+1; y_ < h-2; ++y_) {
        _buffer_set_pixel(x, y_, value);
        _buffer_set_pixel(x+w, y_+h, value);
    }
}

void oled_fill_rect(uint x, uint y, uint w, uint h, bool value)
{
    for (uint y_ = y; y_ < h; ++y_) {
        for (uint x_ = x; x_ < w; ++x_) {
            _buffer_set_pixel(x_, y_, value);
        }
    }
}


void oled_update()
{
    i2c_bus_acquire_blocking();
    _ssd1306_update_async();    
}


static void test()
{
    printf("OLED Test");
    for (int i = 0; i < 20; i++) {
        _ssd1306_send_cmd(0xA5); // ignore RAM, all pixels on
        sleep_ms(500);
        _ssd1306_send_cmd(0xA4); // go back to following RAM
        sleep_ms(500);
    }
}



void ascii_buffer()
{
    printf("-------------------------------------\n");
    for (uint i=0; i<sizeof(g_oled_data); ++i) {
        uint row = i/(OLED_WIDTH/8);
        uint8_t p = g_oled_data[i];
        if ((i%(OLED_WIDTH/8))==0)
            printf("\n%02d: ", row);
        printf("%02x", p);
    }
}


void oled_test()
{
    static bool toggle = false;
    printf("OLED\n");

    memset(g_oled_data, 0x00, sizeof(g_oled_data));

    uint8_t *data = g_oled_data;
    for (uint y=0; y<OLED_HEIGHT/2; y+=4) {
        uint8_t *row = &g_oled_data[y*OLED_WIDTH/8];

        for (uint x=0; x<OLED_WIDTH/8/2; ++x) {
            row[x] = 0xFF;
            row[x+OLED_WIDTH/8/2] = 0xAA;
        }
    }

    //ascii_buffer();

    toggle = !toggle;

    i2c_bus_acquire_blocking();
    _ssd1306_update_async();
}
#endif
