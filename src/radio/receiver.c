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
#include "receiver.h"

#include <pico/stdlib.h>
#include <pico/util/queue.h>
#include <hardware/uart.h>

#include "../boardconfig.h"
#include "frsky.h"


#define RC_RECEIVER_ID 0x67
#define RC_BUFFER_SIZE 64u
#define RC_TELEMETRY_QUEUE_SIZE 32u

enum state_t {
    RCS_SYNCING,
    RCS_READ_CONTROL,
    RCS_READ_DOWNLINK,
    RCS_WAIT_WRITE_UPLINK,
    RCS_WRITE_UPLINK,
    RCS_READ_UPLINK
};
enum uart_state_t {
    RCUS_IDLE,
    RCUS_RX,
    RCUS_TX,
};

static bool g_receiver_connected = false;

static enum state_t g_receiver_state = RCS_SYNCING;
static enum uart_state_t g_receiver_uart_state = RCUS_RX;
static absolute_time_t g_receiver_write_start;

static uint8_t g_receiver_buffer[RC_BUFFER_SIZE];
static uint g_receiver_buffer_pos = 0;
static uint g_receiver_buffer_required = 2;

static queue_t g_receiver_telemetry_queue;





static bool _receiver_buffer_fill()
{
    uint8_t ch;
    while (g_receiver_buffer_pos<g_receiver_buffer_required && uart_is_readable(RADIO_RECEIVER_UART)) {
        uart_read_blocking(RADIO_RECEIVER_UART, &ch, 1);
        g_receiver_buffer[g_receiver_buffer_pos++] = ch;
    }
    return g_receiver_buffer_pos>=g_receiver_buffer_required;
}


static bool _receiver_buffer_drain()
{
    uint8_t ch;
    while (g_receiver_buffer_pos<g_receiver_buffer_required && uart_is_writable(RADIO_RECEIVER_UART)) {
        ch = g_receiver_buffer[g_receiver_buffer_pos++];
        uart_write_blocking(RADIO_RECEIVER_UART, &ch, 1);
    }
    return g_receiver_buffer_pos>=g_receiver_buffer_required;
}


static void _receiver_buffer_drop(uint n)
{
    g_receiver_buffer_pos -= n;
    for (uint i=0; i<g_receiver_buffer_pos; ++i) {
        g_receiver_buffer[i] = g_receiver_buffer[i+n];
    }
}


static void _receiver_begin_sync()
{
    //printf("Begin SYNC!!\n");
    g_receiver_state = RCS_SYNCING;
    g_receiver_uart_state = RCUS_RX;
    g_receiver_buffer_required = 2;
}


static void _receiver_begin_read_control()
{
    //printf("Begin CTRL\n");
    g_receiver_state = RCS_READ_CONTROL;
    g_receiver_uart_state = RCUS_RX;
    g_receiver_buffer_required = 2;
}


static void _receiver_begin_read_downlink()
{
    //printf("Begin DOWN\n");
    g_receiver_state = RCS_READ_DOWNLINK;
    g_receiver_uart_state = RCUS_RX;
    g_receiver_buffer_required = FBUS_DOWNLINK_SIZE;
}


static void _receiver_begin_wait_write_uplink()
{
    //printf("Begin WAIT_WUP\n");
    g_receiver_state = RCS_WAIT_WRITE_UPLINK;
    g_receiver_uart_state = RCUS_IDLE;
    g_receiver_buffer_required = FBUS_UPLINK_SIZE;
    g_receiver_write_start = get_absolute_time();
}


static void _receiver_begin_write_uplink()
{
    //printf("Begin WUP\n");
    g_receiver_state = RCS_WRITE_UPLINK;
    g_receiver_uart_state = RCUS_TX;
    g_receiver_buffer_required = FBUS_DOWNLINK_SIZE;
}


static void _receiver_begin_read_uplink()
{
    //printf("Begin UPL\n");
    //printf("begin_read_uplink()\n");
    g_receiver_state = RCS_READ_UPLINK;
    g_receiver_uart_state = RCUS_RX;
    g_receiver_buffer_required = 1;
}




