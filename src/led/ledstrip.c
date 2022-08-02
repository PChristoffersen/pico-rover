#include "ledstrip.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/sem.h>
#include <hardware/pio.h>
#include <hardware/dma.h>
#include <hardware/irq.h>

#include "../boardconfig.h"
#include "ws2812.pio.h"



typedef uint32_t pixel_t;

#define CLEAR_COLOR 0x00000000

// Semaphore and alarm guarding against sending the next pixel data before the reset period of the ws2812 leds have passed
static struct semaphore g_reset_delay_complete_sem;
static alarm_id_t g_reset_delay_alarm_id;

// PIO Program
static uint g_led_sm = 0;

// DMA Pixel buffers
static uint g_led_dma = 0;
static uint g_led_dma_mask;
static uint g_led_dma_buffer_next = 0;
static pixel_t g_led_dma_buffers[2][LED_STRIP_PIXEL_COUNT];

static color_correction_t g_led_correction;
static color_t g_led_colors[LED_STRIP_PIXEL_COUNT];



static inline pixel_t _color_to_pixel(const color_t color)
{
    // TODO apply color correction
    return (COLOR_RGB_CHANNEL(color) << 8) | COLOR_WHITE_CHANNEL(color) ;
}




// Alarm callback triggered when the reset period of the LED strip has passed, and it is safe to start sending pixel data again
static int64_t _reset_delay_complete(alarm_id_t id, void *user_data)
{
    g_reset_delay_alarm_id = 0;
    sem_release(&g_reset_delay_complete_sem);
    return 0;
}


// DMA transfer of pixel data is complete
static void __isr _dma_complete_handler()
{
    if (dma_hw->ints0 & g_led_dma_mask) {
        // Clear the interrupt request.
        dma_hw->ints0 = g_led_dma_mask;
        if (g_reset_delay_alarm_id) {
            cancel_alarm(g_reset_delay_alarm_id);
        }
        g_reset_delay_alarm_id = add_alarm_in_us(LED_STRIP_RESET_DELAY_US, _reset_delay_complete, NULL, true);
    }
}


void led_strip_init()
{

    g_led_correction = COLOR_CORRECTION_TypicalLEDStrip;

    gpio_init(LED_STRIP_PIN);
    gpio_set_dir(LED_STRIP_PIN, GPIO_OUT);
    gpio_put(LED_STRIP_PIN, 0);

    // Initialize pixel buffers
    for (uint i=0; i<LED_STRIP_PIXEL_COUNT; ++i) {
        g_led_dma_buffers[0][i] = 0x00000000;
        g_led_dma_buffers[1][i] = 0x00000000;
    }
    g_led_dma_buffer_next = 0;

    // Setup PIO program
    uint offset = pio_add_program(LED_STRIP_PIO, &ws2812_program);
    g_led_sm = pio_claim_unused_sm(LED_STRIP_PIO, true);
    ws2812_program_init(LED_STRIP_PIO, g_led_sm, offset, LED_STRIP_PIN, LED_STRIP_FREQUENCY, LED_STRIP_IS_RGBW);

    // Initialize reset delay semaphore
    g_reset_delay_alarm_id = 0;
    sem_init(&g_reset_delay_complete_sem, 1, 1);

    // Setup DMA
    g_led_dma = dma_claim_unused_channel(true);
    g_led_dma_mask = 1u << g_led_dma;

    dma_channel_config config = dma_channel_get_default_config(g_led_dma);
    channel_config_set_dreq(&config, pio_get_dreq(LED_STRIP_PIO, g_led_sm, true)); 
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
    channel_config_set_read_increment(&config, true); // We want DMA reading to PIO
    channel_config_set_write_increment(&config, false); // We don't want DMA writing from PIO

    dma_channel_configure(g_led_dma,
                          &config,
                          &LED_STRIP_PIO->txf[g_led_sm],
                          NULL,
                          LED_STRIP_PIXEL_COUNT,
                          false);


    dma_channel_set_irq0_enabled(g_led_dma, true);
    irq_set_exclusive_handler(DMA_IRQ_0, _dma_complete_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    // Show initial
    led_strip_fill(COLOR_BLACK);
    led_strip_show();
}


void led_strip_set_correction(color_correction_t correction)
{
    g_led_correction = correction;
}



void led_strip_show()
{
    // Copy pixels to DMA buffer
    uint32_t *buffer = g_led_dma_buffers[g_led_dma_buffer_next];
    for (uint i=0; i<LED_STRIP_PIXEL_COUNT; ++i) {
        buffer[i] = _color_to_pixel(g_led_colors[i]);
    }

    // Start transfer    
    sem_acquire_blocking(&g_reset_delay_complete_sem); // Wait for previous to complete
    dma_channel_set_read_addr(g_led_dma, &g_led_dma_buffers[g_led_dma_buffer_next], true); // Start dma 
    g_led_dma_buffer_next = !g_led_dma_buffer_next; // Flip buffers
}


void led_strip_fill(color_t color) 
{
    for (uint i=0; i<LED_STRIP_PIXEL_COUNT; ++i) {
        g_led_colors[i] = color_correct(color, g_led_correction);
    }
}

