/**
 * @author Peter Christoffersen
 * @brief FrSky protocol definitions 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once


namespace Radio::FrSky {

    /**
     * @brief FrSky telemetry data IDs
     * 
     * From https://github.com/opentx/opentx/blob/2.3/radio/src/telemetry/frsky.h
     */
    // FrSky old DATA IDs (1 byte)
    static constexpr uint8_t FRDID_GPS_ALT_BP_ID             = 0x01;
    static constexpr uint8_t FRDID_TEMP1_ID                  = 0x02;
    static constexpr uint8_t FRDID_RPM_ID                    = 0x03;
    static constexpr uint8_t FRDID_FUEL_ID                   = 0x04;
    static constexpr uint8_t FRDID_TEMP2_ID                  = 0x05;
    static constexpr uint8_t FRDID_VOLTS_ID                  = 0x06;
    static constexpr uint8_t FRDID_GPS_ALT_AP_ID             = 0x09;
    static constexpr uint8_t FRDID_BARO_ALT_BP_ID            = 0x10;
    static constexpr uint8_t FRDID_GPS_SPEED_BP_ID           = 0x11;
    static constexpr uint8_t FRDID_GPS_LONG_BP_ID            = 0x12;
    static constexpr uint8_t FRDID_GPS_LAT_BP_ID             = 0x13;
    static constexpr uint8_t FRDID_GPS_COURS_BP_ID           = 0x14;
    static constexpr uint8_t FRDID_GPS_DAY_MONTH_ID          = 0x15;
    static constexpr uint8_t FRDID_GPS_YEAR_ID               = 0x16;
    static constexpr uint8_t FRDID_GPS_HOUR_MIN_ID           = 0x17;
    static constexpr uint8_t FRDID_GPS_SEC_ID                = 0x18;
    static constexpr uint8_t FRDID_GPS_SPEED_AP_ID           = 0x19;
    static constexpr uint8_t FRDID_GPS_LONG_AP_ID            = 0x1A;
    static constexpr uint8_t FRDID_GPS_LAT_AP_ID             = 0x1B;
    static constexpr uint8_t FRDID_GPS_COURS_AP_ID           = 0x1C;
    static constexpr uint8_t FRDID_BARO_ALT_AP_ID            = 0x21;
    static constexpr uint8_t FRDID_GPS_LONG_EW_ID            = 0x22;
    static constexpr uint8_t FRDID_GPS_LAT_NS_ID             = 0x23;
    static constexpr uint8_t FRDID_ACCEL_X_ID                = 0x24;
    static constexpr uint8_t FRDID_ACCEL_Y_ID                = 0x25;
    static constexpr uint8_t FRDID_ACCEL_Z_ID                = 0x26;
    static constexpr uint8_t FRDID_CURRENT_ID                = 0x28;
    static constexpr uint8_t FRDID_VARIO_ID                  = 0x30;
    static constexpr uint8_t FRDID_VFAS_ID                   = 0x39;
    static constexpr uint8_t FRDID_VOLTS_BP_ID               = 0x3A;
    static constexpr uint8_t FRDID_VOLTS_AP_ID               = 0x3B;
    static constexpr uint8_t FRDID_FRSKY_LAST_ID             = 0x3F;
    static constexpr uint8_t FRDID_D_RSSI_ID                 = 0xF0;
    static constexpr uint8_t FRDID_D_A1_ID                   = 0xF1;
    static constexpr uint8_t FRDID_D_A2_ID                   = 0xF2;

    // FrSky new DATA IDs (2 bytes)
    static constexpr uint16_t FRDID_ALT_FIRST_ID              = 0x0100;
    static constexpr uint16_t FRDID_ALT_LAST_ID               = 0x010F;
    static constexpr uint16_t FRDID_VARIO_FIRST_ID            = 0x0110;
    static constexpr uint16_t FRDID_VARIO_LAST_ID             = 0x011F;
    static constexpr uint16_t FRDID_CURR_FIRST_ID             = 0x0200;
    static constexpr uint16_t FRDID_CURR_LAST_ID              = 0x020F;
    static constexpr uint16_t FRDID_VFAS_FIRST_ID             = 0x0210;
    static constexpr uint16_t FRDID_VFAS_LAST_ID              = 0x021F;
    static constexpr uint16_t FRDID_CELLS_FIRST_ID            = 0x0300;
    static constexpr uint16_t FRDID_CELLS_LAST_ID             = 0x030F;
    static constexpr uint16_t FRDID_T1_FIRST_ID               = 0x0400;
    static constexpr uint16_t FRDID_T1_LAST_ID                = 0x040F;
    static constexpr uint16_t FRDID_T2_FIRST_ID               = 0x0410;
    static constexpr uint16_t FRDID_T2_LAST_ID                = 0x041F;
    static constexpr uint16_t FRDID_RPM_FIRST_ID              = 0x0500;
    static constexpr uint16_t FRDID_RPM_LAST_ID               = 0x050F;
    static constexpr uint16_t FRDID_FUEL_FIRST_ID             = 0x0600;
    static constexpr uint16_t FRDID_FUEL_LAST_ID              = 0x060F;
    static constexpr uint16_t FRDID_ACCX_FIRST_ID             = 0x0700;
    static constexpr uint16_t FRDID_ACCX_LAST_ID              = 0x070F;
    static constexpr uint16_t FRDID_ACCY_FIRST_ID             = 0x0710;
    static constexpr uint16_t FRDID_ACCY_LAST_ID              = 0x071F;
    static constexpr uint16_t FRDID_ACCZ_FIRST_ID             = 0x0720;
    static constexpr uint16_t FRDID_ACCZ_LAST_ID              = 0x072F;
    static constexpr uint16_t FRDID_GPS_LONG_LATI_FIRST_ID    = 0x0800;
    static constexpr uint16_t FRDID_GPS_LONG_LATI_LAST_ID     = 0x080F;
    static constexpr uint16_t FRDID_GPS_ALT_FIRST_ID          = 0x0820;
    static constexpr uint16_t FRDID_GPS_ALT_LAST_ID           = 0x082F;
    static constexpr uint16_t FRDID_GPS_SPEED_FIRST_ID        = 0x0830;
    static constexpr uint16_t FRDID_GPS_SPEED_LAST_ID         = 0x083F;
    static constexpr uint16_t FRDID_GPS_COURS_FIRST_ID        = 0x0840;
    static constexpr uint16_t FRDID_GPS_COURS_LAST_ID         = 0x084F;
    static constexpr uint16_t FRDID_GPS_TIME_DATE_FIRST_ID    = 0x0850;
    static constexpr uint16_t FRDID_GPS_TIME_DATE_LAST_ID     = 0x085F;
    static constexpr uint16_t FRDID_A3_FIRST_ID               = 0x0900;
    static constexpr uint16_t FRDID_A3_LAST_ID                = 0x090F;
    static constexpr uint16_t FRDID_A4_FIRST_ID               = 0x0910;
    static constexpr uint16_t FRDID_A4_LAST_ID                = 0x091F;
    static constexpr uint16_t FRDID_AIR_SPEED_FIRST_ID        = 0x0A00;
    static constexpr uint16_t FRDID_AIR_SPEED_LAST_ID         = 0x0A0F;
    static constexpr uint16_t FRDID_FUEL_QTY_FIRST_ID         = 0x0A10;
    static constexpr uint16_t FRDID_FUEL_QTY_LAST_ID          = 0x0A1F;
    static constexpr uint16_t FRDID_RBOX_BATT1_FIRST_ID       = 0x0B00;
    static constexpr uint16_t FRDID_RBOX_BATT1_LAST_ID        = 0x0B0F;
    static constexpr uint16_t FRDID_RBOX_BATT2_FIRST_ID       = 0x0B10;
    static constexpr uint16_t FRDID_RBOX_BATT2_LAST_ID        = 0x0B1F;
    static constexpr uint16_t FRDID_RBOX_STATE_FIRST_ID       = 0x0B20;
    static constexpr uint16_t FRDID_RBOX_STATE_LAST_ID        = 0x0B2F;
    static constexpr uint16_t FRDID_RBOX_CNSP_FIRST_ID        = 0x0B30;
    static constexpr uint16_t FRDID_RBOX_CNSP_LAST_ID         = 0x0B3F;
    static constexpr uint16_t FRDID_SD1_FIRST_ID              = 0x0B40;
    static constexpr uint16_t FRDID_SD1_LAST_ID               = 0x0B4F;
    static constexpr uint16_t FRDID_ESC_POWER_FIRST_ID        = 0x0B50;
    static constexpr uint16_t FRDID_ESC_POWER_LAST_ID         = 0x0B5F;
    static constexpr uint16_t FRDID_ESC_RPM_CONS_FIRST_ID     = 0x0B60;
    static constexpr uint16_t FRDID_ESC_RPM_CONS_LAST_ID      = 0x0B6F;
    static constexpr uint16_t FRDID_ESC_TEMPERATURE_FIRST_ID  = 0x0B70;
    static constexpr uint16_t FRDID_ESC_TEMPERATURE_LAST_ID   = 0x0B7F;
    static constexpr uint16_t FRDID_RB3040_OUTPUT_FIRST_ID    = 0x0B80;
    static constexpr uint16_t FRDID_RB3040_OUTPUT_LAST_ID     = 0x0B8F;
    static constexpr uint16_t FRDID_RB3040_CH1_2_FIRST_ID     = 0x0B90;
    static constexpr uint16_t FRDID_RB3040_CH1_2_LAST_ID      = 0x0B9F;
    static constexpr uint16_t FRDID_RB3040_CH3_4_FIRST_ID     = 0x0BA0;
    static constexpr uint16_t FRDID_RB3040_CH3_4_LAST_ID      = 0x0BAF;
    static constexpr uint16_t FRDID_RB3040_CH5_6_FIRST_ID     = 0x0BB0;
    static constexpr uint16_t FRDID_RB3040_CH5_6_LAST_ID      = 0x0BBF;
    static constexpr uint16_t FRDID_RB3040_CH7_8_FIRST_ID     = 0x0BC0;
    static constexpr uint16_t FRDID_RB3040_CH7_8_LAST_ID      = 0x0BCF;
    static constexpr uint16_t FRDID_X8R_FIRST_ID              = 0x0C20;
    static constexpr uint16_t FRDID_X8R_LAST_ID               = 0x0C2F;
    static constexpr uint16_t FRDID_S6R_FIRST_ID              = 0x0C30;
    static constexpr uint16_t FRDID_S6R_LAST_ID               = 0x0C3F;
    static constexpr uint16_t FRDID_GASSUIT_TEMP1_FIRST_ID    = 0x0D00;
    static constexpr uint16_t FRDID_GASSUIT_TEMP1_LAST_ID     = 0x0D0F;
    static constexpr uint16_t FRDID_GASSUIT_TEMP2_FIRST_ID    = 0x0D10;
    static constexpr uint16_t FRDID_GASSUIT_TEMP2_LAST_ID     = 0x0D1F;
    static constexpr uint16_t FRDID_GASSUIT_SPEED_FIRST_ID    = 0x0D20;
    static constexpr uint16_t FRDID_GASSUIT_SPEED_LAST_ID     = 0x0D2F;
    static constexpr uint16_t FRDID_GASSUIT_RES_VOL_FIRST_ID  = 0x0D30;
    static constexpr uint16_t FRDID_GASSUIT_RES_VOL_LAST_ID   = 0x0D3F;
    static constexpr uint16_t FRDID_GASSUIT_RES_PERC_FIRST_ID = 0x0D40;
    static constexpr uint16_t FRDID_GASSUIT_RES_PERC_LAST_ID  = 0x0D4F;
    static constexpr uint16_t FRDID_GASSUIT_FLOW_FIRST_ID     = 0x0D50;
    static constexpr uint16_t FRDID_GASSUIT_FLOW_LAST_ID      = 0x0D5F;
    static constexpr uint16_t FRDID_GASSUIT_MAX_FLOW_FIRST_ID = 0x0D60;
    static constexpr uint16_t FRDID_GASSUIT_MAX_FLOW_LAST_ID  = 0x0D6F;
    static constexpr uint16_t FRDID_GASSUIT_AVG_FLOW_FIRST_ID = 0x0D70;
    static constexpr uint16_t FRDID_GASSUIT_AVG_FLOW_LAST_ID  = 0x0D7F;
    static constexpr uint16_t FRDID_SBEC_POWER_FIRST_ID       = 0x0E50;
    static constexpr uint16_t FRDID_SBEC_POWER_LAST_ID        = 0x0E5F;
    static constexpr uint16_t FRDID_DIY_FIRST_ID              = 0x5100;
    static constexpr uint16_t FRDID_DIY_LAST_ID               = 0x52FF;
    static constexpr uint16_t FRDID_DIY_STREAM_FIRST_ID       = 0x5000;
    static constexpr uint16_t FRDID_DIY_STREAM_LAST_ID        = 0x50FF;
    static constexpr uint16_t FRDID_SERVO_FIRST_ID            = 0x6800;
    static constexpr uint16_t FRDID_SERVO_LAST_ID             = 0x680F;
    static constexpr uint16_t FRDID_FACT_TEST_ID              = 0xF000;
    static constexpr uint16_t FRDID_VALID_FRAME_RATE_ID       = 0xF010;
    static constexpr uint16_t FRDID_RSSI_ID                   = 0xF101;
    static constexpr uint16_t FRDID_ADC1_ID                   = 0xF102;
    static constexpr uint16_t FRDID_ADC2_ID                   = 0xF103;
    static constexpr uint16_t FRDID_BATT_ID                   = 0xF104;
    static constexpr uint16_t FRDID_RAS_ID                    = 0xF105;
    static constexpr uint16_t FRDID_XJT_VERSION_ID            = 0xF106;
    static constexpr uint16_t FRDID_R9_PWR_ID                 = 0xF107;
    static constexpr uint16_t FRDID_SP2UART_A_ID              = 0xFD00;
    static constexpr uint16_t FRDID_SP2UART_B_ID              = 0xFD01;


    /**
     * @brief FrSky FBus protocol
     * 
     */

    // Control package
    static constexpr uint8_t FBUS_CONTROL_HDR       = 0xFF;
    static constexpr uint8_t FBUS_CONTROL_8CH_SIZE  = 0x0D;
    static constexpr uint8_t FBUS_CONTROL_16CH_SIZE = 0x18;
    static constexpr uint8_t FBUS_CONTROL_24CH_SIZE = 0x23;
    static constexpr size_t  FBUS_CONTROL_HDR_SIZE  = 2u;

    template <typename buffer_type> 
    static inline uint8_t fbus_control_size(const buffer_type &buffer) 
    { 
        return FBUS_CONTROL_HDR_SIZE+buffer[0]+1; 
    }

    template <typename buffer_type> 
    static inline uint8_t fbus_control_crc(const buffer_type &buffer) 
    { 
        return buffer[buffer[0]+FBUS_CONTROL_HDR_SIZE]; 
    }

    template <typename buffer_type> 
    static inline uint8_t fbus_control_flags(const buffer_type &buffer) 
    { 
        return buffer[buffer[0]+FBUS_CONTROL_HDR_SIZE-2]; 
    }

    template <typename buffer_type> 
    static inline uint8_t fbus_control_rssi(const buffer_type &buffer) 
    { 
        return buffer[buffer[0]+FBUS_CONTROL_HDR_SIZE-1]; 
    }


    // Downlink package
    static constexpr uint8_t FBUS_DOWNLINK_HDR      = 0x08;
    static constexpr size_t  FBUS_DOWNLINK_HDR_SIZE = 1u;
    static constexpr size_t  FBUS_DOWNLINK_SIZE     = (FBUS_DOWNLINK_HDR_SIZE+FBUS_DOWNLINK_HDR+1);

    template <typename buffer_type> 
    static inline uint8_t fbus_downlink_crc(const buffer_type &buffer) 
    { 
        return buffer[buffer[0]+FBUS_DOWNLINK_HDR_SIZE]; 
    }

    typedef struct {
        uint8_t size;
        uint8_t id;
        uint8_t prim;
        uint16_t app_id;
        uint32_t data;
        uint8_t crc;
    } __attribute__((__packed__)) fbus_downlink_t;



    // Uplink package
    static constexpr uint8_t FBUS_UPLINK_HDR        = 0x08;
    static constexpr size_t  FBUS_UPLINK_HDR_SIZE   = 1u;
    static constexpr uint8_t FBUS_UPLINK_DATA_FRAME = 0x10;
    static constexpr size_t  FBUS_UPLINK_SIZE       = (FBUS_UPLINK_HDR_SIZE+FBUS_UPLINK_HDR+1);
    static constexpr int64_t FBUS_UPLINK_SEND_DELAY_MAX_US = 3000ll;
    static constexpr int64_t FBUS_UPLINK_SEND_DELAY_MIN_US = 400ll;

    template <typename buffer_type>
    static inline uint8_t fbus_uplink_crc(const buffer_type &buffer)
    {
        return buffer[buffer[0]+FBUS_UPLINK_HDR_SIZE];
    }

    typedef struct {
        uint8_t size;
        uint8_t id;
        uint8_t prim;
        uint16_t app_id;
        uint32_t data;
        uint8_t crc;
    } __attribute__((__packed__)) fbus_uplink_t;

}
