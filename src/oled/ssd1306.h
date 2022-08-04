/**
 * @author Peter Christoffersen
 * @brief SSD1306 registers 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c; 2022
 * 
 */
#pragma once

#include <pico/stdlib.h>
#include <hardware/i2c.h>


/* Control */
constexpr uint8_t SSD1306_CONTROL_COMMAND     = 0x80;
constexpr uint8_t SSD1306_CONTROL_DATA_STREAM = 0x40;
constexpr uint8_t SSD1306_CONTROL_SINGLE_DATA = 0xC0;

/* Commands */
constexpr uint8_t SSD1306_SET_CONTRAST = 0x81;
constexpr uint8_t SSD1306_SET_ENTIRE_ON = 0xA4;
constexpr uint8_t SSD1306_SET_NORM_INV = 0xA6;
constexpr uint8_t SSD1306_SET_DISP_ON = 0xAF;
constexpr uint8_t SSD1306_SET_DISP_OFF = 0xAE;
constexpr uint8_t SSD1306_SET_MEM_ADDR = 0x20;
constexpr uint8_t SSD1306_SET_COL_ADDR = 0x21;
constexpr uint8_t SSD1306_SET_PAGE_ADDR = 0x22;
constexpr uint8_t SSD1306_SET_DISP_START_LINE = 0x40;
constexpr uint8_t SSD1306_SET_SEG_REMAP_SEG0 = 0xA0;
constexpr uint8_t SSD1306_SET_SEG_REMAP_SEG127 = 0xA1;
constexpr uint8_t SSD1306_SET_MUX_RATIO = 0xA8;
constexpr uint8_t SSD1306_SET_COM_OUT_DIR_NORMAL = 0xC0;
constexpr uint8_t SSD1306_SET_COM_OUT_DIR_REMAP = 0xC8;
constexpr uint8_t SSD1306_SET_DISP_OFFSET = 0xD3;
constexpr uint8_t SSD1306_SET_COM_PIN_CFG = 0xDA;
constexpr uint8_t SSD1306_SET_DISP_CLK_DIV = 0xD5;
constexpr uint8_t SSD1306_SET_PRECHARGE = 0xD9;
constexpr uint8_t SSD1306_SET_VCOM_DESEL = 0xDB;
constexpr uint8_t SSD1306_SET_CHARGE_PUMP = 0x8D;
constexpr uint8_t SSD1306_SET_HORIZ_SCROLL = 0x26;
constexpr uint8_t SSD1306_SET_SCROLL_OFF = 0x2E;

constexpr uint8_t SSD1306_MEMORYMODE_HORZONTAL = 0b00;
constexpr uint8_t SSD1306_MEMORYMODE_VERTICAL  = 0b01;
constexpr uint8_t SSD1306_MEMORYMODE_PAGE      = 0b11;

/* Misc */
constexpr uint8_t SSD1306_DEFAULT_ADDR = 0x3C;
constexpr uint8_t SSD1306_PAGE_HEIGHT = 8;



