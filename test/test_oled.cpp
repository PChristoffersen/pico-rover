#include <gtest/gtest.h>

#include <oled/framebuffer.h>

TEST(OLED, draw_hline) {
    OLED::Framebuffer<8,4> buf;
    buf.clear_dirty();

    buf.draw_hline(2, 12, 4);

    EXPECT_EQ(true, true) << buf;

    //buf.print();
}

#if 0
#include <stdio.h>
#include <oled/framebuffer.h>
#include <resources/raspberry_logo.image.h>
#include <resources/battery.image.h>
#include <resources/noconnection.image.h>
#include <resources/warning.image.h>
#include <resources/signal1.image.h>
#include <resources/signal2.image.h>
#include <resources/signal3.image.h>
#include <resources/signal4.image.h>


#include <resources/liberationmono_16.font.h>
#include <resources/liberationmono_24.font.h>
#include <resources/liberationsans_16.font.h>
#include <resources/liberationsans_24.font.h>

#include <oled/fixed_5x8.font.h>
#include <oled/fixed_8x8.font.h>
#include <oled/fixed_12x16.font.h>
#include <oled/fixed_16x32.font.h>



#include <util/battery.h>


void update_battery(OLED::Framebuffer &buffer) 
{
    namespace Image = OLED::Resource::Image;
    namespace Font = OLED::Resource::Font;

    constexpr int BAR_HEIGHT { 32 };

    float percent = 1.0f;
    float voltage = 7.58f;
    float current = 0.15f;



    buffer.fill_rect(0, 0, Image::Battery.width(),buffer.height(), OLED::Framebuffer::DrawOp::SUBTRACT);

    // Icon
    {
        buffer.draw_bitmap(0, 0, Image::Battery);
        int bar_h = BAR_HEIGHT*percent+0.5f;
        buffer.fill_rect(7, 10+BAR_HEIGHT-bar_h, Image::Battery.width()-14, bar_h);
    }

    #if 0
    // Percent
    {
        const OLED::Font &font = Font::Fixed_8x8;
        char text[8];
        sprintf(text, "%u%%", static_cast<uint>(100.0f*percent+0.5));
        buffer.draw_text((Image::Battery.width()-font.width(text))/2, Image::Battery.height(), text, font);
    }
    #endif

    // Voltage and amps
    {
        const OLED::Font &font = Font::Fixed_8x8;
        char text[16];
        sprintf(text, "%.1fV", voltage);
        buffer.draw_text(0, Image::Battery.height(), text, font);
        buffer.draw_text(Image::Battery.width(), 0, text, Font::LiberationSans_16);
        buffer.draw_text(Image::Battery.width(), 16, text, Font::LiberationSans_16);
        sprintf(text, "%.1fA", current);
        buffer.draw_text(0, Image::Battery.height()+8, text, font);
    }


}

int main(int argc, char **argv) {
    namespace Image = OLED::Resource::Image;
    namespace Font = OLED::Resource::Font;


    OLED::Framebuffer buf;
    buf.clear_dirty();

    #if 0
    buf.draw_hline(2, 12, 4);
    buf.draw_hline(30, 10, 4);
    buf.draw_vline(8, 10, 4);
    buf.draw_rect(0,8, 40, 8);
    #endif
    //buf.fill_rect(4, 9, 10, 6);
    //buf.fill_rect(24, 9, 10, 12);
    //buf.fill_rect(44, 2, 10, 20);
    //buf.fill_rect(4, 8, 10, 23);
    //buf.draw_bitmap(128-Image::RSSI5.width(), 8, Image::RSSI5);
    //buf.draw_bitmap(0, -16, OLED::raspberry26x32_image);
    //buf.draw_bitmap(32, 0, OLED::raspberry26x32_image);
    //buf.draw_bitmap(64, 16, OLED::raspberry26x32_image, true);
    //buf.draw_bitmap(96, 32+16, OLED::raspberry26x32_image);
    //buf.draw_text(0, 0, "!\"#$ABC", Font::Fixed_5x8, false);
    //const char *text = "A";
    //const char *text = "Hello world 1234";
    /*
    const char *text = "7.96V";
    buf.fill_rect(0,0,128,8);
    buf.draw_text(4, 0, "94%", Font::Fixed_8x8, OLED::Framebuffer::DrawOp::SUBTRACT);
    text = "7.58V";
    buf.draw_text((buf.width()-Font::Fixed_8x8.width(text))/2, 0, text, Font::Fixed_8x8, OLED::Framebuffer::DrawOp::SUBTRACT);
    */

    //buf.draw_text(50, 64-16, "Hello World", Font::LiberationSans_16);

    int x = 0;
    //buf.draw_bitmap(x, 0, Image::Signal1); x+=16;
    //buf.draw_bitmap(x, 0, Image::Signal2); x+=16;
    //buf.draw_bitmap(x, 0, Image::Signal3); x+=16;
    //buf.draw_bitmap(x, 0, Image::Signal4); x+=16;
    //buf.draw_bitmap(x, 0, Image::NoConnection); x+=16;
    //buf.draw_bitmap(x, 0, Image::Warning); x+=16;
    //buf.draw_bitmap(x, 0, Image::Siren); x+=16;
    //buf.draw_bitmap(x, 0, Image::BulbOn); x+=16;
    //buf.draw_bitmap(x, 0, Image::BulbOff); x+=16;

    buf.draw_bitmap(x, -1, Image::Signal4); x+=32;
    //buf.draw_bitmap(x,  0, Image::Signal4); x+=32;
    //buf.draw_bitmap(x,  4, Image::Signal4); x+=32;
    //buf.draw_bitmap(x,  12, Image::Signal4); x+=32;

    buf.draw_bitmap(32,  -1, Image::Raspberry_Logo);
    //buf.draw_bitmap(64, 0, Image::Raspberry_Logo);


    buf.print();


    return 0;
}
#endif
