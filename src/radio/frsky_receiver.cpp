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
#include "frsky_receiver.h"

#include <pico/stdlib.h>
#include <hardware/uart.h>

#include <util/debug.h>
#include <util/locking.h>
#include "frsky_protocol.h"

namespace Radio::FrSky {


Receiver *Receiver::m_instance = nullptr;




Receiver::Receiver(uart_inst_t *uart, uint baudrate, uint tx_pin, uint rx_pin) :
    m_baudrate { baudrate },
    m_tx_pin { tx_pin },
    m_rx_pin { rx_pin },
    m_uart { uart },
    m_task { nullptr }, 
    m_rx_scratch_size { 0 },
    m_rx_buffer { nullptr },
    m_tx_buffer { nullptr },
    m_state { State::SYNCING },
    m_control_packets { 0 }
{
    assert(m_instance==nullptr);
    m_instance = this;
    m_mutex = xSemaphoreCreateBinaryStatic(&m_mutex_buf);
    assert(m_mutex);
    xSemaphoreGive(m_mutex);
}


void Receiver::init()
{
    assert(m_instance==this);

    m_rx_scratch_size = 0;
    m_rx_buffer = xStreamBufferCreateStatic(RX_BUFFER_SIZE, 1, m_rx_buffer_data, &m_rx_buffer_buf);
    assert(m_rx_buffer);
    m_tx_buffer = xStreamBufferCreateStatic(TX_BUFFER_SIZE, 1, m_tx_buffer_data, &m_tx_buffer_buf);
    assert(m_tx_buffer);

    #ifndef DEBUG_USE_RADIO_PINS
    gpio_set_function(m_tx_pin, GPIO_FUNC_UART);
    gpio_set_function(m_rx_pin, GPIO_FUNC_UART);
    #endif

    uart_init(m_uart, m_baudrate);
    #if PICO_UART_ENABLE_CRLF_SUPPORT
    uart_set_translate_crlf(m_uart, false);
    #endif

    begin_sync();

    m_task = xTaskCreateStatic([](auto args){ reinterpret_cast<Receiver*>(args)->run(); }, "RC", TASK_STACK_SIZE, this, RECEIVER_TASK_PRIORITY, m_task_stack, &m_task_buf);
    vTaskCoreAffinitySet(m_task, 0b10);
    assert(m_task);
}



inline void Receiver::irq_set_tx_enable(bool enable)
{
    hw_write_masked(&uart_get_hw(m_uart)->imsc, bool_to_bit(enable) << UART_UARTIMSC_TXIM_LSB, UART_UARTIMSC_TXIM_BITS);
}


void Receiver::rx_scratch_recv(size_t bytes)
{
    while (m_rx_scratch_size<bytes) {
        size_t rsz = bytes-m_rx_scratch_size;
        xStreamBufferSetTriggerLevel(m_rx_buffer, rsz);
        auto res = xStreamBufferReceive(m_rx_buffer, m_rx_scratch+m_rx_scratch_size, rsz, portMAX_DELAY);
        m_rx_scratch_size+=res;
    }
}


void Receiver::rx_scratch_pop(size_t bytes)
{
    assert(m_rx_scratch_size>=bytes);
    for (size_t i=bytes; i<m_rx_scratch_size; ++i) {
        m_rx_scratch[i-bytes] = m_rx_scratch[i];
    }
    m_rx_scratch_size-=bytes;
}


void Receiver::tx_send(const uint8_t *buf, size_t sz)
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






inline void Receiver::isr_handler()
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



void Receiver::init_isr()
{
    uint UART_IRQ = m_uart == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, +[]() { m_instance->isr_handler(); });
    irq_set_enabled(UART_IRQ, true);

    uart_get_hw(m_uart)->imsc = (1u << UART_UARTIMSC_RTIM_LSB)|(1u << UART_UARTIMSC_RXIM_LSB);
    // Set minimum rx threshold to 1/2
    hw_write_masked(&uart_get_hw(m_uart)->ifls, 0b010 << UART_UARTIFLS_RXIFLSEL_LSB, UART_UARTIFLS_RXIFLSEL_BITS);
    // Set maximum tx threshold to 1/2
    hw_write_masked(&uart_get_hw(m_uart)->ifls, 0b000 << UART_UARTIFLS_TXIFLSEL_LSB, UART_UARTIFLS_TXIFLSEL_BITS);
}



void Receiver::lost_sync()
{
    debugf("Lost sync for too long\n");

    xSemaphoreTake(m_mutex, portMAX_DELAY);
    m_channels.set_flags(Flags::INITIAL_VALUE);
    m_channels.set_rssi(0);
    m_channels.set_sync(false);
    xSemaphoreGive(m_mutex);

    m_control_callback(m_channels);
}




void Receiver::begin_sync()
{
    debugf("Begin SYNC!!   %u\n", m_rx_scratch_size);
    m_state = State::SYNCING;
    m_sync_begin_time = get_absolute_time();
}


void Receiver::begin_read_control()
{
    //printf("Begin CTRL   %u\n", m_rx_scratch_size);
    m_state = State::READ_CONTROL;
}


