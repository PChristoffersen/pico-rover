#include "pico_tcp_transport.h"

#ifdef RASPBERRYPI_PICO_W
#include <pico/cyw43_arch.h>
#include <lwip/pbuf.h>
#include <lwip/tcp.h>


namespace ROS {

#if 0

static bool g_connected = false;
static ip_addr_t g_remote_addr;
struct tcp_pcb *g_tcp_pcb = nullptr;

static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    //TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    if (err != ERR_OK) {
        printf("connect failed %d\n", err);
        return tcp_result(arg, err);
    }
    connected = true;
    printf("Waiting for buffer from server\n");
    return ERR_OK;
}


bool pico_tcp_transport_open(struct uxrCustomTransport *transport)
{
    auto info = reinterpret_cast<tcp_connection_info*>(transport->args);
    ip4addr_aton(info->host, &g_remote_addr);

    g_tcp_pcb = tcp_new_ip_type(IP_GET_TYPE(&g_remote_addr));

    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(g_tcp_pcb, &g_remote_addr, info->port, tcp_client_connected);
    cyw43_arch_lwip_end();

    return true;
}


bool pico_tcp_transport_close(struct uxrCustomTransport *transport)
{
    return true;
}


size_t pico_tcp_transport_write(struct uxrCustomTransport *transport, const uint8_t *buf, size_t len, uint8_t *errno)
{
    return len;
}


size_t pico_tcp_transport_read(struct uxrCustomTransport *transport, uint8_t *buf, size_t len, int timeout, uint8_t *errno)
{
    *errno = 1;

    return 0;
}


bool pico_tcp_transport_connected()
{
    return g_connected;
}

#endif

}


#endif
