#include "colorlayer.h"

namespace LED::Color {

LayerBase::LayerBase(size_t offset) : 
    m_dirty { false }, 
    m_visible { false },
    m_offset { offset }
{
}


}