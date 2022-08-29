/**
 * @author Peter Christoffersen
 * @brief OLED 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>
#include <pico/stdlib.h>

#include "framebuffer.h"

namespace OLED {

    class Display {
        public:
            using addr_type = uint8_t;

            enum class Address: addr_type {
                DISPLAY0 = 0x3C,
                DISPLAY1 = 0x3D,
            };
            enum class Type {
                SSD1306_128x64,
                SSD1306_128x32,
            };

            Display(Address address, Type type);
            Display(const Display&) = delete; // No copy constructor
            Display(Display&&) = delete; // No move constructor

            void init();
            bool present() { return m_present; }

            void update_blocking();

            bool update_needed() const { return m_present && m_framebuffer.is_dirty(); }

            Framebuffer &framebuffer() { return m_framebuffer; }

        private:
            const addr_type m_address;
            bool m_present;

            Framebuffer m_framebuffer;

            void send_cmds(const uint8_t *cmds, uint len);
            void send_data_sync();
            void send_data_async();
    };

}
