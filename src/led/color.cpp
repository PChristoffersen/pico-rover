#include "color.h"

namespace LED::Color {

void RGBW::operator<<(const RGBA &src_color) {
    raw_type src_a = src_color.alpha();
    auto src = src_color.rgb();

    if (src_a==CHANNEL_MIN) {
        return;
    }
    if (src_a==CHANNEL_MAX) {
        m_data = src;
        return;
    }

    raw_type dst_r = std::clamp<raw_type>( (m_data & RED_MASK)   * (CHANNEL_MAX-src_a+1) / CHANNEL_MAX  +  (src & RED_MASK)   * src_a / CHANNEL_MAX, 0, RED_MASK) & RED_MASK;
    raw_type dst_g = std::clamp<raw_type>( (m_data & GREEN_MASK) * (CHANNEL_MAX-src_a+1) / CHANNEL_MAX  +  (src & GREEN_MASK) * src_a / CHANNEL_MAX, 0, GREEN_MASK) & GREEN_MASK;
    raw_type dst_b = std::clamp<raw_type>( (m_data & BLUE_MASK)  * (CHANNEL_MAX-src_a+1) / CHANNEL_MAX  +  (src & BLUE_MASK)  * src_a / CHANNEL_MAX, 0, BLUE_MASK) & BLUE_MASK;
    //raw_type dst_a = alpha() * (CHANNEL_MAX-src_a) + src_a;
    raw_type dst_w = CHANNEL_MIN << WHITE_SHIFT;

    m_data = dst_w | dst_r | dst_g | dst_b;
}

}