#include "lockable.h"

Lockable::Lockable():
    m_sem { nullptr }
{
    m_sem = xSemaphoreCreateMutexStatic(&m_sem_buf);
    assert(m_sem);
    xSemaphoreGive(m_sem);
}