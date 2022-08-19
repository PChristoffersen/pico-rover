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
    m_state { State::SYNCING },
    m_control_packets { 0 }
{
    assert(m_instance==nullptr);
    mutex_init(&m_mutex);
    m_instance = this;
}


void Receiver::init()
{
    assert(m_instance==this);
}



inline void Receiver::irq_set_tx_enable(bool enable)
{
    hw_write_masked(&uart_get_hw(m_uart)->imsc, bool_to_bit(enable) << UART_UARTIMSC_TXIM_LSB, UART_UARTIMSC_TXIM_BITS);
}

void Receiver::start_tx()
{
    m_tx_buffer.enter_blocking();
    while (uart_is_writable(m_uart) && !m_tx_buffer.empty()) {
        uint8_t ch = m_tx_buffer.head();
        uart_write_blocking(m_uart, &ch, sizeof(ch));
        m_tx_buffer.pop(1);
    }
    if (!m_tx_buffer.empty()) {
        irq_set_tx_enable(true);
    }
    m_tx_buffer.exit();
}


inline void Receiver::isr_handler()
{
    uint8_t ch;

    auto status = uart_get_hw(m_uart)->mis;

    m_rx_buffer.enter_blocking();
    while (uart_is_readable(m_uart)) {
        uart_read_blocking(m_uart, &ch, sizeof(ch));
        m_rx_buffer.push(ch);
    }
    m_last_rx_time = get_absolute_time();
    m_rx_buffer.exit();

    if (status & UART_UARTMIS_TXMIS_BITS) {
        m_tx_buffer.enter_blocking();
        while (uart_is_writable(m_uart) && !m_tx_buffer.empty()) {
            ch = m_tx_buffer.pop();
            uart_write_blocking(m_uart, &ch, sizeof(ch));
        }
        if (m_tx_buffer.empty()) {
            irq_set_tx_enable(false);
        }
        m_tx_buffer.exit();
    }
}



void Receiver::begin()
{
    assert(m_instance==this);
    m_rx_buffer.clear();
    m_tx_buffer.clear();

    #ifndef DEBUG_USE_RADIO_PINS
    gpio_set_function(m_tx_pin, GPIO_FUNC_UART);
    gpio_set_function(m_rx_pin, GPIO_FUNC_UART);
    #endif

    uart_init(m_uart, m_baudrate);
    #if PICO_UART_ENABLE_CRLF_SUPPORT
    uart_set_translate_crlf(m_uart, false);
    #endif

    uint UART_IRQ = m_uart == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, +[]() { m_instance->isr_handler(); });
    irq_set_enabled(UART_IRQ, true);

    uart_get_hw(m_uart)->imsc = (1u << UART_UARTIMSC_RTIM_LSB)|(1u << UART_UARTIMSC_RXIM_LSB);
    // Set minimum rx threshold to 1/2
    hw_write_masked(&uart_get_hw(m_uart)->ifls, 0b010 << UART_UARTIFLS_RXIFLSEL_LSB, UART_UARTIFLS_RXIFLSEL_BITS);
    // Set maximum tx threshold to 1/2
    hw_write_masked(&uart_get_hw(m_uart)->ifls, 0b000 << UART_UARTIFLS_TXIFLSEL_LSB, UART_UARTIFLS_TXIFLSEL_BITS);

    begin_sync();
}



void Receiver::lost_sync()
{
    debugf("Lost sync for too long\n");

    mutex_enter_blocking(&m_mutex);

    m_channels.set_flags(Flags::INITIAL_VALUE);
    m_channels.set_rssi(0);
    m_channels.set_sync(false);

    mutex_exit(&m_mutex);

    m_control_callback(m_channels);
}



/**
 * @brief Calculate how many us to wait for serial buffer to fill
 * 
 * @param bytes number of bytes to wait for
 * @return uint64_t wait time in us
 */
uint64_t Receiver::buffer_wait_time_us(int bytes) 
{
    if (bytes<0) {
        return 0;
    }
    uint bits = std::min<uint>(bytes, BUFFER_MAX_WAIT_CHARS) * 10u; // Calculate bits to wait for including start and stop bit
    uint64_t wait_us = 1000000ull * bits / m_baudrate;
    return wait_us;
}


