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

namespace LED::Color {

    class ColorBase {
        public:
            using raw_type = uint32_t;
            using channel_type = uint8_t;
            using brightness_type = float;

            static constexpr brightness_type BRIGHTNESS_MAX { 1.0f };
            static constexpr brightness_type BRIGHTNESS_MIN { 0.0f };
            static constexpr brightness_type BRIGHTNESS_DEFAULT { BRIGHTNESS_MAX };

            static constexpr channel_type CHANNEL_MAX { std::numeric_limits<channel_type>::max() };
            static constexpr channel_type CHANNEL_MIN { std::numeric_limits<channel_type>::min() };

            static constexpr auto     BLUE_SHIFT  { 0 };
            static constexpr raw_type BLUE_MASK   { 0x000000FF };
            static constexpr auto     RED_SHIFT   { 8 };
            static constexpr raw_type RED_MASK    { 0x0000FF00 };
            static constexpr auto     GREEN_SHIFT { 16 };
            static constexpr raw_type GREEN_MASK  { 0x00FF0000 };
            static constexpr auto     X_SHIFT { 24 };
            static constexpr raw_type X_MASK  { 0xFF000000 };
            static constexpr raw_type CHANNEL_MASK { 0xFF };


            enum class Correction : ColorBase::raw_type;

        protected:
            static constexpr inline raw_type raw_rgb(raw_type r, raw_type g, raw_type b)
            {
                return r<<RED_SHIFT | g<<GREEN_SHIFT | b<<BLUE_SHIFT;
            }
            static constexpr inline raw_type raw_rgbx(raw_type r, raw_type g, raw_type b, raw_type x)
            {
                return x<<X_SHIFT | r<<RED_SHIFT | g<<GREEN_SHIFT | b<<BLUE_SHIFT;
            }

            static constexpr inline channel_type to_channel(float v) { return std::clamp<float>(v*static_cast<float>(CHANNEL_MAX)+0.5f, CHANNEL_MIN, CHANNEL_MAX); }
            static constexpr inline channel_type to_channel(int v) { return std::clamp<int>(v, CHANNEL_MIN, CHANNEL_MAX); }
            static constexpr inline channel_type to_channel(unsigned int v) { return std::clamp<unsigned int>(v, CHANNEL_MIN, CHANNEL_MAX); }

    };


    using Correction = ColorBase::Correction;

    class HSV : public ColorBase {
        public:
            using channel_type = uint8_t;

            constexpr HSV() : m_hue { 0x00} , m_sat { 0x00 }, m_val { 0x00 } {}
            constexpr HSV(HSV &other) : m_hue { other.m_hue } , m_sat { other.m_sat }, m_val { other.m_val } {}
            constexpr HSV(HSV &&other) : m_hue { other.m_hue } , m_sat { other.m_sat }, m_val { other.m_val } {}
            constexpr HSV(channel_type h, channel_type s, channel_type v) : m_hue { h }, m_sat { s }, m_val { v } {}

            /**
             * @brief Convert a hue, saturation, and value to RGB
             * 
             * using a visually balanced rainbow (vs a straight
             * mathematical spectrum).
             * This 'rainbow' yields better yellow and orange
             * than a straight 'spectrum'.
             * 
             * @note Here hue is 0-255, not just 0-191
             * 
             * @return RGBW Converted RGB RGB
             */
            raw_type rainbow() const;

            /**
             * @brief Convert a hue, saturation, and value to RGB
             * 
             * Using a mathematically straight spectrum (vs
             * a visually balanced rainbow).
             * This 'spectrum' will have more green & blue
             * than a 'rainbow', and less yellow and orange.
             * 
             * @note Here hue is 0-255, not just 0-191
             * 
             * @return RGBW Converted RGB RGBW
             */
            raw_type spectrum() const;

            /**
             * @brief Convert hue, saturation, and value to RGB.
             * 
             * This 'spectrum' conversion will be more green & blue
             * than a real 'rainbow', and the hue is specified just
             * in the range 0-191.  Together, these result in a
             * slightly faster conversion speed, at the expense of
             * color balance.
             * 
             * @note Hue is 0-191 only! Saturation & value are 0-255 each.
             * 
             * @return RGBW Converted RGB RGBW
             */
            raw_type raw() const;

