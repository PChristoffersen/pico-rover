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

namespace LED {

    class StripBase {
        public: 
            void show();

            virtual void fill(Color color) = 0;
            virtual size_t length() const = 0;

            virtual Color &operator[](size_t n) = 0;
            virtual const Color &operator[](size_t n) const = 0;

        protected:
            using pixel_type = uint32_t;

            static constexpr float STRIP_FREQUENCY = 800000.0f;
            static constexpr uint64_t STRIP_RESET_DELAY_US = 400u;


            const uint m_pin;
            const bool m_is_rgbw;

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
            static StripBase *m_strips[NUM_DMA_CHANNELS];

            StripBase(PIO pio, uint pin, bool is_rgbw);

            void base_init(volatile void *dma_addr, size_t dma_count);

            static void global_init(); 
            static void global_add_strip(StripBase *strip);     
            static void __isr dma_complete_handler();

            virtual void copy_buffer() = 0;
    };


    template<size_t NCOLORS>
    class Strip : public StripBase {
        public:
            Strip(PIO pio, uint pin, bool is_rgbw) : StripBase { pio, pin, is_rgbw } { }
            Strip(const Strip&) = delete; // No copy constructor
            Strip(Strip&&) = delete; // No move constructor

            void init()
            {
                base_init(m_dma_buffer, NCOLORS);

                // Show initial
                fill(Color::BLACK);
                show();
            }

            virtual void fill(Color color) override { m_color_buffer.fill(color); }
            virtual size_t length() const override { return NCOLORS; }

            virtual Color &operator[](size_t n) override { return m_color_buffer[n]; }
            virtual const Color &operator[](size_t n) const override { return m_color_buffer[n]; }


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

}
