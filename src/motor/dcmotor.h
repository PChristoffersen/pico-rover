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

#include "encoder.h"


namespace Motor {

    class DCMotor {
        public: 
            using duty_value_t = float;
            using encoder_value_t = int32_t;


            DCMotor(uint in1_pin, uint in2_pin, PIO enc_pio, uint enca_pin, uint encb_pin);
            DCMotor(const DCMotor&) = delete; // No copy constructor
            DCMotor(DCMotor&&) = delete; // No move constructor

            void init();

            void set_enabled(bool enabled);
            void put(float duty);

            static void set_supply_voltage(float voltage);

            const Encoder &encoder() const { return m_encoder; }
            Encoder &encoder() { return m_encoder; }

        private:
            // Configuration of the motor driver (See https://www.pololu.com/product/4036) 
            enum class DriverMode {
                IN_IN,
                PH_EN,
            };
            static constexpr DriverMode DRIVER_MODE { DriverMode::IN_IN };

            static constexpr uint32_t PWM_MAX { 1000u };
            static constexpr uint32_t PWM_WRAP { 1250u };
            //static constexpr float PWM_FREQUENCY { 12500.0f }; // 1.25 kHz
            static constexpr float PWM_FREQUENCY { 25000.0f }; // 25 kHz


            uint m_in1_pin;
            uint m_in2_pin;

            uint m_slice;

            mutex_t m_mutex;

            bool m_enabled;
            float m_duty;

            Encoder m_encoder;

            void update_duty();

            static float m_supply_voltage;
            static void global_init();
    };

}