            inline constexpr channel_type hue() const { return m_hue; }
            inline constexpr channel_type sat() const { return m_sat; }
            inline constexpr channel_type val() const { return m_val; }

            operator raw_type() const { return rainbow(); }

        private:
            channel_type m_hue;
            channel_type m_sat;
            channel_type m_val;
    };

    class RGB : public ColorBase {
        public:

            static const RGB BLACK;
            static const RGB WHITE;
            static const RGB RED;
            static const RGB GREEN;
            static const RGB BLUE;
            static const RGB YELLOW;
            static const RGB CYAN;
            static const RGB MAGENTA;

            constexpr RGB() : RGB(BLACK) {}
            constexpr RGB(const RGB &color) : m_data{ color.m_data } {}
            constexpr RGB(const RGB &&color) : m_data{ color.m_data } {}
            constexpr RGB(int r, int g, int b) : m_data { raw_rgb(to_channel(r), to_channel(g), to_channel(b)) } {}
            constexpr RGB(unsigned int r, unsigned int g, unsigned int b) : m_data { raw_rgb(to_channel(r), to_channel(g), to_channel(b)) } {}
            constexpr RGB(float r, float g, float b) : m_data { raw_rgb(to_channel(r), to_channel(g), to_channel(b)) } {}
            constexpr explicit RGB(Correction correction) : m_data { static_cast<raw_type>(correction) } {}
            constexpr explicit RGB(raw_type raw) : m_data { raw } {}

            constexpr inline channel_type red()   const { return (m_data >> RED_SHIFT) & CHANNEL_MASK; }
            constexpr inline channel_type green() const { return (m_data >> GREEN_SHIFT) & CHANNEL_MASK; }
            constexpr inline channel_type blue()  const { return (m_data >> BLUE_SHIFT) & CHANNEL_MASK; }
            constexpr inline channel_type white() const { return CHANNEL_MIN; }
            constexpr inline channel_type alpha() const { return CHANNEL_MAX; }

            static constexpr inline channel_type rawRed(raw_type c) { return (c >> RED_SHIFT) & CHANNEL_MASK; }
            static constexpr inline channel_type rawGreen(raw_type c) { return (c >> GREEN_SHIFT) & CHANNEL_MASK; }
            static constexpr inline channel_type rawBlue(raw_type c) { return (c >> BLUE_SHIFT) & CHANNEL_MASK; }

            constexpr inline raw_type     rgb()   const { return (m_data&(GREEN_MASK|RED_MASK|BLUE_MASK)); }
            constexpr inline raw_type     raw()   const { return m_data; }

            RGB &operator=(const RGB &other) { m_data = other.m_data; return *this; }
            RGB &operator=(const raw_type other) { m_data = other; return *this; }

            RGB &operator*=(const Correction correction) { *this = *this * correction; return *this; }
            RGB &operator*=(const brightness_type brightness) { *this = *this * brightness; return *this; }

            constexpr RGB operator*(const brightness_type brightness) const
            {
                raw_type scale = std::clamp<raw_type>(brightness*static_cast<brightness_type>(CHANNEL_MAX)+0.5f, CHANNEL_MIN, CHANNEL_MAX);
                if (scale == CHANNEL_MAX) {
                    return *this;
                }
                if (scale == CHANNEL_MIN) {
                    return RGB { m_data & X_MASK };
                }
                raw_type red   = ( (m_data & RED_MASK)   * scale / CHANNEL_MAX ) & RED_MASK;
                raw_type green = ( (m_data & GREEN_MASK) * scale / CHANNEL_MAX ) & GREEN_MASK;
                raw_type blue  = ( (m_data & BLUE_MASK)  * scale / CHANNEL_MAX ) & BLUE_MASK;
                return RGB { red | green | blue };
            }

