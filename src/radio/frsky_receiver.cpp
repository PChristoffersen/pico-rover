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

#include "frsky_protocol.h"

namespace Radio::FrSky {

Receiver::Receiver(uart_inst_t *uart, uint tx_pin, uint rx_pin, uint baudrate) :
    m_uart { uart },
    m_baudrate { baudrate },
    m_tx_pin { tx_pin },
    m_rx_pin { rx_pin },
    m_state { State::SYNCING },
    m_connected { false }
{
    mutex_init(&m_mutex);
}


void Receiver::init()
{
    queue_init(&m_telemetry_queue, sizeof(Telemetry), TELEMETRY_QUEUE_SIZE);
}


void Receiver::begin()
{
    m_rx_buffer.clear();
    m_tx_buffer.clear();
    m_connected = false;

    uart_init(m_uart, m_baudrate);
    uart_set_translate_crlf(m_uart, false);
    uart_set_fifo_enabled(m_uart, true);

    gpio_set_function(m_tx_pin, GPIO_FUNC_UART);
    gpio_set_function(m_rx_pin, GPIO_FUNC_UART);

    begin_sync();
}



inline void Receiver::buffers_update()
{
    uint8_t ch;
    while (!m_tx_buffer.empty() && uart_is_writable(m_uart)) {
        uart_write_blocking(m_uart, &m_tx_buffer.head(), 1);
        m_tx_buffer.pop(1);
    }
    while (uart_is_readable(m_uart)) {
        uart_read_blocking(m_uart, &ch, 1);
        m_rx_buffer.push(ch);
    }
}


/**
 * @brief Calculate how many us to wait for serial buffer to fill
 * 
 * @param bytes number of bytes to wait for
 * @return uint64_t wait time in us
 */
uint64_t Receiver::buffer_wait_time_us(size_t bytes) 
{
    uint64_t bits = MAX(bytes, BUFFER_MAX_WAIT_CHARS) * 10ull; // Calculate bits to wait for including start and stop bit
    uint64_t wait_us = bits * 1000000ull / m_baudrate;
    return wait_us;
}


void Receiver::begin_sync()
{
    //printf("Begin SYNC!!\n");
    m_state = State::SYNCING;
}


void Receiver::begin_read_control()
{
    //printf("Begin CTRL\n");
    m_state = State::READ_CONTROL;
}


void Receiver::begin_read_downlink()
{
    //printf("Begin DOWN\n");
    m_state = State::READ_DOWNLINK;
}


void Receiver::begin_wait_write_uplink()
{
    //printf("Begin WAIT_WUP\n");
    m_state = State::WAIT_WRITE_UPLINK;
    m_uplink_start = get_absolute_time();
}


void Receiver::begin_write_uplink()
{
    //printf("Begin WUP\n");
    m_state = State::WRITE_UPLINK;
}


void Receiver::begin_read_uplink()
{
    //printf("Begin UPL\n");
    //printf("begin_read_uplink()\n");
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





bool Receiver::do_sync(absolute_time_t &wait)
{
    size_t bufsz = m_rx_buffer.size();

    if (bufsz<2) {
        wait = make_timeout_time_us(buffer_wait_time_us(FBUS_CONTROL_8CH_SIZE-bufsz));
        return false;
    }

    // Look for header byte
    if (m_rx_buffer[1]!=FBUS_CONTROL_HDR) {
        m_rx_buffer.pop(1);
        return true;
    }
    //printf("Got control hdr %02x %02x\n", g_receiver_buffer[0], g_receiver_buffer[1]);

    // Check size 
    if (m_rx_buffer[0]!=FBUS_CONTROL_8CH_SIZE && m_rx_buffer[0]!=FBUS_CONTROL_16CH_SIZE && m_rx_buffer[0]!=FBUS_CONTROL_24CH_SIZE) {
        // Incorrect size - drop the first two bytes 
        m_rx_buffer.pop(2);
        return true;
    }

    // Check if we have a full control package
    if (bufsz<fbus_control_size(m_rx_buffer)) {
        wait = make_timeout_time_us(buffer_wait_time_us(fbus_control_size(m_rx_buffer)-bufsz));
        return false;
    }

    // Check CRC
    if (checksum(m_rx_buffer, FBUS_CONTROL_HDR_SIZE, m_rx_buffer[0])!=fbus_control_crc(m_rx_buffer)) {
        m_rx_buffer.pop(2);
        return true;
    }
    // Control package OK

    // Everything checks out - we are in sync
    begin_read_control();
    return true;
}



bool Receiver::do_read_control(absolute_time_t &wait)
{
    size_t bufsz = m_rx_buffer.size();

    if (bufsz<FBUS_CONTROL_HDR_SIZE) {
        wait = make_timeout_time_us(buffer_wait_time_us(FBUS_CONTROL_8CH_SIZE-bufsz));
        return false;
    }

    if (m_rx_buffer[1]!=FBUS_CONTROL_HDR) {
        // Lost sync
        begin_sync();
        return true;
    }

    if (m_rx_buffer[1]!=FBUS_CONTROL_HDR || (m_rx_buffer[0]!=FBUS_CONTROL_8CH_SIZE && m_rx_buffer[0]!=FBUS_CONTROL_16CH_SIZE && m_rx_buffer[0]!=FBUS_CONTROL_24CH_SIZE)) {
        // Lost sync
        begin_sync();
        return true;
    }

    if (bufsz<fbus_control_size(m_rx_buffer)) {
        wait = make_timeout_time_us(buffer_wait_time_us(fbus_control_size(m_rx_buffer)-bufsz));
        return false;
    }

    // Validate CRC
    if (checksum(m_rx_buffer, FBUS_CONTROL_HDR_SIZE, m_rx_buffer[0])!=fbus_control_crc(m_rx_buffer)) {
        begin_sync();
        return true;
    }

    // Process package
    mutex_enter_blocking(&m_mutex);
    m_flags = fbus_control_flags(m_rx_buffer);
    m_rssi = fbus_control_rssi(m_rx_buffer);

    m_connected = true; // TODO check flags

    #if 0
    uint8_t n_channels = 8;
    set_8channel(shm_fbus->channels, fbus_buffer+FBUS_CONTROL_HDR_SIZE, 0);
    if (fbus_buffer[0]>=FBUS_CONTROL_16CH_SIZE) {
        set_8channel(shm_fbus->channels, fbus_buffer+FBUS_CONTROL_HDR_SIZE+11, 8);
        n_channels=16;
    }
    if (fbus_buffer[0]>=FBUS_CONTROL_24CH_SIZE) {
        set_8channel(shm_fbus->channels, fbus_buffer+FBUS_CONTROL_HDR_SIZE+22, 16);
        n_channels=24;
    }
    shm_fbus->n_channels = n_channels;
    shm_fbus->counter++;
    // Send servo pulses to PRU1
    int i;
    for (i=0; i<FBUS_CHANNELS; i++) {
        if (fbus_servo.map[i]!=FBUS_SERVO_UNMAPPED) {
            servo_pulse_us(fbus_servo.map[i], shm_fbus->channels[i]);
        }
    }
    #endif

    mutex_exit(&m_mutex);

    m_rx_buffer.pop(fbus_control_size(m_rx_buffer));
    begin_read_downlink();
    return true;
}


bool Receiver::do_read_downlink(absolute_time_t &wait)
{
    size_t bufsz = m_rx_buffer.size();

    if (bufsz<FBUS_DOWNLINK_HDR_SIZE) {
        wait = make_timeout_time_us(buffer_wait_time_us(FBUS_DOWNLINK_SIZE-bufsz));
        return false;
    }

    if (m_rx_buffer[0]!=FBUS_DOWNLINK_HDR) {
        begin_sync();
        return true;
    }

    if (bufsz<FBUS_DOWNLINK_SIZE) {
        wait = make_timeout_time_us(buffer_wait_time_us(FBUS_DOWNLINK_SIZE-bufsz));
        return false;
    }

    // Validate CRC
    if (checksum(m_rx_buffer, FBUS_DOWNLINK_HDR_SIZE, m_rx_buffer[0])!=fbus_downlink_crc(m_rx_buffer)) {
        //printf("Downlink CRC!\n");
        begin_sync();
        return true;
    }

    static_assert(FBUS_DOWNLINK_SIZE==sizeof(fbus_downlink_t), "Expected downlink size to match struct");
    fbus_downlink_t downlink;

    m_rx_buffer.copy((uint8_t*)&downlink, sizeof(downlink), 0);
    m_rx_buffer.pop(FBUS_DOWNLINK_SIZE);

    // Check if we need to respond to downlink
    if (downlink.id == RECEIVER_ID) {
        begin_wait_write_uplink();
        return true;
    }

    begin_read_uplink();
    return true;
}


bool Receiver::do_wait_write_uplink(absolute_time_t &wait)
{
    absolute_time_t now = get_absolute_time();
    int64_t diff = absolute_time_diff_us(m_uplink_start, now);

    if (diff < FBUS_UPLINK_SEND_DELAY_MIN_US) {
        // Give the receiver time to start reading
        wait = delayed_by_us(m_uplink_start, FBUS_UPLINK_SEND_DELAY_MIN_US);
        return false;
    }
    if (diff > FBUS_UPLINK_SEND_DELAY_MAX_US) {
        // We missed the send window
        begin_read_uplink();
        return true;
    }

    Telemetry event;
    if (!queue_try_remove(&m_telemetry_queue, &event)) {
        // No data
        begin_read_control();
        return true;
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

    m_tx_buffer.push(uplink_ptr, sizeof(uplink));

    begin_write_uplink();
    return true;
}


bool Receiver::do_write_uplink(absolute_time_t &wait)
{
    if (!m_tx_buffer.empty()) {
        // We still have bytes to send, delay by the time it takes to send 2 bytes
        wait = make_timeout_time_us(buffer_wait_time_us(2));
        return false;
    }

    begin_read_uplink();
    return true;
}


bool Receiver::do_read_uplink(absolute_time_t &wait)
{
    size_t bufsz = m_rx_buffer.size();

    if (bufsz<FBUS_UPLINK_HDR) {
        wait = make_timeout_time_us(buffer_wait_time_us(FBUS_UPLINK_SIZE-bufsz));
        return false;
    }

    if (m_rx_buffer[0]!=FBUS_UPLINK_HDR) {
        // No uplink
        begin_read_control();
        return true;
    }

    if (m_rx_buffer.size()<FBUS_UPLINK_SIZE) {
        wait = make_timeout_time_us(buffer_wait_time_us(FBUS_UPLINK_SIZE-bufsz));
        return false;
    }

    // Validate CRC
    if (checksum(m_rx_buffer, FBUS_UPLINK_HDR_SIZE, m_rx_buffer[0])!=fbus_uplink_crc(m_rx_buffer)) {
        //printf("Uplink CRC!\n");
        begin_sync();
        return true;
    }

    begin_read_control();
    return true;
}



absolute_time_t Receiver::update() 
{
    bool again = false;
    absolute_time_t next = make_timeout_time_ms(1);
    do {
        buffers_update();

        if (!m_connected) {
            telemetry_flush();
        }

        switch (m_state) {
            case State::SYNCING:
                again = do_sync(next);
                break;
            case State::READ_CONTROL:
                again = do_read_control(next);
                break;
            case State::READ_DOWNLINK:
                again = do_read_downlink(next);
                break;
            case State::WAIT_WRITE_UPLINK:
                again = do_wait_write_uplink(next);
                break;
            case State::WRITE_UPLINK:
                again = do_write_uplink(next);
                break;
            case State::READ_UPLINK:
                again = do_read_uplink(next);
                break;
        }    
    } while (again);

    return next;
}

}
