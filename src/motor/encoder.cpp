#include "encoder.h"

#include <hardware/pio.h>
#include <boardconfig.h>

namespace Motor {

void Encoder::global_init() 
{
    static bool initialized = false;
    if (initialized)
        return;

    m_program_offset = pio_add_program(m_pio, &quadrature_encoder_program);

    initialized = true;
}

Encoder::Encoder(PIO pio, uint enca_pin, uint encb_pin) :
    m_enca_pin { enca_pin },
    m_encb_pin { encb_pin }
{
}

void Encoder::init()
{
    global_init();

    gpio_init(m_enca_pin);
    gpio_set_dir(m_enca_pin, GPIO_IN);
    gpio_init(m_encb_pin);
    gpio_set_dir(m_encb_pin, GPIO_IN);

    m_sm = pio_claim_unused_sm(m_pio, true);
    quadrature_encoder_program_init(m_pio, m_sm, m_program_offset, m_enca_pin, MAX_STEP_RATE);

}

}