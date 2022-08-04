#include "ledstrip.h"

#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>
#include <pico/sem.h>
#include <hardware/pio.h>
#include <hardware/dma.h>
#include <hardware/irq.h>

#include "ws2812.pio.h"


#define LED_STRIP_PIO              pio0
#define LED_STRIP_DMA_IRQ          DMA_IRQ_0
#define LED_STRIP_DMA_IRQ_INDEX    0
#define LED_STRIP_DMA_IRQ_SHARED   1
#define LED_STRIP_DMA_IRQ_PRIORITY 1



LEDStripBase *LEDStripBase::m_strips[MAX_STRIPS] = { nullptr, };
uint LEDStripBase::m_program_offset = 0;



// DMA transfer of pixel data is complete
void __isr LEDStripBase::dma_complete_handler()
{
    for (uint i=0; i<count_of(LEDStripBase::m_strips); ++i) {
        LEDStripBase *strip = LEDStripBase::m_strips[i];
        if (strip && dma_irqn_get_channel_status(LED_STRIP_DMA_IRQ_INDEX, strip->m_dma)) {
            dma_irqn_acknowledge_channel(LED_STRIP_DMA_IRQ_INDEX, strip->m_dma);

            // Alarm callback triggered when the reset period of the LED strip has passed, and it is safe to start sending pixel data again
            if (strip->m_reset_alarm) {
                cancel_alarm(strip->m_reset_alarm);
            }
            strip->m_reset_alarm = add_alarm_in_us(STRIP_RESET_DELAY_US, +[](alarm_id_t id, void *user_data) -> int64_t {
                LEDStripBase *strip = (LEDStripBase*)user_data;
                strip->m_reset_alarm = 0;
                sem_release(&strip->m_reset_sem);
                return 0;
            }, strip, true);
        }
    }
}


LEDStripBase::LEDStripBase(uint pin, bool is_rgbw):
    m_pin { pin },
    m_is_rgbw { is_rgbw },
    m_correction { Color::Correction::TypicalLEDStrip },
    m_brightness { Color::BRIGHTNESS_DEFAULT }
{
}



void LEDStripBase::global_init()
{
    static bool initialized = false;
    if (initialized)
        return;

    for (uint i=0; i<count_of(m_strips); ++i) {
        m_strips[i] = nullptr;
    }

    // Setup PIO program
    m_program_offset = pio_add_program(LED_STRIP_PIO, &ws2812_program);

    #if LED_STRIP_DMA_IRQ_SHARED
    irq_add_shared_handler(LED_STRIP_DMA_IRQ, dma_complete_handler, LED_STRIP_DMA_IRQ_PRIORITY);
    #else
    irq_set_exclusive_handler(LED_STRIP_DMA_IRQ, _dma_complete_handler);
    #endif
    irq_set_enabled(LED_STRIP_DMA_IRQ, true);

    initialized = true;
}

void LEDStripBase::global_add_strip(LEDStripBase *strip)
{
    for (uint i=0; i<count_of(m_strips); ++i) {
        if (m_strips[i]==nullptr) {
            m_strips[i] = strip;
            break;
        }
    }
}


void LEDStripBase::base_init(volatile void *dma_addr, size_t dma_count)
{
    global_init();

    gpio_init(m_pin);
    gpio_set_dir(m_pin, GPIO_OUT);
    gpio_put(m_pin, false);

    // Setup PIO program
    m_sm = pio_claim_unused_sm(LED_STRIP_PIO, true);
    ws2812_program_init(LED_STRIP_PIO, m_sm, m_program_offset, m_pin, STRIP_FREQUENCY, m_is_rgbw);

    // Initialize reset delay semaphore
    m_reset_alarm = 0;
    sem_init(&m_reset_sem, 1, 1);

    // Setup DMA
    m_dma = dma_claim_unused_channel(true);
    m_dma_addr = dma_addr;

    dma_channel_config config = dma_channel_get_default_config(m_dma);
    channel_config_set_dreq(&config, pio_get_dreq(LED_STRIP_PIO, m_sm, true)); 
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
    channel_config_set_read_increment(&config, true); // We want DMA reading to PIO
    channel_config_set_write_increment(&config, false); // We don't want DMA writing from PIO

    dma_channel_configure(m_dma,
                          &config,
                          &LED_STRIP_PIO->txf[m_sm],
                          dma_addr,
                          dma_count,
                          false);

    global_add_strip(this);

    #if LED_STRIP_DMA_IRQ == DMA_IRQ_0
    dma_channel_set_irq0_enabled(m_dma, true);
    #endif
    #if LED_STRIP_DMA_IRQ == DMA_IRQ_1
    dma_channel_set_irq1_enabled(m_dma, true);
    #endif
}


void LEDStripBase::show()
{
    // Make sure DMA is not running
    dma_channel_wait_for_finish_blocking(m_dma);

    // Call sub-class function top copy pixel data into dma buffer
    copy_buffer();

    // Wait for previous grace period to complete
    sem_acquire_blocking(&m_reset_sem); 

    // Start transfer    
    dma_channel_set_read_addr(m_dma, m_dma_addr, true);
}