            constexpr RGB operator*(const Correction correction) const 
            {
                raw_type red   = ( (m_data & RED_MASK)   * rawRed(static_cast<raw_type>(correction))   / CHANNEL_MAX ) & RED_MASK;
                raw_type green = ( (m_data & GREEN_MASK) * rawGreen(static_cast<raw_type>(correction)) / CHANNEL_MAX ) & GREEN_MASK;
                raw_type blue  = ( (m_data & BLUE_MASK)  * rawBlue(static_cast<raw_type>(correction))  / CHANNEL_MAX ) & BLUE_MASK;
                return RGB { red | green | blue };
            }

            constexpr void set(raw_type data) { m_data = data; }

        protected:
            raw_type m_data;

            constexpr explicit RGB(channel_type r, channel_type g, channel_type b, channel_type x) : m_data { raw_rgbx(to_channel(r), to_channel(g), to_channel(b), to_channel(x)) } {}
            constexpr explicit RGB(channel_type x) : RGB() { setX(x); }

            constexpr inline void setX(channel_type x) {  m_data |= (x<<X_SHIFT); }
            constexpr inline channel_type x() const { return (m_data >> X_SHIFT) & CHANNEL_MASK; }
            static constexpr inline channel_type rawX(raw_type c) { return (c >> X_SHIFT) & CHANNEL_MASK; }
    };


    class RGBA : public RGB {
        public:
            static constexpr auto     ALPHA_SHIFT { X_SHIFT };
            static constexpr raw_type ALPHA_MASK  { X_MASK };

            static const RGBA TRANSPARENT;

            constexpr RGBA() : RGBA(TRANSPARENT) { }
            constexpr RGBA(const RGBA &color) : RGB(color.m_data) {}
            constexpr RGBA(const RGBA &&color) : RGB(color.m_data) {}
            constexpr RGBA(const RGB &color) : RGB(color) { setX(CHANNEL_MAX); }
            constexpr RGBA(int r, int g, int b) : RGB(to_channel(r), to_channel(g), to_channel(b), CHANNEL_MAX) {}
            constexpr RGBA(int r, int g, int b, int a) : RGB(to_channel(r), to_channel(g), to_channel(b), to_channel(a)) {}
            constexpr RGBA(unsigned int r, unsigned int g, unsigned int b) : RGB(to_channel(r), to_channel(g), to_channel(b), CHANNEL_MAX) {}
            constexpr RGBA(unsigned int r, unsigned int g, unsigned int b, unsigned int a) : RGB(to_channel(r), to_channel(g), to_channel(b), to_channel(a)) {}
            constexpr RGBA(float r, float g, float b) : RGB(to_channel(r), to_channel(g), to_channel(b), CHANNEL_MAX) {}
            constexpr RGBA(float r, float g, float b, float a) : RGB(to_channel(r), to_channel(g), to_channel(b), to_channel(a)) {}
            constexpr explicit RGBA(Correction correction) : RGB(static_cast<raw_type>(correction)) { setX(CHANNEL_MAX); }
            constexpr explicit RGBA(raw_type raw) : RGB(raw) {}

            constexpr inline channel_type alpha() const { return x(); }

            static constexpr inline channel_type rawAlpha(raw_type c) { return (c >> ALPHA_SHIFT) & CHANNEL_MASK; }

            RGBA &operator=(const RGBA &other) { m_data = other.m_data; return *this; }
            RGBA &operator=(const raw_type other) { m_data = other; return *this; }

            constexpr RGBA operator*(const brightness_type brightness) const
            {
                raw_type scale = std::clamp<raw_type>(brightness*static_cast<brightness_type>(CHANNEL_MAX)+0.5f, CHANNEL_MIN, CHANNEL_MAX);
                if (scale == CHANNEL_MAX) {
                    return *this;
                }
                if (scale == CHANNEL_MIN) {
                    return RGBA { m_data & ALPHA_MASK };
                }
                raw_type red   = ( (m_data & RED_MASK)   * scale / CHANNEL_MAX ) & RED_MASK;
                raw_type green = ( (m_data & GREEN_MASK) * scale / CHANNEL_MAX ) & GREEN_MASK;
                raw_type blue  = ( (m_data & BLUE_MASK)  * scale / CHANNEL_MAX ) & BLUE_MASK;
                raw_type alpha = ( ((m_data & ALPHA_MASK)>>8) * scale / CHANNEL_MASK )<<8;
                return RGBA { red | green | blue | alpha};
            }

