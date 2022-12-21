/**
 * @author Peter Christoffersen
 * @brief WS281x LED Strip control 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#if !PICO_NO_HARDWARE

#include <array>
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/pio.h>
#include <hardware/dma.h>
#include <FreeRTOS.h>
#include <semphr.h>

#include <led/color.h>
#include <led/colorbuffer.h>

namespace LED {

    class StripBase {
        public: 
            virtual void show() = 0;

            virtual void fill(Color::RGB color) = 0;
            virtual size_t length() const = 0;

            virtual Color::RGB &operator[](size_t n) = 0;
            virtual const Color::RGB &operator[](size_t n) const = 0;

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

            alarm_id_t m_reset_alarm;
            StaticSemaphore_t m_reset_sem_buf;
            SemaphoreHandle_t m_reset_sem;

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

            inline void dma_wait()
            {
                // Wait for previous grace period to complete
                //printf("LED WaitSem\n");
                xSemaphoreTake(m_reset_sem, portMAX_DELAY);
            }

            inline void dma_start(const volatile void *addr)
            {
                // Start transfer    
                //printf("LED Start\n");
                dma_channel_set_read_addr(m_dma, addr, true);
            }
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
                show_single(Color::RGB::BLACK);
            }

            template<typename COLOR_TYPE>
            void show_single(const COLOR_TYPE color) 
            {
                dma_wait();

                // Call sub-class function top copy pixel data into dma buffer
                //printf("LED COPY\n");
                for (uint idx=0; idx<NCOLORS; idx++) {
                    auto c = color;
                    c *= (m_brightness*m_brightness); // Use brightness ^2 to make the perceived brightness seem more linear
                    c *= m_correction;
                    m_dma_buffer[idx++] = c.rgb()<<8 | c.white();
                }

                dma_start(m_dma_buffer);
            }

            template<typename COLOR_BUFFER>
            void show(const COLOR_BUFFER &color_buffer)
            {
                assert(color_buffer.size()<=NCOLORS);

                dma_wait();

                // Call sub-class function top copy pixel data into dma buffer
                //printf("LED COPY\n");
                uint idx = 0;
                for (const auto &color : color_buffer) {
                    auto c = color;
                    c *= (m_brightness*m_brightness); // Use brightness ^2 to make the perceived brightness seem more linear
                    c *= m_correction;
                    m_dma_buffer[idx++] = c.rgb()<<8 | c.white();
                }

                dma_start(m_dma_buffer);
            }

            void show() override 
            {
                dma_wait();

                // Call sub-class function top copy pixel data into dma buffer
                //printf("LED COPY\n");
                uint idx = 0;
                for (const auto &color : m_color_buffer) {
                    auto c = color;
                    c *= (m_brightness*m_brightness); // Use brightness ^2 to make the perceived brightness seem more linear
                    c *= m_correction;
                    m_dma_buffer[idx++] = c.rgb()<<8 | c.white();
                }

                dma_start(m_dma_buffer);
            }

            virtual void fill(Color::RGB color) override { m_color_buffer.fill(color); }
            virtual size_t length() const override { return NCOLORS; }

            virtual Color::RGB &operator[](size_t n) override { return m_color_buffer[n]; }
            virtual const Color::RGB &operator[](size_t n) const override { return m_color_buffer[n]; }


        private:
            using color_array_type = std::array<Color::RGB, NCOLORS>;

            color_array_type m_color_buffer;
            volatile pixel_type m_dma_buffer[NCOLORS];

    };

}

#endif
