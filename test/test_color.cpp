
#include <gtest/gtest.h>
#include <led/color.h>


TEST(Color, RGB) {
    LED::Color::RGB c;

    c = LED::Color::RGB::BLACK;
    EXPECT_EQ(c.red(),   0x00) << "Black(red) " << c;
    EXPECT_EQ(c.green(), 0x00) << "Black(green) " << c;
    EXPECT_EQ(c.blue(),  0x00) << "Black(blue) " << c;
    EXPECT_EQ(c.alpha(), 0xFF) << "Black(alpha) " << c;

    c = LED::Color::RGB::RED;
    EXPECT_EQ(c.red(),   0xFF) << "Red(red) " << c;
    EXPECT_EQ(c.green(), 0x00) << "Red(green) " << c;
    EXPECT_EQ(c.blue(),  0x00) << "Red(blue) " << c;
    EXPECT_EQ(c.alpha(), 0xFF) << "Red(alpha) " << c;

    c = LED::Color::RGB::GREEN;
    EXPECT_EQ(c.red(),   0x00) << "Green(red) " << c;
    EXPECT_EQ(c.green(), 0xFF) << "Green(green) " << c;
    EXPECT_EQ(c.blue(),  0x00) << "Green(blue) " << c;
    EXPECT_EQ(c.alpha(), 0xFF) << "Green(alpha) " << c;

    c = LED::Color::RGB::BLUE;
    EXPECT_EQ(c.red(),   0x00) << "Blue(red) " << c;
    EXPECT_EQ(c.green(), 0x00) << "Blue(green) " << c;
    EXPECT_EQ(c.blue(),  0xFF) << "Blue(blue) " << c;
    EXPECT_EQ(c.alpha(), 0xFF) << "Blue(alpha) " << c;
}


TEST(Color, RGBA) {
    LED::Color::RGBA c;

    c = LED::Color::RGBA::TRANSPARENT;
    EXPECT_EQ(c.red(),   0x00) << "Transparent(red) " << c;
    EXPECT_EQ(c.green(), 0x00) << "Transparent(green) " << c;
    EXPECT_EQ(c.blue(),  0x00) << "Transparent(blue) " << c;
    EXPECT_EQ(c.alpha(), 0x00) << "Transparent(alpha) " << c;

    c = LED::Color::RGB::BLACK;
    EXPECT_EQ(c.red(),   0x00) << "Black(red) " << c;
    EXPECT_EQ(c.green(), 0x00) << "Black(green) " << c;
    EXPECT_EQ(c.blue(),  0x00) << "Black(blue) " << c;
    EXPECT_EQ(c.alpha(), 0xFF) << "Black(alpha) " << c;

    c = LED::Color::RGB::RED;
    EXPECT_EQ(c.red(),   0xFF) << "Red(red) " << c;
    EXPECT_EQ(c.green(), 0x00) << "Red(green) " << c;
    EXPECT_EQ(c.blue(),  0x00) << "Red(blue) " << c;
    EXPECT_EQ(c.alpha(), 0xFF) << "Red(alpha) " << c;

    c = LED::Color::RGB::GREEN;
    EXPECT_EQ(c.red(),   0x00) << "Green(red) " << c;
    EXPECT_EQ(c.green(), 0xFF) << "Green(green) " << c;
    EXPECT_EQ(c.blue(),  0x00) << "Green(blue) " << c;
    EXPECT_EQ(c.alpha(), 0xFF) << "Green(alpha) " << c;

    c = LED::Color::RGB::BLUE;
    EXPECT_EQ(c.red(),   0x00) << "Blue(red) " << c;
    EXPECT_EQ(c.green(), 0x00) << "Blue(green) " << c;
    EXPECT_EQ(c.blue(),  0xFF) << "Blue(blue) " << c;
    EXPECT_EQ(c.alpha(), 0xFF) << "Blue(alpha) " << c;

}


TEST(Color, RGBW) {
    LED::Color::RGBW c;

    c = LED::Color::RGBA::TRANSPARENT;
    EXPECT_EQ(c.red(),   0x00) << "Transparent(red) " << c;
    EXPECT_EQ(c.green(), 0x00) << "Transparent(green) " << c;
    EXPECT_EQ(c.blue(),  0x00) << "Transparent(blue) " << c;
    EXPECT_EQ(c.alpha(), 0xFF) << "Transparent(alpha) " << c;
    EXPECT_EQ(c.white(), 0x00) << "Transparent(white) " << c;

    c = LED::Color::RGB::BLACK;
    EXPECT_EQ(c.red(),   0x00) << "Black(red) " << c;
    EXPECT_EQ(c.green(), 0x00) << "Black(green) " << c;
    EXPECT_EQ(c.blue(),  0x00) << "Black(blue) " << c;
    EXPECT_EQ(c.alpha(), 0xFF) << "Black(alpha) " << c;
    EXPECT_EQ(c.white(), 0x00) << "Black(white) " << c;

    c = LED::Color::RGB::RED;
    EXPECT_EQ(c.red(),   0xFF) << "Red(red) " << c;
    EXPECT_EQ(c.green(), 0x00) << "Red(green) " << c;
    EXPECT_EQ(c.blue(),  0x00) << "Red(blue) " << c;
    EXPECT_EQ(c.alpha(), 0xFF) << "Red(alpha) " << c;
    EXPECT_EQ(c.white(), 0x00) << "Red(white) " << c;

    c = LED::Color::RGB::GREEN;
    EXPECT_EQ(c.red(),   0x00) << "Green(red) " << c;
    EXPECT_EQ(c.green(), 0xFF) << "Green(green) " << c;
    EXPECT_EQ(c.blue(),  0x00) << "Green(blue) " << c;
    EXPECT_EQ(c.alpha(), 0xFF) << "Green(alpha) " << c;
    EXPECT_EQ(c.white(), 0x00) << "Green(white) " << c;

    c = LED::Color::RGB::BLUE;
    EXPECT_EQ(c.red(),   0x00) << "Blue(red) " << c;
    EXPECT_EQ(c.green(), 0x00) << "Blue(green) " << c;
    EXPECT_EQ(c.blue(),  0xFF) << "Blue(blue) " << c;
    EXPECT_EQ(c.alpha(), 0xFF) << "Blue(alpha) " << c;
    EXPECT_EQ(c.white(), 0x00) << "Blue(white) " << c;

    c = LED::Color::RGBW::WHITE;
    EXPECT_EQ(c.red(),   0xFF) << "Blue(red) " << c;
    EXPECT_EQ(c.green(), 0xFF) << "Blue(green) " << c;
    EXPECT_EQ(c.blue(),  0xFF) << "Blue(blue) " << c;
    EXPECT_EQ(c.alpha(), 0xFF) << "Blue(alpha) " << c;
    EXPECT_EQ(c.white(), 0xFF) << "Blue(white) " << c;

}


