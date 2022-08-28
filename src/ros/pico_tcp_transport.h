#pragma once

#include <pico/stdlib.h>
#include <uxr/client/profile/transport/custom/custom_transport.h>

#ifdef RASPBERRYPI_PICO_W

#include <pico/cyw43_arch.h>


namespace ROS {

    class TCPConnection {
        const char *host;
        uint16_t port;
    };

    bool pico_tcp_transport_open(struct uxrCustomTransport *transport);
    bool pico_tcp_transport_close(struct uxrCustomTransport *transport);
    size_t pico_tcp_transport_write(struct uxrCustomTransport *transport, const uint8_t *buf, size_t len, uint8_t *errno);
    size_t pico_tcp_transport_read(struct uxrCustomTransport *transport, uint8_t *buf, size_t len, int timeout, uint8_t *errno);

    bool pico_tcp_transport_connected();

};

#endif
