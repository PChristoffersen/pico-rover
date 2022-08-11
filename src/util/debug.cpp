#include "debug.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <pico/stdio/driver.h>
#include <hardware/pio.h>

#include "../boardconfig.h"
#include "uart_tx.pio.h"

static uint debug_tx_sm = 0;

static void _debug_out_chars(const char *buf, int len)
{
    for (int i = 0; i <len; i++) {
        uart_tx_program_putc(DEBUG_GPIO_PIO, debug_tx_sm, buf[i]);
    }
}

stdio_driver_t debug_stdio = {
    .out_chars = _debug_out_chars,
    .out_flush = nullptr,
    .in_chars = nullptr,
    .next = nullptr,
#if PICO_STDIO_ENABLE_CRLF_SUPPORT
    .last_ended_with_cr = false,
    .crlf_enabled = PICO_STDIO_DEFAULT_CRLF,
#endif
};


void debug_init()
{
    if constexpr (DEBUG_GPIO_ENABLED) {
        debug_tx_sm = pio_claim_unused_sm(DEBUG_GPIO_PIO, true);
        uint offset = pio_add_program(DEBUG_GPIO_PIO, &uart_tx_program);

        uart_tx_program_init(DEBUG_GPIO_PIO, debug_tx_sm, offset, DEBUG_GPIO_TX_PIN, DEBUG_GPIO_SPEED);

        stdio_set_driver_enabled(&debug_stdio, true);
    }

    debug_pin_init();
}


#ifndef NDEBUG

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
static bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

void debug_i2c_scan_bus()
{
    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(i2c_default, addr, &rxdata, 1, false);

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");
}


#endif
