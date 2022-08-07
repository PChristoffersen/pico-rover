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
#include <limits>
#include <algorithm>

namespace LED {

    class Color {
        public:
            using channel_type = uint8_t;
            using brightness_type = float;
            using raw_type     = uint32_t;


            enum class Correction : raw_type;

            static constexpr channel_type CHANNEL_MAX { std::numeric_limits<channel_type>::max() };
            static constexpr channel_type CHANNEL_MIN { std::numeric_limits<channel_type>::min() };

            static constexpr auto     BLUE_SHIFT  { 0 };
            static constexpr raw_type BLUE_MASK   { 0x000000FF };
            static constexpr auto     RED_SHIFT   { 8 };
            static constexpr raw_type RED_MASK    { 0x0000FF00 };
            static constexpr auto     GREEN_SHIFT { 16 };
            static constexpr raw_type GREEN_MASK  { 0x00FF0000 };
            static constexpr auto     WHITE_SHIFT { 24 };
            static constexpr raw_type WHITE_MASK  { 0xFF000000 };
            static constexpr raw_type CHANNEL_MASK { 0xFF };

            static constexpr brightness_type BRIGHTNESS_MAX { 1.0f };
            static constexpr brightness_type BRIGHTNESS_MIN { 0.0f };
            static constexpr brightness_type BRIGHTNESS_DEFAULT { BRIGHTNESS_MAX };

            static const Color BLACK;
            static const Color WHITE;
            static const Color RED;
            static const Color GREEN;
            static const Color BLUE;
            static const Color YELLOW;
            static const Color CYAN;
            static const Color MAGENTA;

            constexpr Color() : Color(BLACK) {}
            constexpr Color(const Color &color) : m_data{ color.m_data } {}
            constexpr Color(const Color &&color) : m_data{ color.m_data } {}
            constexpr Color(channel_type r, channel_type g, channel_type b) : m_data { raw_rgbw(r, g, b, CHANNEL_MIN) } {}
            constexpr Color(channel_type r, channel_type g, channel_type b, channel_type w) : m_data { raw_rgbw(r, g, b, w) } {}
            constexpr Color(int r, int g, int b) : m_data { raw_rgbw(to_channel(r), to_channel(g), to_channel(b), CHANNEL_MIN) } {}
            constexpr Color(int r, int g, int b, int w) : m_data { raw_rgbw(to_channel(r), to_channel(g), to_channel(b), to_channel(w)) } {}
            constexpr Color(unsigned int r, unsigned int g, unsigned int b) : m_data { raw_rgbw(to_channel(r), to_channel(g), to_channel(b), CHANNEL_MIN) } {}
            constexpr Color(unsigned int r, unsigned int g, unsigned int b, unsigned int w) : m_data { raw_rgbw(to_channel(r), to_channel(g), to_channel(b), to_channel(w)) } {}
            constexpr Color(float r, float g, float b) : m_data { raw_rgbw(to_channel(r), to_channel(g), to_channel(b), CHANNEL_MIN) } {}
            constexpr Color(float r, float g, float b, float w) : m_data { raw_rgbw(to_channel(r), to_channel(g), to_channel(b), to_channel(w)) } {}
            constexpr explicit Color(Correction correction) : m_data { static_cast<raw_type>(correction) } {}
            constexpr explicit Color(raw_type raw) : m_data { raw } {}

            constexpr inline channel_type red()   const { return (m_data >> RED_SHIFT) & CHANNEL_MASK; }
            constexpr inline channel_type green() const { return (m_data >> GREEN_SHIFT) & CHANNEL_MASK; }
            constexpr inline channel_type blue()  const { return (m_data >> BLUE_SHIFT) & CHANNEL_MASK; }
            constexpr inline channel_type white() const { return (m_data >> WHITE_SHIFT) & CHANNEL_MASK; }
            constexpr inline raw_type     rgb()   const { return (m_data&(GREEN_MASK|RED_MASK|BLUE_MASK)); }
            constexpr inline raw_type     raw()   const { return m_data; }

            static constexpr inline channel_type rawRed(raw_type c) { return (c >> RED_SHIFT) & CHANNEL_MASK; }
            static constexpr inline channel_type rawGreen(raw_type c) { return (c >> GREEN_SHIFT) & CHANNEL_MASK; }
            static constexpr inline channel_type rawBlue(raw_type c) { return (c >> BLUE_SHIFT) & CHANNEL_MASK; }
            static constexpr inline channel_type rawWhite(raw_type c) { return (c >> WHITE_SHIFT) & CHANNEL_MASK; }

