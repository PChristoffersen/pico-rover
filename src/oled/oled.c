#include "oled.h"

#include "ssd1306.h"
#include "../boardconfig.h"



static bool g_oled_detected = false;

#define OLED_NUM_PAGES (OLED_HEIGHT / SSD1306_PAGE_HEIGHT)
#define OLED_BUF_LEN (OLED_NUM_PAGES * OLED_WIDTH)


struct render_area {
    uint8_t start_col;
    uint8_t end_col;
    uint8_t start_page;
    uint8_t end_page;

    int buflen;
};



// Plain:  2291
// NoStop: 2183
// Bulk: 1418


static inline void _ssd1306_send_cmd(uint8_t cmd) 
{
    // Co = 1, D/C = 0 => the driver expects a command
    uint8_t buf[2] = {SSD1306_CONTROL_COMMAND, cmd};
    i2c_write_blocking(i2c_default, OLED_ADDRESS, buf, sizeof(buf), false);
}

static inline void _ssd1306_send_cmd2(uint8_t cmd) 
{
    // Co = 1, D/C = 0 => the driver expects a command
    uint8_t buf[2] = {SSD1306_CONTROL_COMMAND, cmd};
    i2c_write_blocking(i2c_default, OLED_ADDRESS, buf, sizeof(buf), true);
}


static inline void _ssd1306_send_cmds(const uint8_t *cmds, uint count)
{
    uint8_t buf[count*2];
    for (uint i=0; i<count; ++i) {
        buf[2*i] = 0x80;
        buf[2*i+1] = cmds[i];
    }
    i2c_write_blocking(i2c_default, OLED_ADDRESS, buf, sizeof(buf), false);
}



void _ssd1306_send_buf(uint8_t buf[], int buflen) {
    // in horizontal addressing mode, the column address pointer auto-increments
    // and then wraps around to the next page, so we can send the entire frame
    // buffer in one gooooooo!

    // copy our frame buffer into a new buffer because we need to add the control byte
    // to the beginning

    // TODO find a more memory-efficient way to do this..
    // maybe break the data transfer into pages?
    uint8_t temp_buf[buflen + 1];

    for (int i = 1; i < buflen + 1; i++) {
        temp_buf[i] = buf[i - 1];
    }
    // Co = 0, D/C = 1 => the driver expects data to be written to RAM
    temp_buf[0] = SSD1306_CONTROL_DATA_STREAM;
    i2c_write_blocking(i2c_default, OLED_ADDRESS, temp_buf, buflen + 1, false);
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


void fill(uint8_t buf[], uint8_t fill) {
    // fill entire buffer with the same byte
    for (int i = 0; i < OLED_BUF_LEN; i++) {
        buf[i] = fill;
    }
};


void render(uint8_t *buf, struct render_area *area) {
    // update a portion of the display with a render area
    _ssd1306_send_cmd(SSD1306_SET_COL_ADDR);
    _ssd1306_send_cmd(area->start_col);
    _ssd1306_send_cmd(area->end_col);

    _ssd1306_send_cmd(SSD1306_SET_PAGE_ADDR);
    _ssd1306_send_cmd(area->start_page);
    _ssd1306_send_cmd(area->end_page);

    _ssd1306_send_buf(buf, area->buflen);
}


void calc_render_area_buflen(struct render_area *area) {
    // calculate how long the flattened buffer will be for a render area
    area->buflen = (area->end_col - area->start_col + 1) * (area->end_page - area->start_page + 1);
}



void oled_init() {
    if (!_oled_detect()) {
        // No OLED display detected
        g_oled_detected = false;
        return;
    }

    // some of these commands are not strictly necessary as the reset
    // process defaults to some of these but they are shown here
    // to demonstrate what the initialization sequence looks like

    // some configuration values are recommended by the board manufacturer

    #if 0

        SSD1306_SET_DISP | 0x00); // set display off

    /* memory mapping */
        SSD1306_SET_MEM_ADDR); // set memory address mode
        0x00); // horizontal addressing mode

    /* resolution and layout */
        SSD1306_SET_DISP_START_LINE); // set display start line to 0

        SSD1306_SET_SEG_REMAP | 0x01); // set segment re-map
    // column address 127 is mapped to SEG0

        SSD1306_SET_MUX_RATIO); // set multiplex ratio
        OLED_HEIGHT - 1); // our display is 64 pixels high

        SSD1306_SET_COM_OUT_DIR | 0x08); // set COM (common) output scan direction
    // scan from bottom up, COM[N-1] to COM0

        SSD1306_SET_DISP_OFFSET); // set display offset
        0x00); // no offset

        SSD1306_SET_COM_PIN_CFG); // set COM (common) pins hardware configuration
        0x02); // manufacturer magic number

    /* timing and driving scheme */
        SSD1306_SET_DISP_CLK_DIV); // set display clock divide ratio
        0x80); // div ratio of 1, standard freq

        SSD1306_SET_PRECHARGE); // set pre-charge period
        0xF1); // Vcc internally generated on our board

        SSD1306_SET_VCOM_DESEL); // set VCOMH deselect level
        0x30); // 0.83xVcc

    /* display */
        SSD1306_SET_CONTRAST); // set contrast control
        0xFF);

        SSD1306_SET_ENTIRE_ON); // set entire display on to follow RAM content

        SSD1306_SET_NORM_INV); // set normal (not inverted) display

        SSD1306_SET_CHARGE_PUMP); // set charge pump
        0x14); // Vcc internally generated on our board

        SSD1306_SET_SCROLL | 0x00); // deactivate horizontal scrolling if set
    // this is necessary as memory writes will corrupt if scrolling was enabled

    _ssd1306_send_cmd(SSD1306_SET_DISP | 0x01); // turn display on

    #else

    const uint8_t cmds[] = {
        SSD1306_SET_DISP | 0x00, // set display off

        /* memory mapping */
        SSD1306_SET_MEM_ADDR, // set memory address mode
        0x00, // horizontal addressing mode

        /* resolution and layout */
        SSD1306_SET_DISP_START_LINE, // set display start line to 0

        SSD1306_SET_SEG_REMAP | 0x01, // set segment re-map
        // column address 127 is mapped to SEG0

        SSD1306_SET_MUX_RATIO, // set multiplex ratio
        OLED_HEIGHT - 1, // our display is 64 pixels high

        SSD1306_SET_COM_OUT_DIR | 0x08, // set COM (common) output scan direction
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

        SSD1306_SET_SCROLL | 0x00, // deactivate horizontal scrolling if set
        // this is necessary as memory writes will corrupt if scrolling was enabled

        SSD1306_SET_DISP | 0x01, // turn display on
    };

    _ssd1306_send_cmds(cmds, sizeof(cmds));

    #endif

    struct render_area frame_area = {start_col: 0, end_col : OLED_WIDTH - 1, start_page : 0, end_page : OLED_NUM_PAGES -
                                                                                                        1};
    calc_render_area_buflen(&frame_area);


    // zero the entire display
    uint8_t buf[OLED_BUF_LEN];
    fill(buf, 0x00);
    render(buf, &frame_area);

    //test();
}



