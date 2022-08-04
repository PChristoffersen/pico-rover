/**
 * @author Peter Christoffersen
 * @brief OLED 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>
#include <pico/stdlib.h>

void oled_init();
bool oled_present();

void oled_draw_rect(uint x, uint y, uint w, uint h, bool value);
void oled_fill_rect(uint x, uint y, uint w, uint h, bool value);
void oled_clear(bool value);

void oled_update();

void oled_test();

class OLEDDisplay {
    public:
        OLEDDisplay();

        void init();
        bool present() { return m_present; }

        void clear();
        void draw_rect(uint x, uint y, uint w, uint h);
        void fill_rect(uint x, uint y, uint w, uint h);

        void update();

    private:

        bool m_present;
};
