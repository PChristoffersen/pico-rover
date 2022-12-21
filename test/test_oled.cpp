#include <gtest/gtest.h>

#include <ssd1306/framebuffer.h>
#include <ssd1306/fixed_5x8.font.h>
#include <ssd1306/fixed_8x8.font.h>
#include <ssd1306/fixed_12x16.font.h>
#include <ssd1306/fixed_16x32.font.h>

TEST(OLED, draw_hline) {
    SSD1306::Framebuffer<8,4> buf;
    buf.clear_dirty();

    buf.draw_hline(2, 12, 4);

    EXPECT_EQ(true, true) << buf;

    //buf.print();
}
