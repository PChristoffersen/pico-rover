/**
 * @author Peter Christoffersen
 * @brief Motor control 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <array>
#include <pico/stdlib.h>
#include <hardware/pio.h>
#include <rtos.h>

#include "encoder.h"

#include <boardconfig.h>
#include <util/battery.h>

namespace Motor {

    class DCMotor {
        public: 
            using array_type = std::array<DCMotor, MOTOR_COUNT>;
            using id_type = uint;
            using duty_type = float;
            using rpm_type = Encoder::rpm_type;

            enum {
                FRONT_LEFT  = 0,
                FRONT_RIGHT = 1,
                REAR_LEFT   = 2,
                REAR_RIGHT  = 3,
            };

            DCMotor(id_type id, uint in1_pin, uint in2_pin, PIO enc_pio, uint enca_pin, uint encb_pin, bool invert);
            DCMotor(const DCMotor&) = delete; // No copy constructor
            DCMotor(DCMotor&&) = delete; // No move constructor

            id_type id() const { return m_id; }

            void init();

            duty_type duty() const { return m_duty; }
            rpm_type rpm() const { return m_encoder.rpm(); }

            void set_enabled(bool enabled);
            void set_duty(duty_type duty);

            static void set_supply_voltage(float voltage);
            static void set_global_enable(bool enabled);

            const Encoder &encoder() const { return m_encoder; }
            Encoder &encoder() { return m_encoder; }

        private:
            // Configuration of the motor driver (See https://www.pololu.com/product/4036) 
            enum class DriverMode {
                IN_IN,
                PH_EN,
            };
            static constexpr DriverMode DRIVER_MODE { DriverMode::PH_EN };

            static constexpr uint32_t PWM_MAX { 1000u };
            static constexpr uint32_t PWM_WRAP { 1250u };
            static constexpr float PWM_FREQUENCY  { MOTOR_PWM_FREQUENCY };
            static constexpr float VOLTAGE_TARGET { MOTOR_TARGET_VOLTAGE };

            const id_type m_id;
            const uint m_in1_pin;
            const uint m_in2_pin;
            const bool m_invert;

            uint m_slice;

            StaticSemaphore_t m_mutex_buf;
            SemaphoreHandle_t m_mutex;

            bool m_enabled;
            float m_duty;

            Encoder m_encoder;

            void update_duty();

            static uint m_enable_count;
            static bool m_global_enabled;
            static float m_supply_voltage;
            static void global_init();
            static void global_enable(bool enable);
    };

}
