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
#include <hardware/pio.h>

namespace FBus2 {

    class ReceiverPIO : public Receiver {
        private:


        public:
            static constexpr size_t MAX_CHANNELS { 24 };
            using channels_type = Channels;

            ReceiverPIO(PIO pio, uint baudrate, uint pin, UBaseType_t task_priority, UBaseType_t lower_task_priority);
            ReceiverPIO(const ReceiverPIO&) = delete; // No copy constructor
            ReceiverPIO(ReceiverPIO&&) = delete; // No move constructor

        private:
            // Config
            const uint m_pin;
            PIO m_pio;

            uint m_rx_sm;
            uint m_tx_sm;

            virtual void hardware_init() override;
            virtual void task_init() override;

            inline void irq_set_tx_enable(bool enable);
            virtual void tx_send(const uint8_t *buf, size_t sz) override;

            void init_isr();
            inline void isr_handler();
    };

}