            constexpr RGBA operator*(const Correction correction) const 
            {
                raw_type red   = ( (m_data & RED_MASK)   * rawRed(static_cast<raw_type>(correction))   / CHANNEL_MAX ) & RED_MASK;
                raw_type green = ( (m_data & GREEN_MASK) * rawGreen(static_cast<raw_type>(correction)) / CHANNEL_MAX ) & GREEN_MASK;
                raw_type blue  = ( (m_data & BLUE_MASK)  * rawBlue(static_cast<raw_type>(correction))  / CHANNEL_MAX ) & BLUE_MASK;
                raw_type white = m_data & ALPHA_MASK;
                return RGBA { red | green | blue | white };
            }

            RGBA &operator*=(const Correction correction) { *this = *this * correction; return *this; }
            RGBA &operator*=(const brightness_type brightness) { *this = *this * brightness; return *this; }


        protected:

    };

    class RGBW : public RGB {
        public:
            static constexpr auto     WHITE_SHIFT { X_SHIFT };
            static constexpr raw_type WHITE_MASK  { X_MASK };

            static const RGBW WHITE;

            constexpr RGBW() : RGB(CHANNEL_MIN) { }
            constexpr RGBW(const RGBW &color) : RGB(color.m_data) {}
            constexpr RGBW(const RGBW &&color) : RGB(color.m_data) {}
            constexpr RGBW(const RGB &color) : RGB(color) { setX(CHANNEL_MIN); }
            constexpr RGBW(int r, int g, int b) : RGB(to_channel(r), to_channel(g), to_channel(b), CHANNEL_MIN) {}
            constexpr RGBW(int r, int g, int b, int a) : RGB(to_channel(r), to_channel(g), to_channel(b), to_channel(a)) {}
            constexpr RGBW(unsigned int r, unsigned int g, unsigned int b) : RGB(to_channel(r), to_channel(g), to_channel(b), CHANNEL_MIN) {}
            constexpr RGBW(unsigned int r, unsigned int g, unsigned int b, unsigned int w) : RGB(to_channel(r), to_channel(g), to_channel(b), to_channel(w)) {}
            constexpr RGBW(float r, float g, float b) : RGB(to_channel(r), to_channel(g), to_channel(b), CHANNEL_MIN) {}
            constexpr RGBW(float r, float g, float b, float w) : RGB(to_channel(r), to_channel(g), to_channel(b), to_channel(w)) {}
            constexpr explicit RGBW(Correction correction) : RGB(static_cast<raw_type>(correction)) { setX(CHANNEL_MIN); }
            constexpr explicit RGBW(raw_type raw) : RGB(raw) {}

            constexpr inline channel_type white() const { return (m_data >> WHITE_SHIFT) & CHANNEL_MASK; }

            static constexpr inline channel_type rawWhite(raw_type c) { return (c >> WHITE_SHIFT) & CHANNEL_MASK; }

            RGBW &operator=(const RGB &other) { m_data = other.rgb(); return *this; }
            RGBW &operator=(const RGBA &other) { m_data = other.rgb(); return *this; }
            RGBW &operator=(const RGBW &other) { m_data = other.m_data; return *this; }
            RGBW &operator=(const raw_type other) { m_data = other; return *this; }

            constexpr RGBW operator*(const brightness_type brightness) const
            {
                raw_type scale = std::clamp<raw_type>(brightness*static_cast<brightness_type>(CHANNEL_MAX)+0.5f, CHANNEL_MIN, CHANNEL_MAX);
                if (scale == CHANNEL_MAX) {
                    return *this;
                }
                raw_type red   = ( (m_data & RED_MASK)   * scale / CHANNEL_MAX ) & RED_MASK;
                raw_type green = ( (m_data & GREEN_MASK) * scale / CHANNEL_MAX ) & GREEN_MASK;
                raw_type blue  = ( (m_data & BLUE_MASK)  * scale / CHANNEL_MAX ) & BLUE_MASK;
                raw_type white = ( ((m_data & WHITE_MASK)>>8) * scale / CHANNEL_MASK )<<8;
                return RGBW { red | green | blue | white};
            }

