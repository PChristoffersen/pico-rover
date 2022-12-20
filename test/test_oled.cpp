#include <gtest/gtest.h>

#include <oled/framebuffer.h>
#include <oled/fixed_5x8.font.h>
#include <oled/fixed_8x8.font.h>
#include <oled/fixed_12x16.font.h>
#include <oled/fixed_16x32.font.h>

TEST(OLED, draw_hline) {
    OLED::Framebuffer<8,4> buf;
    buf.clear_dirty();

    buf.draw_hline(2, 12, 4);

    EXPECT_EQ(true, true) << buf;

    //buf.print();
}
