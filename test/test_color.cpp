
#include <pico/test.h>
#include <led/color.h>

PICOTEST_MODULE_NAME("color_test", "Color test harness");


int test_rgb() 
{
    LED::Color::RGB c;

    c = LED::Color::RGB::BLACK;
    PICOTEST_CHECK(c.red()==0x00,   "Black(red)");
    PICOTEST_CHECK(c.green()==0x00, "Black(green)");
    PICOTEST_CHECK(c.blue()==0x00,  "Black(blue)");
    PICOTEST_CHECK(c.alpha()==0xFF, "Black(alpha)");

    c = LED::Color::RGB::RED;
    PICOTEST_CHECK(c.red()==0xFF,   "Red(red)");
    PICOTEST_CHECK(c.green()==0x00, "Red(green)");
    PICOTEST_CHECK(c.blue()==0x00,  "Red(blue)");
    PICOTEST_CHECK(c.alpha()==0xFF, "Red(alpha)");

    c = LED::Color::RGB::GREEN;
    PICOTEST_CHECK(c.red()==0x00,   "Green(red)");
    PICOTEST_CHECK(c.green()==0xFF, "Green(green)");
    PICOTEST_CHECK(c.blue()==0x00,  "Green(blue)");
    PICOTEST_CHECK(c.alpha()==0xFF, "Green(alpha)");

    c = LED::Color::RGB::BLUE;
    PICOTEST_CHECK(c.red()==0x00,   "Blue(red)");
    PICOTEST_CHECK(c.green()==0x00, "Blue(green)");
    PICOTEST_CHECK(c.blue()==0xFF,  "Blue(blue)");
    PICOTEST_CHECK(c.alpha()==0xFF, "Blue(alpha)");

    return 0;
}


int test_rgba() 
{
    LED::Color::RGBA c;

    c = LED::Color::RGBA::TRANSPARENT;
    PICOTEST_CHECK(c.red()==0x00,   "Transparent(red)");
    PICOTEST_CHECK(c.green()==0x00, "Transparent(green)");
    PICOTEST_CHECK(c.blue()==0x00,  "Transparent(blue)");
    PICOTEST_CHECK(c.alpha()==0x00, "Transparent(alpha)");

    c = LED::Color::RGB::BLACK;
    PICOTEST_CHECK(c.red()==0x00,   "Black(red)");
    PICOTEST_CHECK(c.green()==0x00, "Black(green)");
    PICOTEST_CHECK(c.blue()==0x00,  "Black(blue)");
    PICOTEST_CHECK(c.alpha()==0xFF, "Black(alpha)");

    c = LED::Color::RGB::RED;
    PICOTEST_CHECK(c.red()==0xFF,   "Red(red)");
    PICOTEST_CHECK(c.green()==0x00, "Red(green)");
    PICOTEST_CHECK(c.blue()==0x00,  "Red(blue)");
    PICOTEST_CHECK(c.alpha()==0xFF, "Red(alpha)");

    c = LED::Color::RGB::GREEN;
    PICOTEST_CHECK(c.red()==0x00,   "Green(red)");
    PICOTEST_CHECK(c.green()==0xFF, "Green(green)");
    PICOTEST_CHECK(c.blue()==0x00,  "Green(blue)");
    PICOTEST_CHECK(c.alpha()==0xFF, "Green(alpha)");

    c = LED::Color::RGB::BLUE;
    PICOTEST_CHECK(c.red()==0x00,   "Blue(red)");
    PICOTEST_CHECK(c.green()==0x00, "Blue(green)");
    PICOTEST_CHECK(c.blue()==0xFF,  "Blue(blue)");
    PICOTEST_CHECK(c.alpha()==0xFF, "Blue(alpha)");

    return 0;
}


int test_rgbw() 
{
    LED::Color::RGBW c;

    c = LED::Color::RGBA::TRANSPARENT;
    PICOTEST_CHECK(c.red()==0x00,   "Transparent(red)");
    PICOTEST_CHECK(c.green()==0x00, "Transparent(green)");
    PICOTEST_CHECK(c.blue()==0x00,  "Transparent(blue)");
    PICOTEST_CHECK(c.alpha()==0xFF, "Transparent(alpha)");
    PICOTEST_CHECK(c.white()==0x00, "Transparent(white)");

    c = LED::Color::RGB::BLACK;
    PICOTEST_CHECK(c.red()==0x00,   "Black(red)");
    PICOTEST_CHECK(c.green()==0x00, "Black(green)");
    PICOTEST_CHECK(c.blue()==0x00,  "Black(blue)");
    PICOTEST_CHECK(c.alpha()==0xFF, "Black(alpha)");
    PICOTEST_CHECK(c.white()==0x00, "Black(white)");

    c = LED::Color::RGB::RED;
    PICOTEST_CHECK(c.red()==0xFF,   "Red(red)");
    PICOTEST_CHECK(c.green()==0x00, "Red(green)");
    PICOTEST_CHECK(c.blue()==0x00,  "Red(blue)");
    PICOTEST_CHECK(c.alpha()==0xFF, "Red(alpha)");
    PICOTEST_CHECK(c.white()==0x00, "Red(white)");

    c = LED::Color::RGB::GREEN;
    PICOTEST_CHECK(c.red()==0x00,   "Green(red)");
    PICOTEST_CHECK(c.green()==0xFF, "Green(green)");
    PICOTEST_CHECK(c.blue()==0x00,  "Green(blue)");
    PICOTEST_CHECK(c.alpha()==0xFF, "Green(alpha)");
    PICOTEST_CHECK(c.white()==0x00, "Green(white)");

    c = LED::Color::RGB::BLUE;
    PICOTEST_CHECK(c.red()==0x00,   "Blue(red)");
    PICOTEST_CHECK(c.green()==0x00, "Blue(green)");
    PICOTEST_CHECK(c.blue()==0xFF,  "Blue(blue)");
    PICOTEST_CHECK(c.alpha()==0xFF, "Blue(alpha)");
    PICOTEST_CHECK(c.white()==0x00, "Blue(white)");

    c = LED::Color::RGBW::WHITE;
    PICOTEST_CHECK(c.red()==0xFF,   "Blue(red)");
    PICOTEST_CHECK(c.green()==0xFF, "Blue(green)");
    PICOTEST_CHECK(c.blue()==0xFF,  "Blue(blue)");
    PICOTEST_CHECK(c.alpha()==0xFF, "Blue(alpha)");
    PICOTEST_CHECK(c.white()==0xFF, "Blue(white)");

    return 0;
}


int main()
{
    PICOTEST_START();

    PICOTEST_START_SECTION("RGB");
    test_rgb();
    PICOTEST_END_SECTION()

    PICOTEST_START_SECTION("RGBA");
    test_rgba();
    PICOTEST_END_SECTION()

    PICOTEST_START_SECTION("RGBW");
    test_rgbw();
    PICOTEST_END_SECTION()

    PICOTEST_END_TEST()

    return 0;
}