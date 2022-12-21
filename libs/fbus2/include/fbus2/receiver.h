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
#include <atomic>
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <stream_buffer.h>

#include "channels.h"
#include "telemetry.h"
#include "mapping.h"
#include "ringbuffer.h"

namespace FBus2 {

    class Receiver {
        public:
            static constexpr uint8_t RECEIVER_ID  { 0x67 };
            static constexpr size_t MAX_CHANNELS { 24 };
            using channels_type = Channels;

            Receiver(uint baudrate, UBaseType_t task_priority, UBaseType_t lower_task_priority);
            Receiver(const Receiver&) = delete; // No copy constructor
            Receiver(Receiver&&) = delete; // No move constructor

            void init();
            void start();

            void lock() const { xSemaphoreTake(m_mutex, portMAX_DELAY); }
            void unlock() const { xSemaphoreGive(m_mutex); }

            bool connected() const            { return !m_channels.flags().frameLost(); }
            channels_type            channels() const { return m_channels; }
            channels_type::flag_type flags() const { return m_channels.flags(); }
            channels_type::rssi_type rssi() const  { return m_channels.rssi(); } 
            bool                     sync() const  { return m_channels.sync(); } 
            size_t channel_count() const      { return m_channels.count(); } 

            uint n_control_packets() const   { return m_control_packets; }
            uint n_telemetry_sent() const    { return m_telemetry_sent; }
            uint n_telemetry_skipped() const { return m_telemetry_skipped; }


            #ifndef NDEBUG
            void print_stats();
            #endif

        protected:
            static constexpr uint TASK_STACK_SIZE { configMINIMAL_STACK_SIZE };
            static constexpr uint TASK_LOWER_STACK_SIZE { 2*configMINIMAL_STACK_SIZE };
            static constexpr uint ISR_CORE { 1u };

            static constexpr size_t RX_BUFFER_SIZE { 128u };
            static constexpr size_t TX_BUFFER_SIZE { 32u };
            static constexpr size_t RX_SCRATCH_SIZE { 64u };

            static constexpr size_t BUFFER_MAX_WAIT_CHARS = 32u;
            static constexpr int64_t SYNC_TIMEOUT = 100000; // 100ms

            using rx_buffer_type = RingBuffer<uint8_t, RX_BUFFER_SIZE>;
            using tx_buffer_type = RingBuffer<uint8_t, TX_BUFFER_SIZE>;

            enum class State {
                SYNCING,
                READ_CONTROL,
                READ_DOWNLINK,
                WRITE_UPLINK,
                READ_UPLINK
            };

            virtual void on_data(const channels_type &channels) = 0;
            
            virtual void hardware_init() = 0;
            virtual void task_init() = 0;
            virtual Telemetry get_next_telemetry() 
            {
                return Telemetry::null();
            }


            static Receiver *m_instance;

            // Config
            const uint m_baudrate;
            const UBaseType_t m_task_priority;
            const UBaseType_t m_lower_task_priority;


            StaticTask_t m_task_buf;
            StackType_t m_task_stack[TASK_STACK_SIZE];
            TaskHandle_t m_task;

            StaticTask_t m_task_lower_buf;
            StackType_t m_task_lower_stack[TASK_LOWER_STACK_SIZE];
            TaskHandle_t m_task_lower;

            // Buffers
            uint8_t m_rx_scratch[RX_SCRATCH_SIZE];
            uint8_t m_rx_scratch_size;

            uint8_t m_rx_buffer_data[RX_BUFFER_SIZE];
            StaticStreamBuffer_t m_rx_buffer_buf;
            StreamBufferHandle_t m_rx_buffer;
            
            uint8_t m_tx_buffer_data[TX_BUFFER_SIZE];
            StaticStreamBuffer_t m_tx_buffer_buf;
            StreamBufferHandle_t m_tx_buffer;


            State m_state;
            absolute_time_t m_last_rx_time;
            absolute_time_t m_sync_begin_time;

            // Current data
            StaticSemaphore_t m_mutex_buf;
            mutable SemaphoreHandle_t m_mutex;
            channels_type m_channels;

            // Stats
            uint m_control_packets;
            uint m_telemetry_sent;
            uint m_telemetry_skipped;

            void lost_sync();

            void rx_scratch_recv(size_t bytes);
            void rx_scratch_pop(size_t bytes);
            virtual void tx_send(const uint8_t *buf, size_t sz) = 0;

            inline void begin_sync();
            inline void begin_read_control();
            inline void begin_read_downlink();
            inline void begin_write_uplink();
            inline void begin_read_uplink();

            void do_sync();
            void do_read_control();
            void do_read_downlink();
            void do_write_uplink();
            void do_read_uplink();

            void run();
            void run_lower();

    };

}
