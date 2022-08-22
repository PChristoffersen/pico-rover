#include "displayrender.h"

#include <algorithm>

#include <util/battery.h>
#include <util/time.h>
#include <robot.h>

#include <resources/raspberry_logo.image.h>
#include <resources/battery.image.h>
#include <resources/signal1.image.h>
#include <resources/signal2.image.h>
#include <resources/signal3.image.h>
#include <resources/signal4.image.h>
#include <resources/warning.image.h>
#include <resources/noconnection.image.h>
#include <resources/check.image.h>
#include <resources/cancel.image.h>

#include <liberationmono_16.font.h>
#include <liberationmono_24.font.h>
#include <liberationsans_16.font.h>
#include <liberationsans_24.font.h>
#include <oled/fixed_5x8.font.h>
#include <oled/fixed_8x8.font.h>
#include <oled/fixed_12x16.font.h>
#include <oled/fixed_16x32.font.h>

namespace Image = OLED::Resource::Image;
namespace Font = OLED::Resource::Font;

DisplayRender::DisplayRender(OLED::Display &display, Robot &robot) :
    m_display(display),
    m_framebuffer(display.framebuffer()),
    m_robot { robot }
{

}

void DisplayRender::init()
{
    m_battery_last_level = UINT32_MAX;
    m_battery_show = true;

    if (m_display.present()) {
        m_framebuffer.draw_bitmap((m_framebuffer.width()-Image::Raspberry_Logo.width())/2, (m_framebuffer.height()-Image::Raspberry_Logo.height())/2, Image::Raspberry_Logo);
        m_display.update_blocking();
    }
}


void DisplayRender::begin()
{
    m_framebuffer.clear();
    m_battery_last_update = make_timeout_time_ms(1000);
    m_radio_last_update = delayed_by_us(m_battery_last_update, INTERVAL_STAGGER);
}



void DisplayRender::off()
{
    if (!m_display.present()) 
        return;

    m_framebuffer.clear();
    m_display.update_blocking();
}


absolute_time_t DisplayRender::update_battery()
{
    if (absolute_time_diff_us(m_battery_last_update, get_absolute_time())>BATTERY_INTERVAL) {
        float vbat = m_robot.battery_sensor().get_bus_voltage();
        float vsys = m_robot.sys_sensor().get_vsys();
        float current = m_robot.battery_sensor().get_current();
        uint level = battery_level(vbat);
        bool critical = vbat < battery_critical();

        if (critical || level != m_battery_last_level) {
            // Update battery icon and percent
            constexpr int AREA_LEFT  { 128-Image::Battery.width() };
            constexpr int AREA_TOP   {  0 };
            constexpr int BAR_LEFT   {  7 };
            constexpr int BAR_TOP    { 11 };
            constexpr int BAR_HEIGHT { 30 };

            // Clear
            m_framebuffer.fill_rect(AREA_LEFT, AREA_TOP, Image::Battery.width(),Image::Battery.height()+16, OLED::Framebuffer::DrawOp::SUBTRACT);

            if (critical) 
                m_battery_show = !m_battery_show;
            else
                m_battery_show = true;

            if (m_battery_show) {
                // Icon
                m_framebuffer.draw_bitmap(AREA_LEFT, AREA_TOP, Image::Battery);
                int bar_h = BAR_HEIGHT*level/100;
                m_framebuffer.fill_rect(AREA_LEFT+BAR_LEFT, AREA_TOP+BAR_TOP+BAR_HEIGHT-bar_h, Image::Battery.width()-BAR_LEFT*2, bar_h);

            }

            // Percent
            const OLED::Font &font = Font::LiberationSans_16;
            char text[8];
            sprintf(text, "%u", level);
            m_framebuffer.draw_text(AREA_LEFT+(Image::Battery.width()-font.width(text))/2, AREA_TOP+Image::Battery.height(), text, font);

            m_battery_last_level = level;
        }

        {
            // Update voltage and amps
            const OLED::Font &font = Font::Fixed_8x8;
            const OLED::Font &font2 = Font::LiberationSans_16;
            constexpr int AREA_TOP  { 64-16*3 };
            constexpr int AREA_LEFT { 0 };
            constexpr uint AREA_WIDTH { 128-Image::Battery.width() };
            constexpr uint AREA_HEIGHT { 16*3 };

            // Clear
            m_framebuffer.fill_rect(AREA_LEFT, AREA_TOP, AREA_WIDTH, AREA_HEIGHT, OLED::Framebuffer::DrawOp::SUBTRACT);
            //m_framebuffer.draw_rect(AREA_LEFT, AREA_TOP, AREA_WIDTH, AREA_HEIGHT);

            char text[16];
            int ypos = AREA_TOP;

            sprintf(text, "%.2fV", vbat);
            m_framebuffer.draw_text(AREA_LEFT, ypos, "Batt", font);
            m_framebuffer.draw_text(AREA_LEFT+4*8+10, ypos, text, font2);
            ypos+=16;

            sprintf(text, "%.2fV", vsys);
            m_framebuffer.draw_text(AREA_LEFT, ypos, "Sys", font);
            m_framebuffer.draw_text(AREA_LEFT+4*8+10, ypos, text, font2);
            ypos+=16;

            if (current<0.0f)
                current = 0.0f;
            sprintf(text, "%.2fA", current/1000.0f);
            m_framebuffer.draw_text(AREA_LEFT, ypos, "Power", font);
            m_framebuffer.draw_text(AREA_LEFT+4*8+10, ypos, text, font2);
            ypos+=16;
        }


        m_battery_last_update = delayed_by_us(m_battery_last_update, BATTERY_INTERVAL);
    }

    return delayed_by_us(m_battery_last_update, BATTERY_INTERVAL);
}


