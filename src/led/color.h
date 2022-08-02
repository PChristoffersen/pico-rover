/**
 * @author Peter Christoffersen
 * @brief LED Color util 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef uint32_t color_t; 

#define COLOR_BLUE_SHIFT   0u
#define COLOR_BLUE_MASK    0x000000FF
#define COLOR_RED_SHIFT    8u
#define COLOR_RED_MASK     0x0000FF00
#define COLOR_GREEN_SHIFT  16u
#define COLOR_GREEN_MASK   0x00FF0000
#define COLOR_WHITE_SHIFT  24u
#define COLOR_WHITE_MASK   0xFF000000
#define COLOR_RGB_SHIFT    0u
#define COLOR_RGB_MASK     0x00FFFFFF
#define COLOR_CHANNEL_MASK 0xFF

#define COLOR_BLUE_CHANNEL(color) ((color>>COLOR_BLUE_SHIFT)&COLOR_CHANNEL_MASK)
#define COLOR_RED_CHANNEL(color) ((color>>COLOR_RED_SHIFT)&COLOR_CHANNEL_MASK)
#define COLOR_GREEN_CHANNEL(color) ((color>>COLOR_GREEN_SHIFT)&COLOR_CHANNEL_MASK)
#define COLOR_WHITE_CHANNEL(color) ((color>>COLOR_WHITE_SHIFT)&COLOR_CHANNEL_MASK)
#define COLOR_RGB_CHANNEL(color) ((color>>COLOR_RGB_SHIFT)&COLOR_RGB_MASK)

static inline color_t color_rgb(uint8_t r, uint8_t g, uint8_t b) 
{
    return (((color_t)b)<<COLOR_BLUE_SHIFT) | (((color_t)r)<<COLOR_RED_SHIFT) | (((color_t)g)<<COLOR_GREEN_SHIFT);
}
static inline color_t color_rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w) 
{
    return color_rgb(r,g,b) | (((color_t)w)<<COLOR_WHITE_SHIFT);
}


#define COLOR_BLACK   color_rgb(0x00, 0x00, 0x00)
#define COLOR_WHITE   color_rgb(0xFF, 0xFF, 0xFF)
#define COLOR_RED     color_rgb(0xFF, 0x00, 0x00)
#define COLOR_GREEN   color_rgb(0x00, 0xFF, 0x00)
#define COLOR_BLUE    color_rgb(0x00, 0x00, 0xFF)
#define COLOR_YELLOW  color_rgb(0xFF, 0xFF, 0x00)
#define COLOR_CYAN    color_rgb(0x00, 0xFF, 0xFF)
#define COLOR_MAGENTA color_rgb(0xFF, 0x00, 0xFF)


typedef uint32_t color_correction_t;

static inline color_t color_correct(color_t color, color_correction_t correction)
{
    color_t red   = ( (color & COLOR_RED_MASK)   * COLOR_RED_CHANNEL(correction)   / COLOR_CHANNEL_MASK ) & COLOR_RED_MASK;
    color_t green = ( (color & COLOR_GREEN_MASK) * COLOR_GREEN_CHANNEL(correction) / COLOR_CHANNEL_MASK ) & COLOR_GREEN_MASK;
    color_t blue  = ( (color & COLOR_BLUE_MASK)  * COLOR_BLUE_CHANNEL(correction)  / COLOR_CHANNEL_MASK ) & COLOR_BLUE_MASK;
    color_t white = color & COLOR_WHITE_MASK;
    return white | red | green | blue;
}

/// typical values for SMD5050 LEDs
#define COLOR_CORRECTION_TypicalSMD5050 color_rgb(255, 176, 240)
/// typical values for generic LED strips
#define COLOR_CORRECTION_TypicalLEDStrip color_rgb(255, 176, 240)
/// typical values for 8mm "pixels on a string"
/// also for many through-hole 'T' package LEDs
#define COLOR_CORRECTION_Typical8mmPixel color_rgb(255, 224, 140)
#define COLOR_CORRECTION_TypicalPixelString color_rgb(255, 224, 140)
/// uncorrected color
#define COLOR_CORRECTION_UncorrectedColor = color_rgb(255,255,255)






#ifdef __cplusplus
}
#endif

