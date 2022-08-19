#pragma once

#include <pico/stdlib.h>
#include <pico/util/queue.h>

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

            absolute_time_t update();

            const channels_type &channels() const { return m_channels; }
            const mapping_type &mapping() const { return m_mapping; }

            void add_callback(callback_type::call_type cb) { m_callback.add(cb); }

        private:
            static constexpr uint QUEUE_SIZE { 2 };
            static constexpr int64_t POLL_INTERVAL { 3500 }; // 3.5 ms

            absolute_time_t m_last_event;
            queue_t m_queue;
            
            channels_type m_channels;
            mapping_type m_mapping;

            callback_type m_callback;
    };

};