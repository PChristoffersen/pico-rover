/**
 * @author Peter Christoffersen
 * @brief Simple ringbuffer
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <pico/stdlib.h>
#include <pico/critical_section.h>

namespace Radio::FBus2 {

    template<typename T, size_t SIZE>
    class RingBuffer {
        public:
            using value_type = T;


            RingBuffer() :
                m_head { 0 },
                m_tail { 0 }
            {
                static_assert(SIZE>2, "Buffer size must be larger than 2");
                static_assert((SIZE & MASK)==0u, "Buffer size must be 2^n");
                critical_section_init(&m_lock);
            }

            void push(value_type v) 
            {
                if (size()+1>=SIZE) {
                    // Buffer full
                    return;
                }
                m_data[m_tail] = v;
                m_tail = (m_tail+1)&MASK;
            }

            void push(value_type *data, size_t len)
            {
                for (size_t i=0; i<len; ++i) {
                    push(data[i]);
                }
            }

            value_type pop() 
            {
                assert(!empty());
                size_t h = m_head;
                pop(1);
                return m_data[h];
            }

            void pop(size_t len)
            {
                assert(size()>=len);
                if (m_head==m_tail) {
                    return;
                }
                if (len>size()) {
                    m_head = m_tail;
                    return;
                }
                m_head = (m_head+len)&MASK;
            }
            void pop_blocking(size_t len) 
            { 
                critical_section_enter_blocking(&m_lock);
                pop(len); 
                critical_section_exit(&m_lock);
            }


            void clear() { m_head = m_tail; }

            size_t size() const          { return (m_tail-m_head) & MASK; }
            size_t size_blocking() const 
            { 
                critical_section_enter_blocking(&m_lock);
                auto sz = size(); 
                critical_section_exit(&m_lock);
                return sz;
            }
            bool empty() const           { return m_tail == m_head; }
            bool empty_blocking() const  
            { 
                critical_section_enter_blocking(&m_lock);
                bool res = empty(); 
                critical_section_exit(&m_lock);
                return res;
            }

            volatile value_type &head() { return m_data[m_head]; }
            volatile value_type &operator[](size_t pos)             { assert(pos<size()); return m_data[(m_head+pos)&MASK]; }
            const volatile value_type &operator[](size_t pos) const { assert(pos<size()); return m_data[(m_head+pos)&MASK]; }
            
            void copy(value_type *dst, size_t len, size_t off)
            {
                assert(off+len <= size());
                for (size_t i=0; i<len; i++) {
                    dst[i] = m_data[(m_head+off+i)&MASK];
                }
            }


            critical_section_t &lock() { return m_lock; }
            void enter_blocking()      { critical_section_enter_blocking(&m_lock); }
            void exit()                { critical_section_exit(&m_lock); }

        private:
            static constexpr size_t MASK { SIZE-1 };

            mutable critical_section_t m_lock;

            volatile value_type m_data[SIZE];
            volatile size_t m_head;
            volatile size_t m_tail;
    };

}
