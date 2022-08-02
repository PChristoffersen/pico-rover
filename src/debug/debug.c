#include "debug.h"


#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <pico/stdio/driver.h>
#include <hardware/pio.h>

#include "../boardconfig.h"
#include "uart_tx.pio.h"

#if GENERIC_INTR_STDIO

static uint debug_tx_sm = 0;

static void _debug_out_chars(const char *buf, int len)
{
    for (int i = 0; i <len; i++) {
        uart_tx_program_putc(DEBUG_UART_PIO, debug_tx_sm, buf[i]);
    }
}

stdio_driver_t debug_stdio = {
    .out_chars = _debug_out_chars,
#if PICO_STDIO_ENABLE_CRLF_SUPPORT
    .crlf_enabled = PICO_STDIO_DEFAULT_CRLF
#endif
};

#endif

void debug_init()
{
    #if GENERIC_INTR_STDIO
    debug_tx_sm = pio_claim_unused_sm(DEBUG_UART_PIO, true);

    uint offset = pio_add_program(DEBUG_UART_PIO, &uart_tx_program);

    uart_tx_program_init(DEBUG_UART_PIO, debug_tx_sm, offset, DEBUG_UART_TX, DEBUG_UART_SPEED);

    stdio_set_driver_enabled(&debug_stdio, true);
    #endif
}
