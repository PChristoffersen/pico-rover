/**
 * @author Peter Christoffersen
 * @brief Radio receiver 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <array>
#include <functional>
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/util/queue.h>
#include <pico/mutex.h>


#include "../util/ringbuffer.h"
#include "frsky_telemetry.h"

namespace Radio::FrSky {

    class Receiver {
        public:
            using flags_type = uint8_t;
            using rssi_type = uint8_t;

            static constexpr size_t MAX_CHANNELS { 24 };

            class ChannelData {
                public:
                    using value_type = float;
                    using raw_type = uint16_t;

                    ChannelData();

                    rssi_type  rssi() const { return m_rssi; }
                    flags_type flags() const { return m_flags; }

                    size_t count() const { return m_count; }
                    const value_type &operator[](size_t n) const { assert(n<m_count); return m_values[n]; }
                    const raw_type &raw(size_t n) const { assert(n<m_count); return m_raw[n]; }

                protected:
                    friend Receiver;

                    void set_count(size_t count) { m_count = count; }
                    void set_raw(size_t n, raw_type v) { m_raw[n] = v; }

                private:
                    using raw_array_type = std::array<raw_type, MAX_CHANNELS>;
                    using value_array_type = std::array<value_type, MAX_CHANNELS>;

                    rssi_type m_rssi;
                    flags_type m_flags;
                    size_t m_count;
                    raw_array_type m_raw;
                    value_array_type m_values;
            };

            using callback_type = std::function<void(const ChannelData &channels)>;


            Receiver(uart_inst_t *uart, uint baudrate, uint tx_pin, uint rx_pin);

            void init();
            void begin();

            absolute_time_t update();

            bool telemetry_push(const Telemetry &event);

            void set_callback(callback_type callback) { m_data_callback = callback; }

        private:
            static constexpr uint8_t RECEIVER_ID = 0x67;
            static constexpr size_t RX_BUFFER_SIZE = 128u;
            static constexpr size_t TX_BUFFER_SIZE = 32u;
            static constexpr size_t BUFFER_MAX_WAIT_CHARS = 32u;
            static constexpr uint TELEMETRY_QUEUE_SIZE = 32u;

            using rx_buffer_type = RingBuffer<uint8_t, RX_BUFFER_SIZE>;
            using tx_buffer_type = RingBuffer<uint8_t, TX_BUFFER_SIZE>;

            enum class State {
                SYNCING,
                READ_CONTROL,
                READ_DOWNLINK,
                WAIT_WRITE_UPLINK,
                WRITE_UPLINK,
                READ_UPLINK
            };


            static Receiver *m_instance;

            // Config
            uart_inst_t *m_uart;
            uint m_baudrate;
            uint m_tx_pin;
            uint m_rx_pin;


            State m_state;
            absolute_time_t m_last_rx_time;

            // Current data
            mutex_t m_mutex;
            bool m_connected;

            ChannelData m_data;
            callback_type m_data_callback;

            // Buffers
            rx_buffer_type m_rx_buffer;
            tx_buffer_type m_tx_buffer;

            // Telemetry
            queue_t m_telemetry_queue;

            void telemetry_flush();

            template <typename buffer_type> uint8_t checksum(const buffer_type &buffer, uint off, uint sz);
            void set_8channel(const rx_buffer_type &buffer, uint buffer_offset, uint channel_offset);

            inline void isr_handler();
            inline void irq_set_tx_enable(bool enable);
            void start_tx();

            uint64_t buffer_wait_time_us(int bytes);

            inline void begin_sync();
            inline void begin_read_control();
            inline void begin_read_downlink();
            inline void begin_wait_write_uplink();
            inline void begin_write_uplink();
            inline void begin_read_uplink();

            absolute_time_t do_sync();
            absolute_time_t do_read_control();
            absolute_time_t do_read_downlink();
            absolute_time_t do_wait_write_uplink();
            absolute_time_t do_write_uplink();
            absolute_time_t do_read_uplink();

    };

}