            constexpr RGBW operator*(const Correction correction) const 
            {
                raw_type red   = ( (m_data & RED_MASK)   * rawRed(static_cast<raw_type>(correction))   / CHANNEL_MAX ) & RED_MASK;
                raw_type green = ( (m_data & GREEN_MASK) * rawGreen(static_cast<raw_type>(correction)) / CHANNEL_MAX ) & GREEN_MASK;
                raw_type blue  = ( (m_data & BLUE_MASK)  * rawBlue(static_cast<raw_type>(correction))  / CHANNEL_MAX ) & BLUE_MASK;
                raw_type white = m_data & WHITE_MASK;
                return RGBW { red | green | blue | white };
            }

            RGBW &operator*=(const Correction correction) { *this = *this * correction; return *this; }
            RGBW &operator*=(const brightness_type brightness) { *this = *this * brightness; return *this; }


            RGBW &operator<<(const HSV &src) {
                m_data = src.rainbow();
                return *this;
            }
            RGBW &operator<<(const RGB &src) {
                m_data = src.rgb();
                return *this;
            }
            RGBW &operator<<(const RGBW &src) {
                m_data = src.m_data;
                return *this;
            }

            /**
             * @brief Blend this color with color
             * 
             * res.r = dst.r * (1 - src.a) + src.r * src.a
             * res.g = dst.g * (1 - src.a) + src.g * src.a
             * res.b = dst.b * (1 - src.a) + src.b * src.a
             * res.a = dst.a * (1 - src.a) + src.a
             * 
             * @param color The color to blend with
             * @return Color& this
             */
            RGBW &operator<<(const RGBA &color);


        private: 

    };




    constexpr RGB RGB::BLACK       { 0.0f, 0.0f, 0.0f };
    constexpr RGB RGB::WHITE       { 1.0f, 1.0f, 1.0f };
    constexpr RGB RGB::RED         { 1.0f, 0.0f, 0.0f };
    constexpr RGB RGB::GREEN       { 0.0f, 1.0f, 0.0f };
    constexpr RGB RGB::BLUE        { 0.0f, 0.0f, 1.0f };
    constexpr RGB RGB::YELLOW      { 1.0f, 1.0f, 0.0f };
    constexpr RGB RGB::CYAN        { 0.0f, 1.0f, 1.0f };
    constexpr RGB RGB::MAGENTA     { 1.0f, 0.0f, 1.0f };
    constexpr RGBA RGBA::TRANSPARENT { 0.0f, 0.0f, 0.0f, 0.0f };
    constexpr RGBW RGBW::WHITE       { 1.0f, 1.0f, 1.0f, 1.0f };


    /**
     * @brief LED RGBW correction values
     * 
     * Values borrowed from the FastLED library
     */
    enum class ColorBase::Correction : ColorBase::raw_type {
        /// typical values for SMD5050 LEDs
        TypicalSMD5050 = RGB(0xFF, 0xB0, 0xF0).raw()  /* 255, 176, 240 */,
        /// typical values for generic LED strips
        TypicalLEDStrip = RGB(0xFF, 0xB0, 0xF0).raw() /* 255, 176, 240 */,

        /// typical values for 8mm "pixels on a string"
        /// also for many through-hole 'T' package LEDs
        Typical8mmPixel = RGB(0xFF, 0xE0, 0x8C).raw() /* 255, 224, 140 */,
        TypicalPixelString = RGB(0xFF, 0xE0, 0x8C).raw() /* 255, 224, 140 */,

        /// uncorrected color
        UncorrectedRGBW = RGB(0xFF, 0xFF, 0xFF).raw()
    };


}