TEST(Color, HSV) {
    LED::Color::RGB c;

    c = LED::Color::HSV(0x00, 0x00, 0xFF);
    EXPECT_EQ(c.red(),   0xFF) << "White(red) " << c;
    EXPECT_EQ(c.green(), 0xFF) << "White(green) " << c;
    EXPECT_EQ(c.blue(),  0xFF) << "White(blue) " << c;

    c = LED::Color::HSV(0x00, 0x00, 0x00);
    EXPECT_EQ(c.red(),   0x00) << "Black(red) " << c;
    EXPECT_EQ(c.green(), 0x00) << "Black(green) " << c;
    EXPECT_EQ(c.blue(),  0x00) << "Black(blue) " << c;

    c = LED::Color::HSV(0x00, 0xFF, 0xFF);
    EXPECT_EQ(c.red(),   0xFF) << "Red(red) " << c;
    EXPECT_EQ(c.green(), 0x00) << "Red(green) " << c;
    EXPECT_EQ(c.blue(),  0x00) << "Red(blue) " << c;

    c = LED::Color::HSV(0x60, 0xFF, 0xFF);
    EXPECT_EQ(c.red(),   0x00) << "Green(red) " << c;
    EXPECT_EQ(c.green(), 0xFF) << "Green(green) " << c;
    EXPECT_EQ(c.blue(),  0x00) << "Green(blue) " << c;

    c = LED::Color::HSV(0xA0, 0xFF, 0xFF);
    EXPECT_EQ(c.red(),   0x00) << "Blue(red) " << c;
    EXPECT_EQ(c.green(), 0x00) << "Blue(green) " << c;
    EXPECT_EQ(c.blue(),  0xFF) << "Blue(blue) " << c;
}

TEST(Color, Blend) {
    using namespace LED::Color;

    RGBW dst;

    dst = RGB::GREEN;
    dst << RGBA::TRANSPARENT;
    EXPECT_EQ(dst.red(),   0x00) << "Transparent(red) " << dst;
    EXPECT_EQ(dst.green(), 0xFF) << "Transparent(green) " << dst;
    EXPECT_EQ(dst.blue(),  0x00) << "Transparent(blue) " << dst;
    EXPECT_EQ(dst.white(), 0x00) << "Transparent(white) " << dst;

    dst = RGB::GREEN;
    dst << RGBA(0xFF, 0x00, 0x00, 0x80);
    EXPECT_EQ(dst.red(),   0x80) << "Red(red) " << dst;
    EXPECT_EQ(dst.green(), 0x80) << "Red(green) " << dst;
    EXPECT_EQ(dst.blue(),  0x00) << "Red(blue) " << dst;
    EXPECT_EQ(dst.white(), 0x00) << "Red(white) " << dst;

    dst = RGB::GREEN;
    dst << RGBA(0x00, 0xFF, 0x00, 0x80);
    EXPECT_EQ(dst.red(),   0x00) << "Green(red) " << dst;
    EXPECT_EQ(dst.green(), 0xFF) << "Green(green) " << dst;
    EXPECT_EQ(dst.blue(),  0x00) << "Green(blue) " << dst;
    EXPECT_EQ(dst.white(), 0x00) << "Green(white) " << dst;

    dst = RGB::GREEN;
    dst << RGBA(0x00, 0x00, 0xFF, 0x80);
    EXPECT_EQ(dst.red(),   0x00) << "Blue(red) " << dst;
    EXPECT_EQ(dst.green(), 0x80) << "Blue(green) " << dst;
    EXPECT_EQ(dst.blue(),  0x80) << "Blue(blue) " << dst;
    EXPECT_EQ(dst.white(), 0x00) << "Blue(white) " << dst;

    dst = RGB::GREEN;
    dst << RGBA(0xFF, 0xFF, 0xFF, 0x80);
    EXPECT_EQ(dst.red(),   0x80) << "White(red) " << dst;
    EXPECT_EQ(dst.green(), 0xFF) << "White(green) " << dst;
    EXPECT_EQ(dst.blue(),  0x80) << "White(blue) " << dst;
    EXPECT_EQ(dst.white(), 0x00) << "White(white) " << dst;


    dst = RGB::WHITE;
    dst << RGBA(0xFF, 0xFF, 0xFF, 0x80);
    EXPECT_EQ(dst.red(),   0xFF) << "White2(red) " << dst;
    EXPECT_EQ(dst.green(), 0xFF) << "White2(green) " << dst;
    EXPECT_EQ(dst.blue(),  0xFF) << "White2(blue) " << dst;
    EXPECT_EQ(dst.white(), 0x00) << "White2(white) " << dst;
}