void Receiver::begin_read_downlink()
{
    //printf("Begin DOWN   %u\n", m_rx_scratch_size);
    m_state = State::READ_DOWNLINK;
}


void Receiver::begin_write_uplink()
{
    //printf("Begin WUPL\n");
    m_state = State::WRITE_UPLINK;
}


void Receiver::begin_read_uplink()
{
    //printf("Begin RUPL\n");
    m_state = State::READ_UPLINK;
}





void Receiver::do_sync()
{
    if (!m_channels.flags().frameLost() && absolute_time_diff_us(m_sync_begin_time, get_absolute_time())>SYNC_TIMEOUT) {
        // We lost sync too long, set frame lost
        lost_sync();
    }

    // We need at least 2 bytes
    rx_scratch_recv(2);

    // Look for header byte
    if (m_rx_scratch[1]!=FBUS_CONTROL_HDR) {
        rx_scratch_pop(1);
        return;
    }
    //printf("Got control hdr %02x %02x\n", g_receiver_buffer[0], g_receiver_buffer[1]);

    // Check size 
    if (m_rx_scratch[0]!=FBUS_CONTROL_8CH_SIZE && m_rx_scratch[0]!=FBUS_CONTROL_16CH_SIZE && m_rx_scratch[0]!=FBUS_CONTROL_24CH_SIZE) {
        // Incorrect size - drop the first two bytes 
        rx_scratch_pop(2);
        return;
    }

    // Check if we have a full control package
    rx_scratch_recv(fbus_control_size(m_rx_scratch));

    // Check CRC
    if (fbus_checksum(m_rx_scratch, FBUS_CONTROL_HDR_SIZE, m_rx_scratch[0])!=fbus_control_crc(m_rx_scratch)) {
        rx_scratch_pop(2);
        return;
    }
    // Control package OK

    // Everything checks out - we are in sync
    begin_read_control();
}



void Receiver::do_read_control()
{
    rx_scratch_recv(FBUS_CONTROL_HDR_SIZE);

    if (m_rx_scratch[1]!=FBUS_CONTROL_HDR) {
        // Lost sync
        begin_sync();
        return;
    }

    if (m_rx_scratch[1]!=FBUS_CONTROL_HDR || (m_rx_scratch[0]!=FBUS_CONTROL_8CH_SIZE && m_rx_scratch[0]!=FBUS_CONTROL_16CH_SIZE && m_rx_scratch[0]!=FBUS_CONTROL_24CH_SIZE)) {
        // Lost sync
        begin_sync();
        return;
    }

    rx_scratch_recv(fbus_control_size(m_rx_scratch));

    // Validate CRC
    if (fbus_checksum(m_rx_scratch, FBUS_CONTROL_HDR_SIZE, m_rx_scratch[0])!=fbus_control_crc(m_rx_scratch)) {
        begin_sync();
        return;
    }

    // Process package
    xSemaphoreTake(m_mutex, portMAX_DELAY);

    static_assert(FBUS_CONTROL_8CH_SIZE+FBUS_CONTROL_HDR_SIZE+sizeof(fbus_control_8_t::crc)==sizeof(fbus_control_8_t), "Expected control size to match struct");
    static_assert(FBUS_CONTROL_16CH_SIZE+FBUS_CONTROL_HDR_SIZE+sizeof(fbus_control_16_t::crc)==sizeof(fbus_control_16_t), "Expected control size to match struct");
    static_assert(FBUS_CONTROL_24CH_SIZE+FBUS_CONTROL_HDR_SIZE+sizeof(fbus_control_24_t::crc)==sizeof(fbus_control_24_t), "Expected control size to match struct");


    m_channels.set_sync(true);
    m_channels.set_seq(m_control_packets);

    switch (m_rx_scratch[0]) {
        case FBUS_CONTROL_8CH_SIZE:
            {
                fbus_control_8_t control;
                memcpy(&control, m_rx_scratch, sizeof(control));
                rx_scratch_pop(sizeof(control));

                m_channels.set_flags(control.flags);
                m_channels.set_rssi(control.rssi);
                m_channels.set_count(8);
                fbus_get_8channel<ChannelValue::raw_type>(control.channels, 0u, m_channels, 0u);
            }
            break;
        case FBUS_CONTROL_16CH_SIZE:
            {
                fbus_control_16_t control;
                memcpy(&control, m_rx_scratch, sizeof(control));
                rx_scratch_pop(sizeof(control));

                m_channels.set_flags(control.flags);
                m_channels.set_rssi(control.rssi);
                m_channels.set_count(16);
                fbus_get_8channel<ChannelValue::raw_type>(control.channels, 0u, m_channels, 0u);
                fbus_get_8channel<ChannelValue::raw_type>(control.channels, FBUS_CONTROL_8_VALUE_SIZE, m_channels, 8u);
            }
            break;
        case FBUS_CONTROL_24CH_SIZE:
            {
                fbus_control_16_t control;
                memcpy(&control, m_rx_scratch, sizeof(control));
                rx_scratch_pop(sizeof(control));

                m_channels.set_flags(control.flags);
                m_channels.set_rssi(control.rssi);
                m_channels.set_count(24);
                fbus_get_8channel<ChannelValue::raw_type>(control.channels, 0u, m_channels, 0u);
                fbus_get_8channel<ChannelValue::raw_type>(control.channels, FBUS_CONTROL_8_VALUE_SIZE, m_channels, 8u);
                fbus_get_8channel<ChannelValue::raw_type>(control.channels, FBUS_CONTROL_8_VALUE_SIZE*2, m_channels, 16u);
            }
            break;
        default:
            // We should never get here, but drop package, and begin resync just in case
            xSemaphoreGive(m_mutex);
            rx_scratch_pop(fbus_control_size(m_rx_scratch));
            begin_sync();
            return;
    }

    xSemaphoreGive(m_mutex);

    m_control_callback(m_channels);

    m_control_packets++;

    begin_read_downlink();
}


