/**
 * @author Peter Christoffersen
 * @brief FrSky protocol definitions 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once


/**
 * @brief FrSky telemetry data IDs
 * 
 * From https://github.com/opentx/opentx/blob/2.3/radio/src/telemetry/frsky.h
 */
// FrSky old DATA IDs (1 byte)
#define FRDID_GPS_ALT_BP_ID             0x01
#define FRDID_TEMP1_ID                  0x02
#define FRDID_RPM_ID                    0x03
#define FRDID_FUEL_ID                   0x04
#define FRDID_TEMP2_ID                  0x05
#define FRDID_VOLTS_ID                  0x06
#define FRDID_GPS_ALT_AP_ID             0x09
#define FRDID_BARO_ALT_BP_ID            0x10
#define FRDID_GPS_SPEED_BP_ID           0x11
#define FRDID_GPS_LONG_BP_ID            0x12
#define FRDID_GPS_LAT_BP_ID             0x13
#define FRDID_GPS_COURS_BP_ID           0x14
#define FRDID_GPS_DAY_MONTH_ID          0x15
#define FRDID_GPS_YEAR_ID               0x16
#define FRDID_GPS_HOUR_MIN_ID           0x17
#define FRDID_GPS_SEC_ID                0x18
#define FRDID_GPS_SPEED_AP_ID           0x19
#define FRDID_GPS_LONG_AP_ID            0x1A
#define FRDID_GPS_LAT_AP_ID             0x1B
#define FRDID_GPS_COURS_AP_ID           0x1C
#define FRDID_BARO_ALT_AP_ID            0x21
#define FRDID_GPS_LONG_EW_ID            0x22
#define FRDID_GPS_LAT_NS_ID             0x23
#define FRDID_ACCEL_X_ID                0x24
#define FRDID_ACCEL_Y_ID                0x25
#define FRDID_ACCEL_Z_ID                0x26
#define FRDID_CURRENT_ID                0x28
#define FRDID_VARIO_ID                  0x30
#define FRDID_VFAS_ID                   0x39
#define FRDID_VOLTS_BP_ID               0x3A
#define FRDID_VOLTS_AP_ID               0x3B
#define FRDID_FRSKY_LAST_ID             0x3F
#define FRDID_D_RSSI_ID                 0xF0
#define FRDID_D_A1_ID                   0xF1
#define FRDID_D_A2_ID                   0xF2

