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

            Receiver(uart_inst_t *uart, uint tx_pin, uint rx_pin, uint baudrate);

            void init();
            void begin();

            absolute_time_t update();

            bool telemetry_push(const radio_telemetry_t &event);

        private:
            static constexpr uint8_t RECEIVER_ID = 0x67;
            static constexpr size_t RX_BUFFER_SIZE = 128u;
            static constexpr size_t TX_BUFFER_SIZE = 16u;
            static constexpr size_t BUFFER_MAX_WAIT_CHARS = 8u;
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


            // Config
            uart_inst_t *m_uart;
            uint m_baudrate;
            uint m_tx_pin;
            uint m_rx_pin;


            State m_state;
            absolute_time_t m_uplink_start;

            // Current data
            mutex_t m_mutex;
            bool m_connected;
            flags_type m_flags;
            rssi_type m_rssi;


            // Buffers
            rx_buffer_type m_rx_buffer;
            tx_buffer_type m_tx_buffer;

            // Telemetry
            queue_t m_telemetry_queue;

            void telemetry_flush();

            template <typename buffer_type>
            uint8_t checksum(const buffer_type &buffer, uint off, uint sz)
            {
                uint16_t sum = 0x00;
                for (uint i=0; i<sz; i++) {
                    sum += buffer[off+i];
                }
                while (sum > 0xFF) {
                    sum = (sum & 0xFF) + (sum >>8);
                }
                return 0xFF - sum;
            }


            inline void buffers_update();
            uint64_t buffer_wait_time_us(size_t bytes);

            inline void begin_sync();
            inline void begin_read_control();
            inline void begin_read_downlink();
            inline void begin_wait_write_uplink();
            inline void begin_write_uplink();
            inline void begin_read_uplink();

            bool do_sync(absolute_time_t &wait);
            bool do_read_control(absolute_time_t &wait);
            bool do_read_downlink(absolute_time_t &wait);
            bool do_wait_write_uplink(absolute_time_t &wait);
            bool do_write_uplink(absolute_time_t &wait);
            bool do_read_uplink(absolute_time_t &wait);

    };

}
