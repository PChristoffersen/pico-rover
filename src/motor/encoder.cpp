#include "encoder.h"

#include <stdio.h>
#include <hardware/pio.h>
#include <hardware/irq.h>
#include <boardconfig.h>

#include "quadrature_encoder.pio.h"

namespace Motor {

PIO  Encoder::m_pio = nullptr;
uint Encoder::m_program_offset = 0;
Encoder::array_type Encoder::m_encoders;
absolute_time_t ABSOLUTE_TIME_INITIALIZED_VAR(Encoder::m_last_update, 0);


void Encoder::global_init() 
{
    static bool initialized = false;
    if (initialized)
        return;

    m_program_offset = pio_add_program(m_pio, &quadrature_encoder_program);

    m_last_update = get_absolute_time();

    initialized = true;
}


Encoder::Encoder(id_type id, PIO pio, uint enca_pin, uint encb_pin, bool invert) :
    m_id { id },
    m_enca_pin { enca_pin },
    m_encb_pin { encb_pin },
    m_invert { invert },
    m_sm { 0 },
    m_value { 0u },
    m_rpm { 0.0f }
{
    assert(m_pio==nullptr || pio==m_pio); // Dont allow different PIO instances on different strips
    if (m_pio==nullptr) {
        m_pio = pio;
    }
    mutex_init(&m_mutex);
}

void Encoder::init()
{
    global_init();

    gpio_init(m_enca_pin);
    gpio_set_dir(m_enca_pin, GPIO_IN);
    gpio_init(m_encb_pin);
    gpio_set_dir(m_encb_pin, GPIO_IN);

    int sm = pio_claim_unused_sm(m_pio, true);
    assert(sm>=0 && sm<static_cast<int>(NUM_PIO_STATE_MACHINES));
    m_sm = sm;
    m_encoders[m_sm] = this;
    quadrature_encoder_program_init(m_pio, m_sm, m_program_offset, m_enca_pin, MAX_STEP_RATE);

    m_value_last = get_absolute_time();

}


inline void Encoder::fetch_request() 
{ 
    quadrature_encoder_request_count(m_pio, m_sm); 
}

inline void Encoder::do_fetch()
{ 
    auto value = quadrature_encoder_fetch_count(m_pio, m_sm); 
    if (m_invert) {
        value = -value;
    }

    auto now = get_absolute_time();
    auto diff_t = absolute_time_diff_us(m_last_update, now);
    auto diff_v = value-m_value;
    auto rpm = m_rpm;

    if (diff_v==0) {
        // Zero RPM        
        rpm = 0.0;
    }
    else if (diff_t>0) {
        rpm = static_cast<double>(diff_v)*60.0*1000000.0 / static_cast<double>(diff_t) / SHAFT_CPR;
        rpm = (rpm+m_rpm)/2.0;
    }

    mutex_enter_blocking(&m_mutex);
    m_rpm = rpm;
    m_value = value;
    mutex_exit(&m_mutex);

    m_callback(m_value, m_rpm);

    m_value_last = now;

    #if 0
    if (m_rpm!=0.0) {
        printf("Diff: %ld %ld / %lld   rpm=%.2f  %.2f    (%.2f)\n", value, diff_v, diff_t, m_rpm, m_rpm/WHEEL_RATIO, rdiff/WHEEL_RATIO);
    }
    #endif

}



absolute_time_t Encoder::update()
{
    if (absolute_time_diff_us(m_last_update, get_absolute_time())>UPDATE_INTERVAL) {
        for (auto &encoder : m_encoders) {
            if (encoder!=nullptr) {
                encoder->fetch_request();
            }
        }
        for (auto &encoder : m_encoders) {
            if (encoder!=nullptr) {
                encoder->do_fetch();
            }
        }
        m_last_update = delayed_by_us(m_last_update, UPDATE_INTERVAL);
    }

    return delayed_by_us(m_last_update, UPDATE_INTERVAL);
}

#if 0
void Encoder::begin()
{
    global_begin();
    pio_set_irqn_source_enabled(m_pio, get_core_num(), static_cast<pio_interrupt_source>(pis_sm0_rx_fifo_not_empty+m_sm), true);

}
#endif


}