void Receiver::begin_sync()
{
    debugf("Begin SYNC!!   %u\n", m_rx_buffer.size_blocking());
    m_state = State::SYNCING;
    m_sync_begin_time = get_absolute_time();
}


void Receiver::begin_read_control()
{
    //printf("Begin CTRL   %u\n", m_rx_buffer.size_blocking());
    m_state = State::READ_CONTROL;
}


void Receiver::begin_read_downlink()
{
    //printf("Begin DOWN   %u\n", m_rx_buffer.size_blocking());
    m_state = State::READ_DOWNLINK;
}


void Receiver::begin_wait_write_uplink()
{
    //printf("Begin WAIT_WUP\n");
    m_state = State::WAIT_WRITE_UPLINK;
}


void Receiver::begin_write_uplink()
{
    //printf("Begin WUP\n");
    m_state = State::WRITE_UPLINK;
}


void Receiver::begin_read_uplink()
{
    //printf("Begin UPL\n");
    m_state = State::READ_UPLINK;
}





absolute_time_t Receiver::do_sync()
{
    size_t bufsz;
    
    if (!m_channels.flags().frameLost() && absolute_time_diff_us(m_sync_begin_time, get_absolute_time())>SYNC_TIMEOUT) {
        // We lost sync too long, set frame lost
        lost_sync();
    }

  again:
    bufsz = m_rx_buffer.size_blocking();

    if (bufsz<2) {
        return make_timeout_time_us(buffer_wait_time_us(FBUS_CONTROL_8CH_SIZE-bufsz));
    }

    // Look for header byte
    if (m_rx_buffer[1]!=FBUS_CONTROL_HDR) {
        m_rx_buffer.pop_blocking(1);
        goto again;
    }
    //printf("Got control hdr %02x %02x\n", g_receiver_buffer[0], g_receiver_buffer[1]);

    // Check size 
    if (m_rx_buffer[0]!=FBUS_CONTROL_8CH_SIZE && m_rx_buffer[0]!=FBUS_CONTROL_16CH_SIZE && m_rx_buffer[0]!=FBUS_CONTROL_24CH_SIZE) {
        // Incorrect size - drop the first two bytes 
        m_rx_buffer.pop_blocking(2);
        goto again;
    }

    // Check if we have a full control package
    if (bufsz<fbus_control_size(m_rx_buffer)) {
        return make_timeout_time_us(buffer_wait_time_us(fbus_control_size(m_rx_buffer)-bufsz));
    }

    // Check CRC
    if (fbus_checksum(m_rx_buffer, FBUS_CONTROL_HDR_SIZE, m_rx_buffer[0])!=fbus_control_crc(m_rx_buffer)) {
        m_rx_buffer.pop_blocking(2);
        return get_absolute_time();
    }
    // Control package OK

    // Everything checks out - we are in sync
    begin_read_control();

    return get_absolute_time();
}



