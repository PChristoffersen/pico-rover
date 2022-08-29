#pragma once

#include <pico/stdlib.h>
#include <pico/util/queue.h>
#include <rtos.h>

#include <util/callback.h>

#include "frsky_receiver.h"
#include "frsky_channels.h"
#include "frsky_mapping.h"

namespace Radio::FrSky {

    /** 
     * Listener class for moving Receiver channels events from core1 to core0
     * */
    class ReceiverListener {
        public:
            using mapping_type = TaranisX9DPlus;        
            using channels_type = Channels;
            using callback_type = Callback<const Channels &, const mapping_type &>;

            ReceiverListener();
            ReceiverListener(const ReceiverListener&) = delete; // No copy constructor
            ReceiverListener(ReceiverListener&&) = delete; // No move constructor

            void init(Receiver &receiver);

            const channels_type &channels() const { return m_channels; }
            const mapping_type &mapping() const { return m_mapping; }

            void add_callback(callback_type::call_type cb) { m_callback.add(cb); }

        private:
            static constexpr uint TASK_STACK_SIZE { configMINIMAL_STACK_SIZE };
            static constexpr uint QUEUE_SIZE { 2 };

            uint8_t m_buffer_data[QUEUE_SIZE*sizeof(channels_type)];
            StaticMessageBuffer_t m_buffer_buf;
            MessageBufferHandle_t m_buffer;

            StaticTask_t m_task_buf;
            StackType_t m_task_stack[TASK_STACK_SIZE];
            TaskHandle_t m_task;

            channels_type m_channels;
            mapping_type m_mapping;

            callback_type m_callback;

            void run();
    };

};