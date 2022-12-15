#include "color.h"

namespace LED::Color {

RGBW &RGBW::operator<<(const RGBA &src_color) {
    raw_type src_a = src_color.alpha();
    auto src = src_color.rgb();

    if (src_a==CHANNEL_MIN) {
        return *this;
    }
    if (src_a==CHANNEL_MAX) {
        m_data = src;
        return *this;
    }


    raw_type dst_r   = ( (m_data & RED_MASK)   * (CHANNEL_MAX-src_a+1) / CHANNEL_MAX  +  (src & RED_MASK)   * src_a / CHANNEL_MAX ) & RED_MASK;
    raw_type dst_g = ( (m_data & GREEN_MASK) * (CHANNEL_MAX-src_a+1) / CHANNEL_MAX  +  (src & GREEN_MASK) * src_a / CHANNEL_MAX ) & GREEN_MASK;
    raw_type dst_b  = ( (m_data & BLUE_MASK)  * (CHANNEL_MAX-src_a+1) / CHANNEL_MAX  +  (src & BLUE_MASK)  * src_a / CHANNEL_MAX ) & BLUE_MASK;
    //raw_type dst_a = alpha() * (CHANNEL_MAX-src_a) + src_a;
    raw_type dst_w = CHANNEL_MIN << WHITE_SHIFT;

    m_data = dst_w | dst_r | dst_g | dst_b;

    return *this;
}

}