// FrSky new DATA IDs (2 bytes)
#define FRDID_ALT_FIRST_ID              0x0100
#define FRDID_ALT_LAST_ID               0x010F
#define FRDID_VARIO_FIRST_ID            0x0110
#define FRDID_VARIO_LAST_ID             0x011F
#define FRDID_CURR_FIRST_ID             0x0200
#define FRDID_CURR_LAST_ID              0x020F
#define FRDID_VFAS_FIRST_ID             0x0210
#define FRDID_VFAS_LAST_ID              0x021F
#define FRDID_CELLS_FIRST_ID            0x0300
#define FRDID_CELLS_LAST_ID             0x030F
#define FRDID_T1_FIRST_ID               0x0400
#define FRDID_T1_LAST_ID                0x040F
#define FRDID_T2_FIRST_ID               0x0410
#define FRDID_T2_LAST_ID                0x041F
#define FRDID_RPM_FIRST_ID              0x0500
#define FRDID_RPM_LAST_ID               0x050F
#define FRDID_FUEL_FIRST_ID             0x0600
#define FRDID_FUEL_LAST_ID              0x060F
#define FRDID_ACCX_FIRST_ID             0x0700
#define FRDID_ACCX_LAST_ID              0x070F
#define FRDID_ACCY_FIRST_ID             0x0710
#define FRDID_ACCY_LAST_ID              0x071F
#define FRDID_ACCZ_FIRST_ID             0x0720
#define FRDID_ACCZ_LAST_ID              0x072F
#define FRDID_GPS_LONG_LATI_FIRST_ID    0x0800
#define FRDID_GPS_LONG_LATI_LAST_ID     0x080F
#define FRDID_GPS_ALT_FIRST_ID          0x0820
#define FRDID_GPS_ALT_LAST_ID           0x082F
#define FRDID_GPS_SPEED_FIRST_ID        0x0830
#define FRDID_GPS_SPEED_LAST_ID         0x083F
#define FRDID_GPS_COURS_FIRST_ID        0x0840
#define FRDID_GPS_COURS_LAST_ID         0x084F
#define FRDID_GPS_TIME_DATE_FIRST_ID    0x0850
#define FRDID_GPS_TIME_DATE_LAST_ID     0x085F
#define FRDID_A3_FIRST_ID               0x0900
#define FRDID_A3_LAST_ID                0x090F
#define FRDID_A4_FIRST_ID               0x0910
#define FRDID_A4_LAST_ID                0x091F
#define FRDID_AIR_SPEED_FIRST_ID        0x0A00
#define FRDID_AIR_SPEED_LAST_ID         0x0A0F
#define FRDID_FUEL_QTY_FIRST_ID         0x0A10
#define FRDID_FUEL_QTY_LAST_ID          0x0A1F
#define FRDID_RBOX_BATT1_FIRST_ID       0x0B00
#define FRDID_RBOX_BATT1_LAST_ID        0x0B0F
#define FRDID_RBOX_BATT2_FIRST_ID       0x0B10
#define FRDID_RBOX_BATT2_LAST_ID        0x0B1F
#define FRDID_RBOX_STATE_FIRST_ID       0x0B20
#define FRDID_RBOX_STATE_LAST_ID        0x0B2F
#define FRDID_RBOX_CNSP_FIRST_ID        0x0B30
#define FRDID_RBOX_CNSP_LAST_ID         0x0B3F
#define FRDID_SD1_FIRST_ID              0x0B40
#define FRDID_SD1_LAST_ID               0x0B4F
#define FRDID_ESC_POWER_FIRST_ID        0x0B50
#define FRDID_ESC_POWER_LAST_ID         0x0B5F
#define FRDID_ESC_RPM_CONS_FIRST_ID     0x0B60
#define FRDID_ESC_RPM_CONS_LAST_ID      0x0B6F
#define FRDID_ESC_TEMPERATURE_FIRST_ID  0x0B70
#define FRDID_ESC_TEMPERATURE_LAST_ID   0x0B7F
#define FRDID_RB3040_OUTPUT_FIRST_ID    0x0B80
#define FRDID_RB3040_OUTPUT_LAST_ID     0x0B8F
#define FRDID_RB3040_CH1_2_FIRST_ID     0x0B90
#define FRDID_RB3040_CH1_2_LAST_ID      0x0B9F
#define FRDID_RB3040_CH3_4_FIRST_ID     0x0BA0
#define FRDID_RB3040_CH3_4_LAST_ID      0x0BAF
#define FRDID_RB3040_CH5_6_FIRST_ID     0x0BB0
#define FRDID_RB3040_CH5_6_LAST_ID      0x0BBF
#define FRDID_RB3040_CH7_8_FIRST_ID     0x0BC0
#define FRDID_RB3040_CH7_8_LAST_ID      0x0BCF
#define FRDID_X8R_FIRST_ID              0x0C20
#define FRDID_X8R_LAST_ID               0x0C2F
#define FRDID_S6R_FIRST_ID              0x0C30
#define FRDID_S6R_LAST_ID               0x0C3F
#define FRDID_GASSUIT_TEMP1_FIRST_ID    0x0D00
#define FRDID_GASSUIT_TEMP1_LAST_ID     0x0D0F
#define FRDID_GASSUIT_TEMP2_FIRST_ID    0x0D10
#define FRDID_GASSUIT_TEMP2_LAST_ID     0x0D1F
#define FRDID_GASSUIT_SPEED_FIRST_ID    0x0D20
#define FRDID_GASSUIT_SPEED_LAST_ID     0x0D2F
#define FRDID_GASSUIT_RES_VOL_FIRST_ID  0x0D30
#define FRDID_GASSUIT_RES_VOL_LAST_ID   0x0D3F
#define FRDID_GASSUIT_RES_PERC_FIRST_ID 0x0D40
#define FRDID_GASSUIT_RES_PERC_LAST_ID  0x0D4F
#define FRDID_GASSUIT_FLOW_FIRST_ID     0x0D50
#define FRDID_GASSUIT_FLOW_LAST_ID      0x0D5F
#define FRDID_GASSUIT_MAX_FLOW_FIRST_ID 0x0D60
#define FRDID_GASSUIT_MAX_FLOW_LAST_ID  0x0D6F
#define FRDID_GASSUIT_AVG_FLOW_FIRST_ID 0x0D70
#define FRDID_GASSUIT_AVG_FLOW_LAST_ID  0x0D7F
#define FRDID_SBEC_POWER_FIRST_ID       0x0E50
#define FRDID_SBEC_POWER_LAST_ID        0x0E5F
#define FRDID_DIY_FIRST_ID              0x5100
#define FRDID_DIY_LAST_ID               0x52FF
#define FRDID_DIY_STREAM_FIRST_ID       0x5000
#define FRDID_DIY_STREAM_LAST_ID        0x50FF
#define FRDID_SERVO_FIRST_ID            0x6800
#define FRDID_SERVO_LAST_ID             0x680F
#define FRDID_FACT_TEST_ID              0xF000
#define FRDID_VALID_FRAME_RATE_ID       0xF010
#define FRDID_RSSI_ID                   0xF101
#define FRDID_ADC1_ID                   0xF102
#define FRDID_ADC2_ID                   0xF103
#define FRDID_BATT_ID                   0xF104
#define FRDID_RAS_ID                    0xF105
#define FRDID_XJT_VERSION_ID            0xF106
#define FRDID_R9_PWR_ID                 0xF107
#define FRDID_SP2UART_A_ID              0xFD00
#define FRDID_SP2UART_B_ID              0xFD01