void Receiver::do_read_downlink()
{
    // Fill buffer
    rx_scratch_recv(FBUS_DOWNLINK_SIZE);
    if (m_rx_scratch[0]!=FBUS_DOWNLINK_HDR) {
        begin_sync();
        return;
    }

    // Validate CRC
    if (fbus_checksum(m_rx_scratch, FBUS_DOWNLINK_HDR_SIZE, m_rx_scratch[0])!=fbus_downlink_crc(m_rx_scratch)) {
        begin_sync();
        return;
    }

    // Process package
    static_assert(FBUS_DOWNLINK_SIZE==sizeof(fbus_downlink_t), "Expected downlink size to match struct");
    fbus_downlink_t downlink;
    memcpy(&downlink, m_rx_scratch, FBUS_DOWNLINK_SIZE);
    rx_scratch_pop(FBUS_DOWNLINK_SIZE);

    // Check if we need to respond to downlink
    if (downlink.id == RECEIVER_ID && m_telemetry_provider) {
        begin_write_uplink();
        return;
    }

    begin_read_uplink();
}


void Receiver::do_write_uplink()
{
    //m_rx_buffer.enter_blocking();
    taskENTER_CRITICAL();
    absolute_time_t last_rx = m_last_rx_time;
    taskEXIT_CRITICAL();

    absolute_time_t now = get_absolute_time();
    int64_t diff = absolute_time_diff_us(last_rx, now);

    if (diff > FBUS_UPLINK_SEND_DELAY_MAX_US) {
        // We missed the send window
        m_telemetry_skipped++;
        begin_read_uplink();
        return;
    }

    Telemetry event = m_telemetry_provider->get_next_telemetry();

    // Start sending uplink    
    fbus_uplink_t uplink;
    uint8_t *uplink_ptr = (uint8_t*)&uplink;
    static_assert(FBUS_UPLINK_SIZE == sizeof(fbus_uplink_t), "Uplink mismatch");
    uplink.size = FBUS_UPLINK_HDR;
    uplink.id = RECEIVER_ID;
    uplink.prim = FBUS_UPLINK_DATA_FRAME;
    uplink.app_id = event.app_id;
    uplink.data = event.data;
    uplink.crc = fbus_checksum(uplink_ptr, FBUS_UPLINK_HDR_SIZE, uplink.size);

    //printf("Uplink: - %02x   %08x   - %lld\n", uplink.app_id, uplink.data, diff);
    tx_send(uplink_ptr, sizeof(uplink));

    m_telemetry_sent++;

    begin_read_uplink();
}


void Receiver::do_read_uplink()
{
    // Fill buffer
    rx_scratch_recv(FBUS_UPLINK_HDR_SIZE);
    if (m_rx_scratch[0]!=FBUS_UPLINK_HDR) {
        // No uplink
        begin_read_control();
        return;
    }
    rx_scratch_recv(FBUS_UPLINK_SIZE);

    // Validate CRC
    if (fbus_checksum(m_rx_scratch, FBUS_UPLINK_HDR_SIZE, m_rx_scratch[0])!=fbus_uplink_crc(m_rx_scratch)) {
        begin_sync();
        return;
    }

    rx_scratch_pop(FBUS_UPLINK_SIZE);

    begin_read_control();
}


void Receiver::run()
{
    init_isr();

    // Allow task to run on all cores after we have setup interrupt handler
    vTaskCoreAffinitySet(nullptr, 0b11);

    printf("Receiver task running\n");

    while (true) {
        switch (m_state) {
            case State::SYNCING:
                do_sync();
                break;
            case State::READ_CONTROL:
                do_read_control();
                break;
            case State::READ_DOWNLINK:
                do_read_downlink();
                break;
            case State::WRITE_UPLINK:
                do_write_uplink();
                break;
            case State::READ_UPLINK:
                do_read_uplink();
                break;
            default:
                assert(false);
                vTaskDelay(pdMS_TO_TICKS(10));
        }    
    }
}


#ifndef NDEBUG
void Receiver::print_stats()
{
    printf("Receiver: control: %d   telemetry: sent=%d  skipped=%d\n", m_control_packets, m_telemetry_sent, m_telemetry_skipped);
}
#endif



}


