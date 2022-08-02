/**
 * @author Peter Christoffersen
 * @brief SSD1306 registers 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <pico/stdlib.h>
#include <hardware/i2c.h>


/* Control */
#define SSD1306_CONTROL_COMMAND     _u(0x80)
#define SSD1306_CONTROL_DATA_STREAM _u(0x40)
#define SSD1306_CONTROL_SINGLE_DATA _u(0xC0)

/* Commands */
#define SSD1306_SET_CONTRAST _u(0x81)
#define SSD1306_SET_ENTIRE_ON _u(0xA4)
#define SSD1306_SET_NORM_INV _u(0xA6)
#define SSD1306_SET_DISP _u(0xAE)
#define SSD1306_SET_MEM_ADDR _u(0x20)
#define SSD1306_SET_COL_ADDR _u(0x21)
#define SSD1306_SET_PAGE_ADDR _u(0x22)
#define SSD1306_SET_DISP_START_LINE _u(0x40)
#define SSD1306_SET_SEG_REMAP _u(0xA0)
#define SSD1306_SET_MUX_RATIO _u(0xA8)
#define SSD1306_SET_COM_OUT_DIR _u(0xC0)
#define SSD1306_SET_DISP_OFFSET _u(0xD3)
#define SSD1306_SET_COM_PIN_CFG _u(0xDA)
#define SSD1306_SET_DISP_CLK_DIV _u(0xD5)
#define SSD1306_SET_PRECHARGE _u(0xD9)
#define SSD1306_SET_VCOM_DESEL _u(0xDB)
#define SSD1306_SET_CHARGE_PUMP _u(0x8D)
#define SSD1306_SET_HORIZ_SCROLL _u(0x26)
#define SSD1306_SET_SCROLL _u(0x2E)


/* Misc */
#define SSD1306_DEFAULT_ADDR _u(0x3C)
#define SSD1306_PAGE_HEIGHT _u(8)



