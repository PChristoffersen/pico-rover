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
        }

        void push(value_type v) 
        {
            if (size()+1>=SIZE) {
                m_head = (m_tail+2)&MASK;
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
            if (m_head==m_tail) {
                return;
            }
            if (len>size()) {
                m_head = m_tail;
                return;
            }
            m_head = (m_head+len)&MASK;
        }

        void clear() 
        {
            m_head = m_tail;
        }

        size_t size() const
        {
            return (m_tail-m_head) & MASK;
        }

        bool empty() const 
        {
            return m_tail == m_head;
        }

        value_type &head() { return m_data[m_head]; }


        value_type &operator[](size_t pos) 
        { 
            assert(pos<size());
            return m_data[(m_head+pos)&MASK]; 
        }
        const value_type &operator[](size_t pos) const 
        { 
            assert(pos<size());
            return m_data[(m_head+pos)&MASK]; 
        }

        
        void copy(value_type *dst, size_t len, size_t off)
        {
            assert(off+len <= size());
            for (size_t i=0; i<len; i++) {
                dst[i] = m_data[(m_head+off+i)&MASK];
            }
        }

        void printc()
        {
            printf("   ");
            for (size_t i=0; i<size(); ++i) {
                printf("%02x ", (int)(*this)[i]);
            }
            printf("\n");
        }
        void print(const char *reason) {
            printf("%s:  head=%-3u tail=%-3u size=%-3u  ", reason, m_head, m_tail, size());
            printc();
        }

    private:
        static constexpr size_t MASK { SIZE-1 };

        value_type m_data[SIZE];
        size_t m_head;
        size_t m_tail;
};