absolute_time_t DisplayRender::update_radio()
{
    if (absolute_time_diff_us(m_radio_last_update, get_absolute_time())>RADIO_INTERVAL) {
        const auto &channels = m_robot.receiver_listener().channels();

        constexpr int TEXT_WIDTH  { 12 };
        constexpr int AREA_LEFT   { 128-Image::Battery.width()-20-TEXT_WIDTH };
        constexpr int AREA_TOP    {  0 };
        constexpr int AREA_WIDTH  { 16+TEXT_WIDTH };
        constexpr int AREA_HEIGHT { 16 };
        constexpr int ICON_LEFT  { AREA_LEFT+TEXT_WIDTH };

        m_framebuffer.fill_rect(AREA_LEFT, AREA_TOP, AREA_WIDTH, AREA_HEIGHT, OLED::Framebuffer::DrawOp::SUBTRACT);

        if (!channels.sync()) {
            // Not in sync with receiver module
            m_framebuffer.draw_bitmap(ICON_LEFT, AREA_TOP, Image::Warning);
        }
        else if (channels.flags().frameLost()) {
            // Receiver module lost connection to transmitter
            m_framebuffer.draw_bitmap(ICON_LEFT, AREA_TOP, Image::NoConnection);
        }
        else {
            // All is good
            constexpr auto font { Font::Fixed_8x8 };
            auto rssi = channels.rssi();
            if (rssi>99) 
                rssi = 99;
            char text[4];
            sprintf(text, "%u", rssi);
            m_framebuffer.draw_text(AREA_LEFT, AREA_TOP, text, font);
            if (rssi > 80) {
                m_framebuffer.draw_bitmap(ICON_LEFT, AREA_TOP, Image::Signal4);
            }
            else if (rssi > 60) {
                m_framebuffer.draw_bitmap(ICON_LEFT, AREA_TOP, Image::Signal3);
            }
            else if (rssi > 40) {
                m_framebuffer.draw_bitmap(ICON_LEFT, AREA_TOP, Image::Signal2);
            }
            else {
                m_framebuffer.draw_bitmap(ICON_LEFT, AREA_TOP, Image::Signal1);
            }
        }

        m_radio_last_update = delayed_by_us(m_radio_last_update, RADIO_INTERVAL);
    }

    return delayed_by_us(m_radio_last_update, RADIO_INTERVAL);
}


void DisplayRender::update_armed(bool armed) 
{
    constexpr int AREA_LEFT   {  8 };
    constexpr int AREA_TOP    {  0 };
    constexpr int AREA_WIDTH  { 24 };
    constexpr int AREA_HEIGHT {  8 };

    constexpr auto &font { Font::Fixed_5x8 };

    m_framebuffer.fill_rect(AREA_LEFT, AREA_TOP, AREA_WIDTH, AREA_HEIGHT, OLED::Framebuffer::DrawOp::SUBTRACT);
    if (armed) {
        m_framebuffer.draw_text(AREA_LEFT, AREA_TOP, "ARMED", font);
    }
    else {
    }
}


absolute_time_t DisplayRender::update()
{
    if (!m_display.present())
        return make_timeout_time_ms(60000);

    absolute_time_t wait = update_battery();
    wait = earliest_time(wait, update_radio());
    return wait;
}

