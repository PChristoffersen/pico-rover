/**
 * @author Peter Christoffersen
 * @brief Radio receiver 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include "receiver.h"

namespace Radio::FBus2 {

    class ReceiverUART : public Receiver {
        private:


        public:
            static constexpr size_t MAX_CHANNELS { 24 };
            using channels_type = Channels;

            ReceiverUART(uart_inst_t *uart, uint baudrate, uint tx_pin, uint rx_pin, UBaseType_t task_priority, UBaseType_t lower_task_priority);
            ReceiverUART(const ReceiverUART&) = delete; // No copy constructor
            ReceiverUART(ReceiverUART&&) = delete; // No move constructor

        private:
            // Config
            const uint m_tx_pin;
            const uint m_rx_pin;
            uart_inst_t *m_uart;

            virtual void hardware_init() override;
            virtual void task_init() override;

            inline void irq_set_tx_enable(bool enable);
            virtual void tx_send(const uint8_t *buf, size_t sz) override;

            void init_isr();
            inline void isr_handler();
    };

}
