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
#include <functional>
#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <hardware/pio.h>

#include <boardconfig.h>
#include <util/locking.h>
#include <util/callback.h>

namespace Motor {

    class Encoder {
        public:
            using value_type = int32_t;
            using id_type = uint;
            using rpm_type = double;
            using callback_type = Callback<value_type,rpm_type>;

            Encoder(id_type id, PIO pio, uint enca_pin, uint encb_pin, bool invert);
            Encoder(const Encoder&) = delete; // No copy constructor
            Encoder(Encoder&&) = delete; // No move constructor

            id_type id() const { return m_id; }

            void init();

            static absolute_time_t update();

            value_type get() const { MUTEX_GUARD(m_mutex); return m_value; }
            rpm_type rpm() const { MUTEX_GUARD(m_mutex); return m_rpm; }

            void add_callback(callback_type::call_type cb) { m_callback.add(cb); }

        private:
            using array_type = std::array<Encoder*, NUM_PIO_STATE_MACHINES>;

            static constexpr uint MAX_STEP_RATE { 100000u };
            static constexpr int64_t UPDATE_INTERVAL { 50000 };
            static constexpr rpm_type SHAFT_CPR { MOTOR_SHAFT_CPR };
            static constexpr rpm_type WHEEL_RATIO { MOTOR_GEAR_RATIO };

            const id_type m_id;
            const uint m_enca_pin;
            const uint m_encb_pin;
            const bool m_invert;
            uint m_sm;

            mutable mutex_t m_mutex;
            absolute_time_t m_value_last;
            value_type m_value;
            rpm_type m_rpm;

            callback_type m_callback;

            inline void fetch_request();
            inline void do_fetch();

            static PIO  m_pio;
            static uint m_program_offset;
            static array_type m_encoders;
            static absolute_time_t m_last_update;

            static void global_init();
    };

}