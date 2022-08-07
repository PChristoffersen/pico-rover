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
            using addr_t = uint8_t;

            enum class Address: addr_t {
                DISPLAY0 = 0x3C,
                DISPLAY1 = 0x3D,
            };
            enum class Type {
                SSD1306_128x64,
                SSD1306_128x32,
            };

            Display(Address address, Type type);

            void init();
            bool present() { return m_present; }

            void update();

            Framebuffer &frame() { return m_framebuffer; }

        private:
            addr_t m_address;
            bool m_present;

            Framebuffer m_framebuffer;

            void send_cmds(const uint8_t *cmds, uint len);
            void send_data_sync();
            void send_data_async();
    };

}
