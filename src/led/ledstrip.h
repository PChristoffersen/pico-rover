/**
 * @author Peter Christoffersen
 * @brief WS281x LED Strip control 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <array>
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/sem.h>
#include <hardware/pio.h>
#include <hardware/dma.h>

#include "color.h"


class LEDStripBase {
    public: 
        void show();

    protected:
        using pixel_type = uint32_t;

        static constexpr uint MAX_STRIPS = 2;
        static constexpr float STRIP_FREQUENCY = 800000.0f;
        static constexpr uint64_t STRIP_RESET_DELAY_US = 400u;


        uint m_pin;
        bool m_is_rgbw;

        Color::Correction m_correction;
        float m_brightness;

        uint m_sm;
        uint m_dma;
        volatile void *m_dma_addr;

        alarm_id_t m_reset_alarm;
        semaphore_t m_reset_sem;

        // Global
        static uint m_dma_irq_index;
        static PIO m_pio;
        static uint m_program_offset;
        static LEDStripBase *m_strips[MAX_STRIPS];

        LEDStripBase(PIO pio, uint pin, bool is_rgbw);

        void base_init(volatile void *dma_addr, size_t dma_count);

        static void global_init(); 
        static void global_add_strip(LEDStripBase *strip);     
        static void __isr dma_complete_handler();

        virtual void copy_buffer() = 0;
};


template<size_t NCOLORS>
class LEDStrip : public LEDStripBase {
    public:
        LEDStrip(PIO pio, uint pin, bool is_rgbw) : LEDStripBase { pio, pin, is_rgbw } { }
        LEDStrip(const LEDStrip&) = delete; // No copy constructor
        LEDStrip(LEDStrip&&) = delete; // No move constructor

        void init()
        {
            base_init(m_dma_buffer, NCOLORS);

            // Show initial
            fill(Color::BLACK);
            show();
        }

        void fill(Color color)
        {
            m_color_buffer.fill(color);
        }

        Color &operator[](size_t n) noexcept { return m_color_buffer[n]; }
        constexpr const Color &operator[](size_t n) const noexcept { return m_color_buffer[n]; }


    private:
        using color_array_type = std::array<Color, NCOLORS>;

        color_array_type m_color_buffer;
        volatile pixel_type m_dma_buffer[NCOLORS];

        void copy_buffer() 
        {
            // Copy color data to dma buffer
            uint idx = 0;
            for (const Color &color : m_color_buffer) {
                auto c = color;
                c *= (m_brightness*m_brightness); // Use brightness ^2 to make the perceived brightness seem more linear
                c *= m_correction;
                m_dma_buffer[idx++] = c.rgb()<<8 | c.white();
            }

        }
};

