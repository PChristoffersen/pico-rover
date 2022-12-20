#pragma once

#include <fbus2/receiver_uart.h>
#include <fbus2/mapping.h>
#include <fbus2/channels.h>
#include <fbus2/telemetry.h>
#include <util/callback.h>
#include <util/locking.h>
#include <boardconfig.h>
#include <rtos.h>
#include "telemetry.h"

namespace Radio {

    class Receiver : public FBus2::ReceiverUART {
        public:
            using mapping_type = FBus2::TaranisX9DPlus;        
            using control_cb_type = Callback<const channels_type &, const mapping_type &>;

            Receiver() : 
                FBus2::ReceiverUART { 
                    RADIO_RECEIVER_UART, 
                    RADIO_RECEIVER_BAUD_RATE, 
                    RADIO_RECEIVER_TX_PIN, 
                    RADIO_RECEIVER_RX_PIN,
                    RECEIVER_TASK_PRIORITY,
                    RECEIVER_LOWER_TASK_PRIORITY
                }
            {

            }

            void add_callback(control_cb_type::call_type callback) { m_control_callback.add(callback); }
            void set_telemetry_provider(TelemetryProvider *provider) { m_telemetry_provider = provider; }

        protected:

            virtual void on_data(const channels_type &channels) override;
            virtual Telemetry get_next_telemetry() override;

        private:
            control_cb_type m_control_callback;
            TelemetryProvider *m_telemetry_provider;
            mapping_type m_mapping;
    };

    using Channels = FBus2::Channels;
    using Toggle = FBus2::Toggle;

}