absolute_time_t Receiver::do_read_control()
{
    size_t bufsz = m_rx_buffer.size_blocking();

    if (bufsz<FBUS_CONTROL_HDR_SIZE) {
        return make_timeout_time_us(buffer_wait_time_us(FBUS_CONTROL_8CH_SIZE-bufsz));
    }

    if (m_rx_buffer[1]!=FBUS_CONTROL_HDR) {
        // Lost sync
        begin_sync();
        return get_absolute_time();
    }

    if (m_rx_buffer[1]!=FBUS_CONTROL_HDR || (m_rx_buffer[0]!=FBUS_CONTROL_8CH_SIZE && m_rx_buffer[0]!=FBUS_CONTROL_16CH_SIZE && m_rx_buffer[0]!=FBUS_CONTROL_24CH_SIZE)) {
        // Lost sync
        begin_sync();
        return get_absolute_time();
    }

    if (bufsz<fbus_control_size(m_rx_buffer)) {
        return make_timeout_time_us(buffer_wait_time_us(fbus_control_size(m_rx_buffer)-bufsz));
    }

    // Validate CRC
    if (fbus_checksum(m_rx_buffer, FBUS_CONTROL_HDR_SIZE, m_rx_buffer[0])!=fbus_control_crc(m_rx_buffer)) {
        begin_sync();
        return get_absolute_time();
    }

    // Process package
    mutex_enter_blocking(&m_mutex);

    static_assert(FBUS_CONTROL_8CH_SIZE+FBUS_CONTROL_HDR_SIZE+sizeof(fbus_control_8_t::crc)==sizeof(fbus_control_8_t), "Expected control size to match struct");
    static_assert(FBUS_CONTROL_16CH_SIZE+FBUS_CONTROL_HDR_SIZE+sizeof(fbus_control_16_t::crc)==sizeof(fbus_control_16_t), "Expected control size to match struct");
    static_assert(FBUS_CONTROL_24CH_SIZE+FBUS_CONTROL_HDR_SIZE+sizeof(fbus_control_24_t::crc)==sizeof(fbus_control_24_t), "Expected control size to match struct");


    m_channels.set_sync(true);
    m_channels.set_seq(m_control_packets);

    switch (m_rx_buffer[0]) {
        case FBUS_CONTROL_8CH_SIZE:
            {
                fbus_control_8_t control;
                m_rx_buffer.enter_blocking();
                m_rx_buffer.copy(reinterpret_cast<uint8_t*>(&control), sizeof(control), 0);
                m_rx_buffer.pop(sizeof(control));
                m_rx_buffer.exit();

                m_channels.set_flags(control.flags);
                m_channels.set_rssi(control.rssi);
                m_channels.set_count(8);
                fbus_get_8channel<ChannelValue::raw_type>(control.channels, 0u, m_channels, 0u);
            }
            break;
        case FBUS_CONTROL_16CH_SIZE:
            {
                fbus_control_16_t control;
                m_rx_buffer.enter_blocking();
                m_rx_buffer.copy(reinterpret_cast<uint8_t*>(&control), sizeof(control), 0);
                m_rx_buffer.pop(sizeof(control));
                m_rx_buffer.exit();

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
                m_rx_buffer.enter_blocking();
                m_rx_buffer.copy(reinterpret_cast<uint8_t*>(&control), sizeof(control), 0);
                m_rx_buffer.pop(sizeof(control));
                m_rx_buffer.exit();

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
            mutex_exit(&m_mutex);
            m_rx_buffer.pop_blocking(fbus_control_size(m_rx_buffer));
            begin_sync();
            return get_absolute_time();
    }

    mutex_exit(&m_mutex);

    m_control_callback(m_channels);

    m_control_packets++;

    begin_read_downlink();
    return get_absolute_time();
}


absolute_time_t Receiver::do_read_downlink()
{
    size_t bufsz = m_rx_buffer.size_blocking();

    if (bufsz<FBUS_DOWNLINK_HDR_SIZE) {
        return make_timeout_time_us(buffer_wait_time_us(FBUS_DOWNLINK_SIZE-bufsz));
    }

    if (m_rx_buffer[0]!=FBUS_DOWNLINK_HDR) {
        begin_sync();
        return get_absolute_time();
    }

    if (bufsz<FBUS_DOWNLINK_SIZE) {
        return make_timeout_time_us(buffer_wait_time_us(FBUS_DOWNLINK_SIZE-bufsz));
    }

    // Validate CRC
    if (fbus_checksum(m_rx_buffer, FBUS_DOWNLINK_HDR_SIZE, m_rx_buffer[0])!=fbus_downlink_crc(m_rx_buffer)) {
        begin_sync();
        return get_absolute_time();
    }

    static_assert(FBUS_DOWNLINK_SIZE==sizeof(fbus_downlink_t), "Expected downlink size to match struct");
    fbus_downlink_t downlink;

    m_rx_buffer.enter_blocking();
    m_rx_buffer.copy(reinterpret_cast<uint8_t*>(&downlink), sizeof(downlink), 0);
    m_rx_buffer.pop(FBUS_DOWNLINK_SIZE);
    m_rx_buffer.exit();

    // Check if we need to respond to downlink
    if (downlink.id == RECEIVER_ID && m_telemetry_provider) {
        begin_wait_write_uplink();
        return get_absolute_time();
    }

    begin_read_uplink();
    return get_absolute_time();
}


absolute_time_t Receiver::do_wait_write_uplink()
{
    m_rx_buffer.enter_blocking();
    absolute_time_t last_rx = m_last_rx_time;
    m_rx_buffer.exit();

    absolute_time_t now = get_absolute_time();
    int64_t diff = absolute_time_diff_us(last_rx, now);

    if (diff < FBUS_UPLINK_SEND_DELAY_MIN_US) {
        // Give the receiver time to start reading
        return delayed_by_us(last_rx, FBUS_UPLINK_SEND_DELAY_MIN_US);
    }
    if (diff > FBUS_UPLINK_SEND_DELAY_MAX_US) {
        // We missed the send window
        m_telemetry_skipped++;
        begin_read_uplink();
        return now;
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

    m_tx_buffer.enter_blocking();
    m_tx_buffer.push(uplink_ptr, sizeof(uplink));
    m_tx_buffer.exit();
    start_tx();

    m_telemetry_sent++;

    begin_write_uplink();

    return make_timeout_time_us(buffer_wait_time_us(sizeof(fbus_uplink_t)));
}


absolute_time_t Receiver::do_write_uplink()
{
    if (!m_tx_buffer.empty()) {
        // We still have bytes to send, delay by the time it takes to send 10 bytes
        return make_timeout_time_us(buffer_wait_time_us(4));
    }
    if (uart_get_hw(m_uart)->fr & UART_UARTFR_BUSY_BITS) {
        // UART is still transmitting
        return make_timeout_time_us(buffer_wait_time_us(4));
    }

    begin_read_uplink();
    return get_absolute_time();
}


absolute_time_t Receiver::do_read_uplink()
{
    size_t bufsz = m_rx_buffer.size_blocking();

    auto wait_time = absolute_time_diff_us(m_last_rx_time, get_absolute_time());

    if (bufsz==0 && wait_time > FBUS_UPLINK_SEND_TIMEOUT_US) {
        // No uplink is comming
        begin_read_control();
        return make_timeout_time_us(FBUS_UPLINK_POST_DELAY_US-wait_time);
    }

    if (bufsz<FBUS_UPLINK_HDR) {
        return make_timeout_time_us(buffer_wait_time_us(FBUS_UPLINK_SIZE-bufsz));
    }

    if (m_rx_buffer[0]!=FBUS_UPLINK_HDR) {
        // No uplink
        begin_read_control();
        return get_absolute_time();
    }

    if (bufsz<FBUS_UPLINK_SIZE) {
        return make_timeout_time_us(buffer_wait_time_us(FBUS_UPLINK_SIZE-bufsz));
    }

    // Validate CRC
    if (fbus_checksum(m_rx_buffer, FBUS_UPLINK_HDR_SIZE, m_rx_buffer[0])!=fbus_uplink_crc(m_rx_buffer)) {
        begin_sync();
        return get_absolute_time();
    }

    m_rx_buffer.pop_blocking(FBUS_UPLINK_SIZE);

    begin_read_control();
    return make_timeout_time_us(FBUS_UPLINK_POST_DELAY_US);
}



absolute_time_t Receiver::update() 
{
    absolute_time_t next;

    switch (m_state) {
        case State::SYNCING:
            next = do_sync();
            break;
        case State::READ_CONTROL:
            next = do_read_control();
            break;
        case State::READ_DOWNLINK:
            next = do_read_downlink();
            break;
        case State::WAIT_WRITE_UPLINK:
            next = do_wait_write_uplink();
            break;
        case State::WRITE_UPLINK:
            next = do_write_uplink();
            break;
        case State::READ_UPLINK:
            next = do_read_uplink();
            break;
        default:
            next = make_timeout_time_ms(1000);
    }    

    return next;
}


#ifndef NDEBUG
void Receiver::print_stats()
{
    printf("Receiver: control: %d   telemetry: sent=%d  skipped=%d\n", m_control_packets, m_telemetry_sent, m_telemetry_skipped);
}
#endif



}