void radio_receiver_init()
{
    queue_init(&g_receiver_telemetry_queue, sizeof(radio_telemetry_t), RC_TELEMETRY_QUEUE_SIZE);
}


void radio_receiver_begin()
{
    #if DEBUG_USE_RECEIVER_UART
    printf("Debug is using receiver uart\n");
    return;
    #else
    g_receiver_buffer_pos = 0;

    uart_init(RADIO_RECEIVER_UART, RADIO_RECEIVER_BAUD_RATE);

    uart_set_translate_crlf(RADIO_RECEIVER_UART, false);
    uart_set_fifo_enabled(RADIO_RECEIVER_UART, true);

    gpio_set_function(RADIO_RECEIVER_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(RADIO_RECEIVER_RX_PIN, GPIO_FUNC_UART);

    _receiver_begin_sync();
    #endif
}



bool radio_receiver_telemetry_push(const radio_telemetry_t *event)
{
    #if DEBUG_USE_RECEIVER_UART
    return false;
    #else
    return queue_try_add(&g_receiver_telemetry_queue, event);
    #endif
}


static void _receiver_telemetry_flush()
{
    radio_telemetry_t event;
    while (queue_try_remove(&g_receiver_telemetry_queue, &event));
}





static uint8_t _receiver_checksum(const uint8_t *buffer, uint off, uint sz)
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








static inline void _receiver_do_sync()
{
    // Look for header byte
    if (g_receiver_buffer[1]!=FBUS_CONTROL_HDR) {
        _receiver_buffer_drop(1);
        return;
    }
    //printf("Got control hdr %02x %02x\n", g_receiver_buffer[0], g_receiver_buffer[1]);

    // Check size 
    if (g_receiver_buffer[0]!=FBUS_CONTROL_8CH_SIZE && g_receiver_buffer[0]!=FBUS_CONTROL_16CH_SIZE && g_receiver_buffer[0]!=FBUS_CONTROL_24CH_SIZE) {
        // Incorrect size - drop the first two bytes 
        _receiver_buffer_drop(2);
        return;
    }

    // Update required size to size of control package
    g_receiver_buffer_required = FBUS_CONTROL_SIZE(g_receiver_buffer);
    if (!_receiver_buffer_fill()) {
        return;
    }

    // Check CRC
    if (_receiver_checksum(g_receiver_buffer, FBUS_CONTROL_HDR_SIZE, g_receiver_buffer[0])!=FBUS_CONTROL_CRC(g_receiver_buffer)) {
        _receiver_buffer_drop(2);
        return;
    }
    // Control package OK

    // Everything checks out - we are in sync
    _receiver_begin_read_control();
}



static inline void _receiver_do_read_control()
{
    uint8_t n_channels = 8;

    if (g_receiver_buffer[1]!=FBUS_CONTROL_HDR) {
        // Lost sync
        _receiver_begin_sync();
        return;
    }

    if (g_receiver_buffer[1]!=FBUS_CONTROL_HDR || (g_receiver_buffer[0]!=FBUS_CONTROL_8CH_SIZE && g_receiver_buffer[0]!=FBUS_CONTROL_16CH_SIZE && g_receiver_buffer[0]!=FBUS_CONTROL_24CH_SIZE)) {
        // Lost sync
        _receiver_begin_sync();
        return;
    }

    g_receiver_buffer_required = FBUS_CONTROL_SIZE(g_receiver_buffer);
    if (!_receiver_buffer_fill()) {
        return;
    }

    // Validate CRC
    if (_receiver_checksum(g_receiver_buffer, FBUS_CONTROL_HDR_SIZE, g_receiver_buffer[0])!=FBUS_CONTROL_CRC(g_receiver_buffer)) {
        _receiver_begin_sync();
        return;
    }

    // Process package
    uint8_t flags = FBUS_CONTROL_FLAGS(g_receiver_buffer);
    uint8_t rssi = FBUS_CONTROL_RSSI(g_receiver_buffer);

    g_receiver_connected = true; // TODO check flags

    #if 0
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

    g_receiver_buffer_pos = 0;
    _receiver_begin_read_downlink();
}


static inline void _receiver_do_read_downlink()
{
    if (g_receiver_buffer[0]!=FBUS_DOWNLINK_HDR) {
        _receiver_begin_sync();
        return;
    }

    // Validate CRC
    if (_receiver_checksum(g_receiver_buffer, FBUS_DOWNLINK_HDR_SIZE, g_receiver_buffer[0])!=FBUS_DOWNLINK_CRC(g_receiver_buffer)) {
        //printf("Downlink CRC!\n");
        _receiver_begin_sync();
        return;
    }

    fbus_downlink_t *downlink = (fbus_downlink_t*)g_receiver_buffer;

    // Check if we need to respond to downlink
    if (downlink->id == RC_RECEIVER_ID) {
        g_receiver_buffer_pos = 0;
        _receiver_begin_wait_write_uplink();
        return;
    }

    g_receiver_buffer_pos = 0;
    _receiver_begin_read_uplink();
}


static inline void _receiver_do_wait_write_uplink()
{
    absolute_time_t now = get_absolute_time();
    int64_t diff = absolute_time_diff_us(g_receiver_write_start, now);
    if (diff < FBUS_UPLINK_SEND_DELAY_MIN_US) {
        // Give the receiver time to start reading
        return;
    }
    if (diff > FBUS_UPLINK_SEND_DELAY_MAX_US) {
        // We missed the send window
        g_receiver_buffer_pos = 0;
        _receiver_begin_read_uplink();
        return;
    }

    radio_telemetry_t event;
    if (!queue_try_remove(&g_receiver_telemetry_queue, &event)) {
        // No data
        g_receiver_buffer_pos = 0;
        _receiver_begin_read_control();
        return;
    }

   // Start sending uplink    
    fbus_uplink_t *uplink = (fbus_uplink_t*)g_receiver_buffer;
    uplink->size = FBUS_UPLINK_HDR;
    uplink->id = RC_RECEIVER_ID;
    uplink->prim = FBUS_UPLINK_DATA_FRAME;
    uplink->app_id = event.app_id;
    uplink->data = event.data;
    uplink->crc = _receiver_checksum(g_receiver_buffer, FBUS_UPLINK_HDR_SIZE, uplink->size);

    g_receiver_buffer_pos = 0;
    _receiver_begin_write_uplink();
    _receiver_buffer_drain();
}


static inline void _receiver_do_write_uplink()
{
    g_receiver_buffer_pos = 0;
    _receiver_begin_read_uplink();
}


static inline void _receiver_do_read_uplink()
{
    if (g_receiver_buffer[0]!=FBUS_UPLINK_HDR) {
        // No uplink
        _receiver_begin_read_control();
        return;
    }

    g_receiver_buffer_required = FBUS_UPLINK_SIZE;
    if (!_receiver_buffer_fill()) {
        return;
    }

    // Validate CRC
    if (_receiver_checksum(g_receiver_buffer, FBUS_UPLINK_HDR_SIZE, g_receiver_buffer[0])!=FBUS_UPLINK_CRC(g_receiver_buffer)) {
        //printf("Uplink CRC!\n");
        _receiver_begin_sync();
        return;
    }

    g_receiver_buffer_pos = 0;
    _receiver_begin_read_control();
}



bool radio_receiver_update() 
{
    #if DEBUG_USE_RECEIVER_UART
    return false;
    #else

    switch (g_receiver_uart_state) {
        case RCUS_RX:
            if (!_receiver_buffer_fill()) {
                return false;
            }
            break;
        case RCUS_TX:
            if (!_receiver_buffer_drain()) {
                return false;
            }
            break;
    }
    if (!g_receiver_connected) {
        _receiver_telemetry_flush();
    }


   switch (g_receiver_state) {
    case RCS_SYNCING:
        _receiver_do_sync();
        break;
    case RCS_READ_CONTROL:
        _receiver_do_read_control();
        break;
    case RCS_READ_DOWNLINK:
        _receiver_do_read_downlink();
        break;
    case RCS_WAIT_WRITE_UPLINK:
        _receiver_do_wait_write_uplink();
        break;
    case RCS_WRITE_UPLINK:
        _receiver_do_write_uplink();
        break;
    case RCS_READ_UPLINK:
        _receiver_do_read_uplink();
        break;
    }    

    return true;
    #endif
}