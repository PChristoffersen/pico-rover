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

#include "quadrature_encoder.pio.h"
#include "../boardconfig.h"


class Motor {
    public: 
        using duty_value_t = float;
        using encoder_value_t = int32_t;


        Motor(uint in1_pin, uint in2_pin, uint enca_pin, uint encb_pin);
        Motor(const Motor&) = delete; // No copy constructor
        Motor(Motor&&) = delete; // No move constructor

        void init();

        void set_enabled(bool enabled);
        void put(float duty);

        void encoder_fetch_request() { quadrature_encoder_request_count(MOTOR_ENCODER_PIO, m_encoder_index); }
        encoder_value_t encoder_fetch() { return quadrature_encoder_fetch_count(MOTOR_ENCODER_PIO, m_encoder_index); }
        encoder_value_t encoder_get() { return quadrature_encoder_get_count(MOTOR_ENCODER_PIO, m_encoder_index); }

        static void set_supply_voltage(float voltage);

    private:
        uint m_in1_pin;
        uint m_in2_pin;
        uint m_enca_pin;
        uint m_encb_pin;

        uint m_slice;
        uint m_encoder_index;

        mutex_t m_mutex;

        bool m_enabled;
        float m_duty;

        void update_duty();

        static uint m_encoder_program_offset;
        static float m_supply_voltage;
        static void global_init();
};