/**
 * @brief FrSky FBus protocol
 * 
 */
#define FBUS_CONTROL_HDR       0xFF
#define FBUS_CONTROL_8CH_SIZE  0x0D
#define FBUS_CONTROL_16CH_SIZE 0x18
#define FBUS_CONTROL_24CH_SIZE 0x23
#define FBUS_CONTROL_HDR_SIZE 2u
#define FBUS_CONTROL_SIZE(BUF) (FBUS_CONTROL_HDR_SIZE+BUF[0]+1)
#define FBUS_CONTROL_CRC(BUF) (BUF[BUF[0]+FBUS_CONTROL_HDR_SIZE])
#define FBUS_CONTROL_FLAGS(BUF) (BUF[BUF[0]+FBUS_CONTROL_HDR_SIZE-2])
#define FBUS_CONTROL_RSSI(BUF) (BUF[BUF[0]+FBUS_CONTROL_HDR_SIZE-1])

#define FBUS_DOWNLINK_HDR 0x08 
#define FBUS_DOWNLINK_HDR_SIZE 1u
#define FBUS_DOWNLINK_SIZE (FBUS_DOWNLINK_HDR_SIZE+FBUS_DOWNLINK_HDR+1)
#define FBUS_DOWNLINK_CRC(BUF) (BUF[BUF[0]+1])

#define FBUS_UPLINK_HDR 0x08
#define FBUS_UPLINK_HDR_SIZE 1u
#define FBUS_UPLINK_DATA_FRAME 0x10
#define FBUS_UPLINK_SIZE (FBUS_UPLINK_HDR_SIZE+FBUS_UPLINK_HDR+1)
#define FBUS_UPLINK_CRC(BUF) (BUF[BUF[0]+1])
#define FBUS_UPLINK_SEND_DELAY_MAX_US 3000u
#define FBUS_UPLINK_SEND_DELAY_MIN_US 400u


typedef struct {
    uint8_t size;
    uint8_t id;
    uint8_t prim;
    uint16_t app_id;
    uint32_t data;
    uint8_t crc;
} __attribute__((__packed__)) fbus_downlink_t;


typedef struct {
    uint8_t size;
    uint8_t id;
    uint8_t prim;
    uint16_t app_id;
    uint32_t data;
    uint8_t crc;
} __attribute__((__packed__)) fbus_uplink_t;




