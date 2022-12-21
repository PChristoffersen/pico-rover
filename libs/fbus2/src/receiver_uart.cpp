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
#include <fbus2/receiver_uart.h>

#include <pico/stdlib.h>
#include <hardware/uart.h>

namespace FBus2 {


ReceiverUART::ReceiverUART(uart_inst_t *uart, uint baudrate, uint tx_pin, uint rx_pin, UBaseType_t task_priority, UBaseType_t lower_task_priority) :
    Receiver { baudrate, task_priority, lower_task_priority },
    m_uart { uart },
    m_tx_pin { tx_pin },
    m_rx_pin { rx_pin }
{

}


void ReceiverUART::hardware_init()
{
    gpio_set_function(m_tx_pin, GPIO_FUNC_UART);
    gpio_set_function(m_rx_pin, GPIO_FUNC_UART);

    uart_init(m_uart, m_baudrate);
    #if PICO_UART_ENABLE_CRLF_SUPPORT
    uart_set_translate_crlf(m_uart, false);
    #endif

}


void ReceiverUART::task_init()
{
    init_isr();
}


inline void ReceiverUART::irq_set_tx_enable(bool enable)
{
    hw_write_masked(&uart_get_hw(m_uart)->imsc, bool_to_bit(enable) << UART_UARTIMSC_TXIM_LSB, UART_UARTIMSC_TXIM_BITS);
}

void ReceiverUART::tx_send(const uint8_t *buf, size_t sz)
{
    while (uart_is_writable(m_uart) && sz) {
        uint8_t ch = *buf;
        uart_write_blocking(m_uart, &ch, sizeof(ch));
        sz-=1;
        buf++;
    }
    if (sz) {
        xStreamBufferSend(m_tx_buffer, buf, sz, portMAX_DELAY);
        irq_set_tx_enable(true);
    }
}


inline void ReceiverUART::isr_handler()
{
    uint8_t ch;

    auto status = uart_get_hw(m_uart)->mis;

    while (uart_is_readable(m_uart)) {
        uart_read_blocking(m_uart, &ch, sizeof(ch));
        if (xStreamBufferSendFromISR(m_rx_buffer, &ch, 1, nullptr)!=sizeof(ch))
            break;
    }
    auto saved = taskENTER_CRITICAL_FROM_ISR();
    m_last_rx_time = get_absolute_time();
    taskEXIT_CRITICAL_FROM_ISR(saved);

    if (status & UART_UARTMIS_TXMIS_BITS) {
        while (uart_is_writable(m_uart) && !xStreamBufferIsEmpty(m_tx_buffer)) {
            if (xStreamBufferReceiveFromISR(m_tx_buffer, &ch, sizeof(ch), nullptr)==sizeof(ch)) {
                uart_write_blocking(m_uart, &ch, sizeof(ch));
            }
        }
        if (xStreamBufferIsEmpty(m_tx_buffer)) {
            irq_set_tx_enable(false);
        }
    }
}



void ReceiverUART::init_isr()
{
    uint UART_IRQ = m_uart == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, +[]() { static_cast<ReceiverUART*>(m_instance)->isr_handler(); });
    irq_set_enabled(UART_IRQ, true);

    uart_get_hw(m_uart)->imsc = (1u << UART_UARTIMSC_RTIM_LSB)|(1u << UART_UARTIMSC_RXIM_LSB);
    // Set minimum rx threshold to 1/2
    hw_write_masked(&uart_get_hw(m_uart)->ifls, 0b010 << UART_UARTIFLS_RXIFLSEL_LSB, UART_UARTIFLS_RXIFLSEL_BITS);
    // Set maximum tx threshold to 1/2
    hw_write_masked(&uart_get_hw(m_uart)->ifls, 0b000 << UART_UARTIFLS_TXIFLSEL_LSB, UART_UARTIFLS_TXIFLSEL_BITS);
}


}


