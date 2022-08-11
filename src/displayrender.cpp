#include "displayrender.h"

#include <algorithm>
#include <util/battery.h>

#include <resources/raspberry_logo.image.h>
#include <resources/battery.image.h>

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

DisplayRender::DisplayRender(OLED::Display &display) :
    m_display(display),
    m_framebuffer(display.framebuffer())
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



absolute_time_t DisplayRender::update_battery(battery_sensor_type &sensor)
{
    if (absolute_time_diff_us(m_battery_last_update, get_absolute_time())>BATTERY_INTERVAL) {
        float voltage = sensor.get_bus_voltage();
        float current = sensor.get_current()/1000.0f;
        uint level = battery_level(voltage);
        bool critical = battery_critical(voltage);

        if (critical || level != m_battery_last_level) {
            // Update battery icon and percent
            constexpr int AREA_LEFT  { 128-Image::Battery.width() };
            constexpr int AREA_TOP   {  0 };
            constexpr int BAR_LEFT   {  7 };
            constexpr int BAR_TOP    { 11 };
            constexpr int BAR_HEIGHT { 30 };

            // Clear
            m_framebuffer.fill_rect(AREA_LEFT, AREA_TOP, Image::Battery.width(),Image::Battery.height(), OLED::Framebuffer::DrawOp::SUBTRACT);

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

            #if 0
            // Percent
            const OLED::Font &font = Font::Fixed_8x8;
            char text[8];
            sprintf(text, "%u%%", level);
            m_framebuffer.draw_text(AREA_LEFT+(Image::Battery.width()-font.width(text))/2, AREA_TOP+Image::Battery.height()+8, text, font);
            #endif

            m_battery_last_level = level;
        }

        {
            // Update voltage and amps
            const OLED::Font &font = Font::LiberationSans_16;
            constexpr int AREA_TOP  { 64-16 };
            constexpr int AREA_LEFT { 32 };
            constexpr uint AREA_WIDTH { 128-AREA_LEFT };
            constexpr uint AREA_HEIGHT { 16 };

            // Clear
            m_framebuffer.fill_rect(AREA_LEFT, AREA_TOP, AREA_WIDTH, AREA_HEIGHT, OLED::Framebuffer::DrawOp::SUBTRACT);
            //m_framebuffer.draw_rect(AREA_LEFT, AREA_TOP, AREA_WIDTH, AREA_HEIGHT);

            char text[16];
            if (current < 10.0f)
                sprintf(text, "%.2fA %.2fV", current, voltage);
            else
                sprintf(text, "%.1fA %.2fV", current, voltage);
            m_framebuffer.draw_text(AREA_LEFT+AREA_WIDTH-font.width(text), AREA_TOP, text, font);
        }


        m_battery_last_update = delayed_by_us(m_battery_last_update, BATTERY_INTERVAL);
    }

    return delayed_by_us(m_battery_last_update, BATTERY_INTERVAL);
}


absolute_time_t DisplayRender::update_radio(radio_receiver_type &receiver)
{
    if (absolute_time_diff_us(m_radio_last_update, get_absolute_time())>RADIO_INTERVAL) {
        
        m_radio_last_update = delayed_by_us(m_radio_last_update, RADIO_INTERVAL);
    }

    return delayed_by_us(m_radio_last_update, RADIO_INTERVAL);
}


