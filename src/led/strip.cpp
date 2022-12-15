#include "strip.h"

#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>
#include <pico/sem.h>
#include <hardware/pio.h>
#include <hardware/dma.h>
#include <hardware/irq.h>

#include "ws2812.pio.h"


namespace LED {

static constexpr bool LED_STRIP_DMA_IRQ_SHARED   = true;
static constexpr uint LED_STRIP_DMA_IRQ_PRIORITY = 1;

PIO StripBase::m_pio = nullptr;
StripBase *StripBase::m_strips[NUM_DMA_CHANNELS];
uint StripBase::m_program_offset = 0;
uint StripBase::m_dma_irq_index = 0;



// DMA transfer of pixel data is complete
void __isr StripBase::dma_complete_handler()
{
    for (auto strip : m_strips) {
        if (strip && dma_irqn_get_channel_status(m_dma_irq_index, strip->m_dma)) {
            dma_irqn_acknowledge_channel(m_dma_irq_index, strip->m_dma);

            //printf("LED DmaComplete\n");

            assert(strip->m_reset_alarm==0);
            if (strip->m_reset_alarm) {
                cancel_alarm(strip->m_reset_alarm);
            }
            // Alarm callback triggered when the reset period of the LED strip has passed, and it is safe to start sending pixel data again
            strip->m_reset_alarm = add_alarm_in_us(STRIP_RESET_DELAY_US, +[](alarm_id_t id, void *user_data) -> int64_t {
                StripBase *strip = (StripBase*)user_data;
                strip->m_reset_alarm = 0;
                xSemaphoreGiveFromISR(strip->m_reset_sem, nullptr);
                return 0;
            }, strip, true);
            if (strip->m_reset_alarm<0) {
                // Fore some reason we did not get an alarm so we just release the semaphore and hope for the best
                strip->m_reset_alarm = 0;
                xSemaphoreGiveFromISR(strip->m_reset_sem, nullptr);
                assert(false);
            }
        }
    }
}


StripBase::StripBase(PIO pio, uint pin, bool is_rgbw):
    m_pin { pin },
    m_is_rgbw { is_rgbw },
    m_correction { Color::Correction::TypicalLEDStrip },
    m_brightness { Color::ColorBase::BRIGHTNESS_DEFAULT }
{
    assert(m_pio==nullptr || pio==m_pio); // Dont allow different PIO instances on different strips
    if (m_pio==nullptr) {
        m_pio = pio;
    }
}



void StripBase::global_init()
{
    static bool initialized = false;
    if (initialized)
        return;

    for (auto &strip : m_strips) {
        strip = nullptr;
    }

    // Setup PIO program
    m_program_offset = pio_add_program(m_pio, &ws2812_program);

    m_dma_irq_index = get_core_num();
    uint dma_irq;
    if (m_dma_irq_index==0) {
        dma_irq = DMA_IRQ_0;
    }
    else {
        dma_irq = DMA_IRQ_1;
    }

    if (LED_STRIP_DMA_IRQ_SHARED) {
        irq_add_shared_handler(dma_irq, dma_complete_handler, LED_STRIP_DMA_IRQ_PRIORITY);
    }
    else {
        irq_set_exclusive_handler(dma_irq, dma_complete_handler);
    }
    irq_set_enabled(dma_irq, true);

    initialized = true;
}


void StripBase::base_init(volatile void *dma_addr, size_t dma_count)
{
    global_init();

    gpio_init(m_pin);
    gpio_set_dir(m_pin, GPIO_OUT);
    gpio_put(m_pin, false);

    // Setup PIO program
    m_sm = pio_claim_unused_sm(m_pio, true);
    ws2812_program_init(m_pio, m_sm, m_program_offset, m_pin, STRIP_FREQUENCY, m_is_rgbw);

    // Initialize reset delay semaphore
    m_reset_alarm = 0;
    m_reset_sem = xSemaphoreCreateBinaryStatic(&m_reset_sem_buf);
    xSemaphoreGive(m_reset_sem);

    // Setup DMA
    m_dma = dma_claim_unused_channel(true);

    dma_channel_config config = dma_channel_get_default_config(m_dma);
    channel_config_set_dreq(&config, pio_get_dreq(m_pio, m_sm, true)); 
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
    channel_config_set_read_increment(&config, true); // We want DMA reading to PIO
    channel_config_set_write_increment(&config, false); // We don't want DMA writing from PIO

    dma_channel_configure(m_dma,
                          &config,
                          &m_pio->txf[m_sm],
                          dma_addr,
                          dma_count,
                          false);

    m_strips[m_dma] = this;

    if (get_core_num()==0) {
        dma_channel_set_irq0_enabled(m_dma, true);
    }
    else {
        dma_channel_set_irq1_enabled(m_dma, true);
    }

}



}