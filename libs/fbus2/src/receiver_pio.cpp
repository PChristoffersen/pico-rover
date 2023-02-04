/**
 * @file receiver.cpp
 * @author Peter Christoffersen
 * @brief 
 * @version 0.1
 * @date 2022-07-22
 * 
 * @copyright Copyright (c) 2022
 * 
 * Implementation of the FrSky FBus2 protocol. 
 * 
 */
#include <fbus2/receiver_pio.h>

#include <pico/stdlib.h>
#include <hardware/pio.h>
#include <hardware/irq.h>

#include "uart_rx.pio.h"
#include "uart_tx.pio.h"


namespace FBus2 {


ReceiverPIO::ReceiverPIO(PIO pio, uint baudrate, uint pin, UBaseType_t task_priority, UBaseType_t lower_task_priority) :
    Receiver { baudrate, task_priority, lower_task_priority },
    m_pio { pio },
    m_pin { pin }
{

}


void ReceiverPIO::hardware_init()
{
    auto rx_offset = pio_add_program(m_pio, &uart_rx_program);
    auto tx_offset = pio_add_program(m_pio, &uart_tx_program);

    m_rx_sm = pio_claim_unused_sm(m_pio, true);
    uart_rx_program_init(m_pio, m_rx_sm, rx_offset, m_pin, m_baudrate);

    m_tx_sm = pio_claim_unused_sm(m_pio, true);
    uart_tx_program_init(m_pio, m_tx_sm, tx_offset, m_pin, m_baudrate);

    // Only drive output on LOW, on HIGH leave pin floating with a pullup
    gpio_pull_up(m_pin);
    gpio_set_oeover(m_pin, GPIO_OVERRIDE_INVERT);

}


void ReceiverPIO::task_init()
{
    init_isr();
}


void ReceiverPIO::irq_set_tx_enable(bool enable)
{
    pio_set_irqn_source_enabled(m_pio, get_core_num(), static_cast<pio_interrupt_source>(pis_sm0_tx_fifo_not_full+m_tx_sm), enable);
}


void ReceiverPIO::tx_send(const uint8_t *buf, size_t sz)
{
    while (sz && !pio_sm_is_tx_fifo_full(m_pio, m_tx_sm)) {
        uart_tx_program_putc(m_pio, m_tx_sm, *buf);
        buf++;
        sz--;
    }
    if (sz) {
        xStreamBufferSend(m_tx_buffer, buf, sz, portMAX_DELAY);
        irq_set_tx_enable(true);
    }
}


inline void ReceiverPIO::isr_handler()
{
    uint8_t ch;

    while (pio_sm_is_rx_fifo_empty(m_pio, m_rx_sm)) {
        ch = uart_rx_program_getc(m_pio, m_rx_sm);
        if (xStreamBufferSendFromISR(m_rx_buffer, &ch, 1, nullptr)!=sizeof(ch))
            break;
    }
    auto saved = taskENTER_CRITICAL_FROM_ISR();
    m_last_rx_time = get_absolute_time();
    taskEXIT_CRITICAL_FROM_ISR(saved);

    if (!xStreamBufferIsEmpty(m_tx_buffer) && !pio_sm_is_tx_fifo_full(m_pio, m_tx_sm)) {
        if (xStreamBufferReceiveFromISR(m_tx_buffer, &ch, sizeof(ch), nullptr)==sizeof(ch)) {
            uart_tx_program_putc(m_pio, m_tx_sm, ch);
        }
    }
    if (xStreamBufferIsEmpty(m_tx_buffer)) {
        irq_set_tx_enable(false);
    }
}



void ReceiverPIO::init_isr()
{
    uint irq_num;
    if (m_pio==pio0) {
        irq_num = PIO0_IRQ_0 + get_core_num();
    }
    else {
        irq_num = PIO1_IRQ_0 + get_core_num();
    }

    pio_set_irqn_source_enabled(m_pio, get_core_num(), static_cast<pio_interrupt_source>(pis_sm0_rx_fifo_not_empty+m_rx_sm), true);

    irq_set_exclusive_handler(irq_num, +[]() { static_cast<ReceiverPIO*>(m_instance)->isr_handler(); });
    irq_set_enabled(irq_num, true);
}


}


