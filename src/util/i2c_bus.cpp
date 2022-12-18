#include "i2c_bus.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/i2c.h>
#include <hardware/dma.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include "boardconfig.h"
#include <util/debug.h>

static constexpr size_t DMA_BUFFER_SIZE = 1024+16;

static constexpr uint I2C_DMA_IRQ          = DMA_IRQ_0;
static constexpr bool I2C_DMA_IRQ_SHARED   = true;
static constexpr uint I2C_DMA_IRQ_PRIORITY = 0;


//static struct semaphore g_bus_sem;
static uint g_bus_dma = 0;
static uint g_bus_dma_irq_index = 0;
static dma_channel_config g_bus_dma_config;
static volatile uint16_t g_bus_dma_buffer[DMA_BUFFER_SIZE];
static size_t g_bus_dma_pos = 0;

static SemaphoreHandle_t g_bus_sem = nullptr;
static StaticSemaphore_t g_bus_sem_buf;

// DMA transfer of pixel data is complete
static void __isr _dma_complete_handler()
{
    if (dma_irqn_get_channel_status(g_bus_dma_irq_index, g_bus_dma)) {
        // Clear the interrupt request.
        dma_irqn_acknowledge_channel(g_bus_dma_irq_index, g_bus_dma);
        hw_set_bits(&i2c_get_hw(i2c_default)->intr_mask, I2C_IC_INTR_MASK_M_TX_EMPTY_BITS); // Unmask TX_EMPTY IRQ
    }
}


static void __isr _i2c_handler()
{
    if (i2c_get_hw(i2c_default)->intr_stat & I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS) {
        i2c_get_hw(i2c_default)->intr_mask = 0u; // Mask all interrupts
        irq_set_enabled(I2C0_IRQ, false);
        xSemaphoreGiveFromISR(g_bus_sem, nullptr);
    }
    if (i2c_get_hw(i2c_default)->intr_stat & I2C_IC_INTR_STAT_R_TX_ABRT_BITS) {
        i2c_get_hw(i2c_default)->clr_tx_abrt;    // Clear abort flag
        i2c_get_hw(i2c_default)->intr_mask = 0u; // Mask all interrupts
        irq_set_enabled(I2C0_IRQ, false);
        xSemaphoreGiveFromISR(g_bus_sem, nullptr);
    }
}


void i2c_bus_init()
{
    g_bus_sem = xSemaphoreCreateBinaryStatic(&g_bus_sem_buf);
    configASSERT(g_bus_sem);
    xSemaphoreGive(g_bus_sem);

    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    i2c_init(i2c_default, BOARD_I2C_SPEED);

    g_bus_dma = dma_claim_unused_channel(true);

    dma_channel_config *config = &g_bus_dma_config;
    *config = dma_channel_get_default_config(g_bus_dma);
    channel_config_set_dreq(config, i2c_get_dreq(i2c_default, true)); 
    channel_config_set_transfer_data_size(config, DMA_SIZE_16);
    channel_config_set_read_increment(config, true);
    channel_config_set_write_increment(config, false);

    dma_channel_configure(g_bus_dma,
                          config,
                          &i2c_get_hw(i2c_default)->data_cmd,
                          NULL,
                          DMA_BUFFER_SIZE,
                          false);


    if constexpr (I2C_DMA_IRQ == DMA_IRQ_0) {
        dma_channel_set_irq0_enabled(g_bus_dma, true);
        g_bus_dma_irq_index = 0;
    }
    else {
        dma_channel_set_irq1_enabled(g_bus_dma, true);
        g_bus_dma_irq_index = 1;
    }

    if constexpr (I2C_DMA_IRQ_SHARED) {
        irq_add_shared_handler(I2C_DMA_IRQ, _dma_complete_handler, I2C_DMA_IRQ_PRIORITY);
    }
    else {
        irq_set_exclusive_handler(I2C_DMA_IRQ, _dma_complete_handler);
    }
    irq_set_enabled(I2C_DMA_IRQ, true);

    irq_set_exclusive_handler(I2C0_IRQ, _i2c_handler);
}


void i2c_bus_acquire_blocking()
{
    auto res __attribute__((unused)) = xSemaphoreTake(g_bus_sem, portMAX_DELAY);
    configASSERT(res==pdTRUE);
}


bool i2c_bus_acquire_timeout_ms(uint32_t timeout_ms)
{
    return xSemaphoreTake(g_bus_sem, pdMS_TO_TICKS(timeout_ms)) == pdTRUE;
}


bool i2c_bus_try_acquire()
{
    return xSemaphoreTake(g_bus_sem, 0) == pdTRUE;
}


void i2c_bus_release()
{
    xSemaphoreGive(g_bus_sem);
}



void i2c_bus_write_dma(uint8_t addr)
{
    if (g_bus_dma_pos==0) {
        i2c_bus_release();
        return;
    }

    // Wait for any previous DMA transfer to complete
    dma_channel_wait_for_finish_blocking(g_bus_dma);

    g_bus_dma_buffer[0] |= (bool_to_bit(i2c_default->restart_on_next) << I2C_IC_DATA_CMD_RESTART_LSB);
    g_bus_dma_buffer[g_bus_dma_pos-1] |= (1 << I2C_IC_DATA_CMD_STOP_LSB);

    i2c_get_hw(i2c_default)->enable = 0;
    i2c_get_hw(i2c_default)->tar = addr;
    i2c_get_hw(i2c_default)->clr_intr; // Clear any interrupts
    i2c_get_hw(i2c_default)->intr_mask = I2C_IC_INTR_MASK_M_TX_ABRT_BITS; // Enable aborted interrupt
    i2c_get_hw(i2c_default)->enable = 1;

    // Enable i2c interrupts and start DMA
    irq_set_enabled(I2C0_IRQ, true);
    dma_channel_transfer_from_buffer_now(g_bus_dma, g_bus_dma_buffer, g_bus_dma_pos);

    g_bus_dma_pos = 0;
}


void i2c_dma_buffer_reset()
{
    g_bus_dma_pos = 0;
}


void i2c_dma_buffer_append(const uint8_t *src, size_t len)
{
    for (size_t i=0; i<len; ++i) {
        g_bus_dma_buffer[g_bus_dma_pos++] = src[i];
    }
}
