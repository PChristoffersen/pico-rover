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
#include <pico/mutex.h>


#include <util/ringbuffer.h>
#include <util/locking.h>
#include <util/callback.h>
#include "frsky_channels.h"
#include "frsky_telemetry.h"

namespace Radio::FrSky {

    class Receiver {
        public:
            static constexpr size_t MAX_CHANNELS { 24 };

            using control_cb_type = Callback<const Channels &>;

            Receiver(uart_inst_t *uart, uint baudrate, uint tx_pin, uint rx_pin);
            Receiver(const Receiver&) = delete; // No copy constructor
            Receiver(Receiver&&) = delete; // No move constructor

            void init();
            void begin();

            absolute_time_t update();

            void add_callback(control_cb_type::call_type callback) { m_control_callback.add(callback); }
            void set_telemetry_provider(TelemetryProvider *provider) { m_telemetry_provider = provider; }

            bool connected() const            { MUTEX_GUARD(m_mutex); return !m_channels.flags().frameLost(); }
            Channels::flag_type flags() const { MUTEX_GUARD(m_mutex); return m_channels.flags(); }
            Channels::rssi_type rssi() const  { MUTEX_GUARD(m_mutex); return m_channels.rssi(); } 
            size_t channel_count() const      { MUTEX_GUARD(m_mutex); return m_channels.count(); } 

            uint n_control_packets() const   { return m_control_packets; }
            uint n_telemetry_sent() const    { return m_telemetry_sent; }
            uint n_telemetry_skipped() const { return m_telemetry_skipped; }


            #ifndef NDEBUG
            void print_stats();
            #endif

        private:
            static constexpr uint8_t RECEIVER_ID = 0x67;
            static constexpr size_t RX_BUFFER_SIZE = 128u;
            static constexpr size_t TX_BUFFER_SIZE = 32u;
            static constexpr size_t BUFFER_MAX_WAIT_CHARS = 32u;
            static constexpr int64_t SYNC_TIMEOUT = 100000; // 100ms

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
            const uint m_baudrate;
            const uint m_tx_pin;
            const uint m_rx_pin;
            uart_inst_t *m_uart;


            State m_state;
            absolute_time_t m_last_rx_time;
            absolute_time_t m_sync_begin_time;

            // Current data
            mutable mutex_t m_mutex;

            Channels m_channels;
            control_cb_type m_control_callback;

            // Buffers
            rx_buffer_type m_rx_buffer;
            tx_buffer_type m_tx_buffer;

            // Telemetry
            TelemetryProvider *m_telemetry_provider;

            // Stats
            uint m_control_packets;
            uint m_telemetry_sent;
            uint m_telemetry_skipped;

            void lost_sync();

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
