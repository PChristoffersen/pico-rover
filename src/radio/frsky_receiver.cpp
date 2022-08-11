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
 * The only documentation of the protocol I have been able to find is:
 * 
 * https://github.com/betaflight/betaflight/wiki/The-FrSky-FPort-Protocol
 * 
 * The PDF on that page does not represent precicely what is sent, but by
 * reverse engineering dumped data from a receiver I have been able to 
 * implement a working protocol.
 * 
 */
#include "frsky_receiver.h"

#include <pico/stdlib.h>
#include <pico/util/queue.h>
#include <hardware/uart.h>

#include <util/debug.h>
#include <util/locking.h>
#include "frsky_protocol.h"

namespace Radio::FrSky {


Receiver *Receiver::m_instance = nullptr;


Receiver::ChannelData::ChannelData():
    m_rssi { 0x00 },
    m_flags { 0x00 },
    m_count { 0 } 
{
}


template <typename buffer_type>
uint8_t Receiver::checksum(const buffer_type &buffer, uint off, uint sz)
{
    uint16_t sum = 0x00;
    for (uint i=0; i<sz; i++) {
        sum += buffer[off+i];
    }
    while (sum > 0xFF) {
        sum = (sum & 0xFF) + (sum >>8);
    }
    return 0xFF - sum;
}


Receiver::Receiver(uart_inst_t *uart, uint baudrate, uint tx_pin, uint rx_pin) :
    m_uart { uart },
    m_baudrate { baudrate },
    m_tx_pin { tx_pin },
    m_rx_pin { rx_pin },
    m_state { State::SYNCING },
    m_connected { false },
    m_data_callback { nullptr }
{
    assert(m_instance==nullptr);
    mutex_init(&m_mutex);
    m_instance = this;
}


void Receiver::init()
{
    assert(m_instance==this);
    queue_init(&m_telemetry_queue, sizeof(Telemetry), TELEMETRY_QUEUE_SIZE);
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
    uint tx_cnt = 0;
    uint rx_cnt = 0;

    m_rx_buffer.enter_blocking();
    while (uart_is_readable(m_uart)) {
        uart_read_blocking(m_uart, &ch, sizeof(ch));
        m_rx_buffer.push(ch);
        rx_cnt++;
    }
    m_last_rx_time = get_absolute_time();
    m_rx_buffer.exit();

    if (status & UART_UARTMIS_TXMIS_BITS) {
        m_tx_buffer.enter_blocking();
        while (uart_is_writable(m_uart) && !m_tx_buffer.empty()) {
            ch = m_tx_buffer.pop();
            uart_write_blocking(m_uart, &ch, sizeof(ch));
            tx_cnt++;
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
    m_connected = false;

    gpio_set_function(m_tx_pin, GPIO_FUNC_UART);
    gpio_set_function(m_rx_pin, GPIO_FUNC_UART);

    uart_init(m_uart, m_baudrate);
    #if PICO_UART_ENABLE_CRLF_SUPPORT
    uart_set_translate_crlf(m_uart, false);
    #endif

    #if 1
    uint UART_IRQ = m_uart == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, +[]() { m_instance->isr_handler(); });
    irq_set_enabled(UART_IRQ, true);

    uart_get_hw(m_uart)->imsc = (1u << UART_UARTIMSC_RTIM_LSB)|(1u << UART_UARTIMSC_RXIM_LSB);
    // Set minimum rx threshold to 1/2
    hw_write_masked(&uart_get_hw(m_uart)->ifls, 0b010 << UART_UARTIFLS_RXIFLSEL_LSB, UART_UARTIFLS_RXIFLSEL_BITS);
    // Set maximum tx threshold to 1/2
    hw_write_masked(&uart_get_hw(m_uart)->ifls, 0b000 << UART_UARTIFLS_TXIFLSEL_LSB, UART_UARTIFLS_TXIFLSEL_BITS);
    #endif

    begin_sync();
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
    uint64_t bits = std::max<int>(bytes, BUFFER_MAX_WAIT_CHARS) * 10ull; // Calculate bits to wait for including start and stop bit
    uint64_t wait_us = bits * 1000000ull / m_baudrate;
    return wait_us;
}


void Receiver::begin_sync()
{
    printf("Begin SYNC!!   %u\n", m_rx_buffer.size_blocking());
    m_state = State::SYNCING;
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





bool Receiver::telemetry_push(const Telemetry &event)
{
    #if DEBUG_USE_RECEIVER_UART
    return false;
    #else
    return queue_try_add(&m_telemetry_queue, &event);
    #endif
}


void Receiver::telemetry_flush()
{
    Telemetry event;
    while (queue_try_remove(&m_telemetry_queue, &event));
}





absolute_time_t Receiver::do_sync()
{
    size_t bufsz;
    
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
    if (checksum(m_rx_buffer, FBUS_CONTROL_HDR_SIZE, m_rx_buffer[0])!=fbus_control_crc(m_rx_buffer)) {
        m_rx_buffer.pop_blocking(2);
        return get_absolute_time();
    }
    // Control package OK

    // Everything checks out - we are in sync
    begin_read_control();

    return get_absolute_time();
}



void Receiver::set_8channel(const rx_buffer_type &buffer, uint buffer_offset, uint channel_offset)
{
    m_data.set_raw(channel_offset,   (static_cast<ChannelData::raw_type>(buffer[0]))           + ((0x07 & buffer[1])<<8));
    m_data.set_raw(channel_offset+1, (static_cast<ChannelData::raw_type>(0xF8 & buffer[1])>>3) + ((0x3F & buffer[2])<<5));
    m_data.set_raw(channel_offset+2, (static_cast<ChannelData::raw_type>(0xC0 & buffer[2])>>6) + (buffer[3]<<2) + ((0x01 & buffer[4])<<10));
    m_data.set_raw(channel_offset+3, (static_cast<ChannelData::raw_type>(0xFE & buffer[4])>>1) + ((0x0F & buffer[5])<<7));
    m_data.set_raw(channel_offset+4, (static_cast<ChannelData::raw_type>(0xF0 & buffer[5])>>4) + ((0x7F & buffer[6])<<4));
    m_data.set_raw(channel_offset+5, (static_cast<ChannelData::raw_type>(0x80 & buffer[6])>>7) + (buffer[7]<<1) + ((0x03 & buffer[8])<<9));
    m_data.set_raw(channel_offset+6, (static_cast<ChannelData::raw_type>(0xFC & buffer[8])>>2) + ((0x1F & buffer[9])<<6));
    m_data.set_raw(channel_offset+7, (static_cast<ChannelData::raw_type>(0xF0 & buffer[9])>>5) + ((buffer[10])<<3));
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
    if (checksum(m_rx_buffer, FBUS_CONTROL_HDR_SIZE, m_rx_buffer[0])!=fbus_control_crc(m_rx_buffer)) {
        begin_sync();
        return get_absolute_time();
    }

    // Process package
    mutex_enter_blocking(&m_mutex);

    m_connected = true; // TODO check flags

    uint8_t n_channels = 8;
    set_8channel(m_rx_buffer, FBUS_CONTROL_HDR_SIZE, 0);
    if (m_rx_buffer[0]>=FBUS_CONTROL_16CH_SIZE) {
        set_8channel(m_rx_buffer, FBUS_CONTROL_HDR_SIZE+11, 8);
        n_channels=16;
    }
    if (m_rx_buffer[0]>=FBUS_CONTROL_24CH_SIZE) {
        set_8channel(m_rx_buffer, FBUS_CONTROL_HDR_SIZE+22, 16);
        n_channels=24;
    }
    m_data.set_count(n_channels);
    mutex_exit(&m_mutex);

    m_rx_buffer.pop_blocking(fbus_control_size(m_rx_buffer));

    if (m_data_callback) {
        m_data_callback(m_data);
    }

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
    if (checksum(m_rx_buffer, FBUS_DOWNLINK_HDR_SIZE, m_rx_buffer[0])!=fbus_downlink_crc(m_rx_buffer)) {
        begin_sync();
        return get_absolute_time();
    }

    static_assert(FBUS_DOWNLINK_SIZE==sizeof(fbus_downlink_t), "Expected downlink size to match struct");
    fbus_downlink_t downlink;

    m_rx_buffer.enter_blocking();
    m_rx_buffer.copy((uint8_t*)&downlink, sizeof(downlink), 0);
    m_rx_buffer.pop(FBUS_DOWNLINK_SIZE);
    m_rx_buffer.exit();

    // Check if we need to respond to downlink
    if (downlink.id == RECEIVER_ID) {
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
        begin_read_uplink();
        return now;
    }

    Telemetry event;
    if (!queue_try_remove(&m_telemetry_queue, &event)) {
        // No data - wait a little longer (until we miss the upload window)
        return delayed_by_us(last_rx, 100);
    }

    // Start sending uplink    
    fbus_uplink_t uplink;
    uint8_t *uplink_ptr = (uint8_t*)&uplink;
    static_assert(FBUS_UPLINK_SIZE == sizeof(fbus_uplink_t), "Uplink mismatch");
    uplink.size = FBUS_UPLINK_HDR;
    uplink.id = RECEIVER_ID;
    uplink.prim = FBUS_UPLINK_DATA_FRAME;
    uplink.app_id = event.app_id;
    uplink.data = event.data;
    uplink.crc = checksum(uplink_ptr, FBUS_UPLINK_HDR_SIZE, uplink.size);

    //printf("Uplink: - %02x   %08x   - %lld\n", uplink.app_id, uplink.data, diff);

    m_tx_buffer.enter_blocking();
    m_tx_buffer.push(uplink_ptr, sizeof(uplink));
    m_tx_buffer.exit();
    start_tx();


    begin_write_uplink();
    return now;
}


absolute_time_t Receiver::do_write_uplink()
{
    if (!m_tx_buffer.empty()) {
        // We still have bytes to send, delay by the time it takes to send 10 bytes
        return make_timeout_time_us(buffer_wait_time_us(10));
    }
    /*
    if (uart_get_hw(m_uart)->fr & UART_UARTFR_BUSY_BITS) {
        // UART is still transmitting
        return make_timeout_time_us(buffer_wait_time_us(10));
    }*/

    begin_read_uplink();
    return get_absolute_time();
}


absolute_time_t Receiver::do_read_uplink()
{
    size_t bufsz = m_rx_buffer.size_blocking();

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
    if (checksum(m_rx_buffer, FBUS_UPLINK_HDR_SIZE, m_rx_buffer[0])!=fbus_uplink_crc(m_rx_buffer)) {
        begin_sync();
        return get_absolute_time();
    }

    m_rx_buffer.pop_blocking(FBUS_UPLINK_SIZE);

    begin_read_control();
    return get_absolute_time();
}



absolute_time_t Receiver::update() 
{
    absolute_time_t next;
    //buffers_update();

    if (!m_connected) {
        telemetry_flush();
    }

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

}