            Color &operator=(const Color &other) { m_data = other.m_data; return *this; }
            Color &operator=(const raw_type other) { m_data = other; return *this; }

            Color &operator*=(const Correction correction) { *this = *this * correction; return *this; }
            Color &operator*=(const brightness_type brightness) { *this = *this * brightness; return *this; }

            constexpr Color operator*(const brightness_type brightness) const
            {
                raw_type scale = std::clamp<raw_type>(brightness*static_cast<brightness_type>(CHANNEL_MAX)+0.5f, CHANNEL_MIN, CHANNEL_MAX);
                if (scale == CHANNEL_MAX) {
                    return *this;
                }
                if (scale == CHANNEL_MIN) {
                    return Color { m_data & WHITE_MASK };
                }
                raw_type red   = ( (m_data & RED_MASK)   * scale / CHANNEL_MAX ) & RED_MASK;
                raw_type green = ( (m_data & GREEN_MASK) * scale / CHANNEL_MAX ) & GREEN_MASK;
                raw_type blue  = ( (m_data & BLUE_MASK)  * scale / CHANNEL_MAX ) & BLUE_MASK;
                raw_type white = ( ((m_data & WHITE_MASK)>>8) * scale / CHANNEL_MASK )<<8;
                return Color { red | green | blue | white};
            }

            constexpr Color operator*(const Correction correction) const 
            {
                raw_type red   = ( (m_data & RED_MASK)   * rawRed(static_cast<raw_type>(correction))   / CHANNEL_MAX ) & RED_MASK;
                raw_type green = ( (m_data & GREEN_MASK) * rawGreen(static_cast<raw_type>(correction)) / CHANNEL_MAX ) & GREEN_MASK;
                raw_type blue  = ( (m_data & BLUE_MASK)  * rawBlue(static_cast<raw_type>(correction))  / CHANNEL_MAX ) & BLUE_MASK;
                raw_type white = m_data & WHITE_MASK;
                return Color { red | green | blue | white };
            }



        private: 
            raw_type m_data;

            static constexpr inline raw_type raw_rgbw(raw_type r, raw_type g, raw_type b, raw_type w)
            {
                return w<<WHITE_SHIFT | r<<RED_SHIFT | g<<GREEN_SHIFT | b<<BLUE_SHIFT;
            }
            static constexpr inline channel_type to_channel(float v) { return std::clamp<float>(v*static_cast<float>(CHANNEL_MAX)+0.5f, CHANNEL_MIN, CHANNEL_MAX); }
            static constexpr inline channel_type to_channel(int v) { return std::clamp<int>(v, CHANNEL_MIN, CHANNEL_MAX); }
            static constexpr inline channel_type to_channel(unsigned int v) { return std::clamp<unsigned int>(v, CHANNEL_MIN, CHANNEL_MAX); }

    };

    constexpr Color Color::BLACK       { 0.0f, 0.0f, 0.0f };
    constexpr Color Color::WHITE       { 1.0f, 1.0f, 1.0f };
    constexpr Color Color::RED         { 1.0f, 0.0f, 0.0f };
    constexpr Color Color::GREEN       { 0.0f, 1.0f, 0.0f };
    constexpr Color Color::BLUE        { 0.0f, 0.0f, 1.0f };
    constexpr Color Color::YELLOW      { 0.0f, 1.0f, 1.0f };
    constexpr Color Color::CYAN        { 0.0f, 1.0f, 1.0f };
    constexpr Color Color::MAGENTA     { 1.0f, 0.0f, 1.0f };

    /**
     * @brief LED Color correction values
     * 
     * Values borrowed from the FastLED library
     */
    enum class Color::Correction : Color::raw_type {
        /// typical values for SMD5050 LEDs
        TypicalSMD5050 = Color(0xFF, 0xB0, 0xF0).raw()  /* 255, 176, 240 */,
        /// typical values for generic LED strips
        TypicalLEDStrip = Color(0xFF, 0xB0, 0xF0).raw() /* 255, 176, 240 */,

        /// typical values for 8mm "pixels on a string"
        /// also for many through-hole 'T' package LEDs
        Typical8mmPixel = Color(0xFF, 0xE0, 0x8C).raw() /* 255, 224, 140 */,
        TypicalPixelString = Color(0xFF, 0xE0, 0x8C).raw() /* 255, 224, 140 */,

        /// uncorrected color
        UncorrectedColor = Color(0xFF, 0xFF, 0xFF).raw()
    };


}
