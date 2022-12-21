/**
 * @author Peter Christoffersen
 * @brief FrSky protocol definitions 
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 * The only documentation of the protocol I have been able to find is:
 * 
 * https://github.com/betaflight/betaflight/wiki/The-FrSky-FPort-Protocol
 * 
 * The PDF on that page does not represent precicely what is sent, but by
 * reverse engineering dumped data from a receiver I have been able to 
 * implement a working protocol.
 * 
 */
#pragma once


namespace FBus2 {

    /**
     * @brief Control package
     * 
     * |<SIZE>|<ID>|<CHANNEL DATA>|<RSSI>|<FLAGS>|<CRC>|
     * 
     * SIZE: size of the package
     * ID: Always 0xFF
     * CHANNEL DATA: 11 bits per channel (8,16,or 24 channels) depending on SIZE
     * RSSI: Signal strength
     * FLAGS: Connection flags
     * CRC: Checksum of package
     * 
     */
    // Control package
    static constexpr uint8_t FBUS_CONTROL_HDR       = 0xFF;
    static constexpr uint8_t FBUS_CONTROL_8CH_SIZE  = 0x0D;
    static constexpr uint8_t FBUS_CONTROL_16CH_SIZE = 0x18;
    static constexpr uint8_t FBUS_CONTROL_24CH_SIZE = 0x23;
    static constexpr size_t  FBUS_CONTROL_HDR_SIZE  = 2u;
    static constexpr size_t  FBUS_CONTROL_8_VALUE_SIZE = 11u;

    typedef struct {
        uint8_t size;
        uint8_t id;
        uint8_t channels[FBUS_CONTROL_8_VALUE_SIZE];
        uint8_t flags;
        uint8_t rssi;
        uint8_t crc;
    } __attribute__((__packed__)) fbus_control_8_t;

    typedef struct {
        uint8_t size;
        uint8_t id;
        uint8_t channels[FBUS_CONTROL_8_VALUE_SIZE*2];
        uint8_t flags;
        uint8_t rssi;
        uint8_t crc;
    } __attribute__((__packed__)) fbus_control_16_t;

    typedef struct {
        uint8_t size;
        uint8_t id;
        uint8_t channels[FBUS_CONTROL_8_VALUE_SIZE*3];
        uint8_t flags;
        uint8_t rssi;
        uint8_t crc;
    } __attribute__((__packed__)) fbus_control_24_t;

    
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
    static constexpr int64_t FBUS_UPLINK_SEND_DELAY_MAX_US = 2500ll;
    static constexpr int64_t FBUS_UPLINK_SEND_TIMEOUT_US   = 3200ll;
    static constexpr int64_t FBUS_UPLINK_POST_DELAY_US     = 5000ll; // 4 ms

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




    /**
     * @brief Calculate checksum of buffer
     * 
     * @tparam buffer_type Type of buffer, must have [] operator
     * @param buffer Buffer to calculate checksum
     * @param off Offset into the buffer to start
     * @param sz Size of data
     * @return uint8_t Checksum value
     */
    template <typename buffer_type> 
    uint8_t fbus_checksum(const buffer_type &buffer, size_t off, size_t sz)
    {
        uint16_t sum = 0x00;
        for (uint i=0; i<sz; i++) {
            sum += buffer[off+i];
        }
        while (sum > 0xFF) {
            sum = (sum & 0xFF) + (sum >>8);
        }
        return 0xFF - sum;
    }


    /**
     * @brief Extracts 8 channels (11 bit per channel) from fbus package
     * 
     * @tparam source_buffer_type Type of source buffer
     * @tparam dest_buffer_type Type of destination buffer
     * @param src Source buffer
     * @param src_offset Offset into source buffer
     * @param dst Destination buffer
     * @param dst_offset Offset into destination
     */
    template <typename value_type, typename source_buffer_type, typename dest_buffer_type>
    void fbus_get_8channel(const source_buffer_type &src, size_t src_offset, dest_buffer_type &dst, size_t dst_offset)
    {
        dst[dst_offset  ] = (static_cast<value_type>(src[src_offset+0]))           + ((0x07 & src[src_offset+1])<<8);
        dst[dst_offset+1] = (static_cast<value_type>(0xF8 & src[src_offset+1])>>3) + ((0x3F & src[src_offset+2])<<5);
        dst[dst_offset+2] = (static_cast<value_type>(0xC0 & src[src_offset+2])>>6) + (src[src_offset+3]<<2) + ((0x01 & src[src_offset+4])<<10);
        dst[dst_offset+3] = (static_cast<value_type>(0xFE & src[src_offset+4])>>1) + ((0x0F & src[src_offset+5])<<7);
        dst[dst_offset+4] = (static_cast<value_type>(0xF0 & src[src_offset+5])>>4) + ((0x7F & src[src_offset+6])<<4);
        dst[dst_offset+5] = (static_cast<value_type>(0x80 & src[src_offset+6])>>7) + (src[src_offset+7]<<1) + ((0x03 & src[src_offset+8])<<9);
        dst[dst_offset+6] = (static_cast<value_type>(0xFC & src[src_offset+8])>>2) + ((0x1F & src[src_offset+9])<<6);
        dst[dst_offset+7] = (static_cast<value_type>(0xF0 & src[src_offset+9])>>5) + ((src[src_offset+10])<<3);
    }





}
