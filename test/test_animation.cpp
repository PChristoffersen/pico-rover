#include <iostream>
#include <gtest/gtest.h>

#include <led/color.h>
#include <led/colorbuffer.h>
#include <led/colorlayer.h>
#include <led/animation/indicators.h>

using namespace LED;

using layer_type = Color::Layer<Color::RGBA, LED_STRIP_PIXEL_COUNT>;
using indicator_type = Animation::Indicators<LED_STRIP_PIXEL_COUNT>;

static void expect_indicator_leds(const char *name, TickType_t time, const indicator_type &anim, layer_type &layer, uint lid)
{
    auto off_count = indicator_type::LED_COUNT-lid;
    for (uint i=0; i<indicator_type::LED_COUNT; i++) {
        if (i<off_count) {
            EXPECT_EQ(layer[i], Color::RGBA::TRANSPARENT) << name << " state=" << anim.get_state() << " time=" << time << " " << " led=" << i << std::endl << layer;
        }
        else {
            EXPECT_EQ(layer[i], indicator_type::COLOR) << name << " state=" << anim.get_state() << " time=" << time << " " << " led=" << i << std::endl << layer;
        }
    }
}

TEST(LEDAnimation, indicators) 
{
    layer_type layer;
    indicator_type anim { layer };
    TickType_t time = 0;

    EXPECT_FALSE(layer.is_dirty()) << "Initial dirty";
    EXPECT_FALSE(layer.is_visible()) << "Initial visibility";

    anim.set_mode(indicator_type::Mode::HAZARD);
    anim.start(time);
    EXPECT_TRUE(layer.is_dirty()) << "Start dirty";
    EXPECT_TRUE(layer.is_visible()) << "Start visible";
    expect_indicator_leds("initial", time, anim, layer, 0);
    layer.clear_dirty();

    // LEDS should be off for the first intervals
    for (uint i=0; i<indicator_type::OFF_INTERVALS; i++) {
        time += pdMS_TO_TICKS(indicator_type::INTERVAL);
        anim.update(time);
        EXPECT_TRUE(layer.is_dirty()) << "dirty";
        expect_indicator_leds("off", time, anim, layer, 0);
        layer.clear_dirty();
    }

    // LEDS should transition to on
    for (uint i=0; i<indicator_type::LED_COUNT; i++) {
        time += pdMS_TO_TICKS(indicator_type::INTERVAL);
        anim.update(time);
        EXPECT_TRUE(layer.is_dirty()) << "dirty";
        expect_indicator_leds("transition", time, anim, layer, i+1);
        layer.clear_dirty();
    }

    // LEDS should stay on
    for (uint i=0; i<indicator_type::ON_INTERVALS; i++) {
        time += pdMS_TO_TICKS(indicator_type::INTERVAL);
        anim.update(time);
        EXPECT_TRUE(layer.is_dirty()) << "dirty";
        expect_indicator_leds("on", time, anim, layer, indicator_type::LED_COUNT);
        layer.clear_dirty();
    }

    // Finally back to off
    time += pdMS_TO_TICKS(indicator_type::INTERVAL);
    anim.update(time);
    EXPECT_TRUE(layer.is_dirty()) << "dirty";
    expect_indicator_leds("final", time, anim, layer, 0);
    layer.clear_dirty();
}

