/**
 * @author Peter Christoffersen
 * @brief Motor control 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <hardware/pio.h>

#include "quadrature_encoder.pio.h"

namespace Motor {

    class Encoder {
        public:
            using encoder_value_t = int32_t;

            Encoder(PIO pio, uint enca_pin, uint encb_pin);
            Encoder(const Encoder&) = delete; // No copy constructor
            Encoder(Encoder&&) = delete; // No move constructor

            void init();

            void fetch_request() { quadrature_encoder_request_count(m_pio, m_sm); }
            encoder_value_t fetch() { return quadrature_encoder_fetch_count(m_pio, m_sm); }
            encoder_value_t get() { return quadrature_encoder_get_count(m_pio, m_sm); }

        private:
            static constexpr uint MAX_STEP_RATE { 10000u };

            uint m_enca_pin;
            uint m_encb_pin;
            uint m_sm;

            static PIO  m_pio;
            static uint m_program_offset;

            static void global_init();
    };

}