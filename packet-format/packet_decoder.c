#include <packet_header.h>
#include <stddef.h>
#include <packet_decoder.h>
#include <string.h>

/**
* The next few macros are useful for making unique names
* for variables, usually useful for use within a macro.
* NOTE: this will break if you use it more than once per line!
*/
#define PP_CAT(a, b) PP_CAT_I(a, b)
#define PP_CAT_I(a, b) PP_CAT_II(~, a ## b)
#define PP_CAT_II(p, res) res

#define UNIQUE_NAME(base) PP_CAT(base, __LINE__)

/**
* Generic read function that will convert between network and platform byte order.
* @param dst destination buffer to read data into.
* @param data_ptr source buffer to read bytes from
* @param size size of data to read
* @param bytes_remaining number of bytes available in data_ptr
*/
void read_data(void* dst, uint8_t** data_ptr, size_t size, size_t* bytes_remaining) {
    if (size > *bytes_remaining) {
        return;
    }
    memcpy(dst, *data_ptr, size);
    *data_ptr += size;
    *bytes_remaining -= size;
}


/**
* Generic write function that will convert between network and platform byte order.
* @param data_ptr source buffer to write bytes to
* @param src pointer to platform specific variable to read data from
* @param size size of data to write to data_ptr
* @param bytes_remaining number of bytes available in data_ptr
*/
void write_data(uint8_t** data_ptr, void* src, size_t size, size_t* bytes_remaining) {
    if (size > *bytes_remaining) {
        return;
    }
    memcpy(*data_ptr, src, size);
    *data_ptr += size;
    *bytes_remaining -= size;
}




#define READ_TYPED(c_type, packet_type, dst, src, bytes_remaining)  read_data(&(dst), src, sizeof(c_type), bytes_remaining)
#define WRITE_TYPED(c_type,packet_type, dst, src, bytes_remaining) write_data(dst, &(src), sizeof(c_type), bytes_remaining)

#define READ_ENUM(c_type, packet_type, dst, src, bytes_remaining) \
    uint64_t UNIQUE_NAME(scratch) = 0; memcpy(&UNIQUE_NAME(scratch), *(src), sizeof(packet_type)); dst = (c_type) UNIQUE_NAME(scratch); *(src) += sizeof(packet_type)


#define WRITE_ENUM(c_type, packet_type, dst, src, bytes_remaining) \
    packet_type UNIQUE_NAME(scratch) = (packet_type) src; memcpy(dst, &UNIQUE_NAME(scratch), sizeof(packet_type));

/**
 *  Decode packet from raw network order (LE) packet
 * @param pointer to union representing all possible packets
 * @param data raw bytes received from 'network'
 * @param size number of bytes of received packet
 * @return
 */
PacketParserReturnType
packet_decode_from_byte_array(union ConcentratorPacket* packet,
                              uint8_t *data,
                              size_t size
                              ) {
    uint8_t* data_ptr = data;
    size_t bytes_remaining = size;
    

    READ_TYPED(uint8_t, uint8_t, packet->packet_header.source_address, &data_ptr, &bytes_remaining);
    READ_ENUM(enum PacketType, uint8_t, packet->packet_header.packet_type, &data_ptr, &bytes_remaining);
    READ_TYPED(uint8_t, uint8_t, packet->packet_header.size, &data_ptr, &bytes_remaining);
    switch(packet->packet_header.packet_type) {
        case 0:
            break;
        case 1:
            READ_TYPED(uint16_t, uint16_t, packet->adc_sensor_packet.adc_value, &data_ptr, &bytes_remaining);
            break;
        case 2:
            READ_TYPED(uint16_t, uint16_t, packet->dual_mode_sensor_packet.adc_value, &data_ptr, &bytes_remaining);
            READ_TYPED(uint16_t, uint16_t, packet->dual_mode_sensor_packet.battery_percentage, &data_ptr, &bytes_remaining);
            READ_TYPED(uint32_t, uint32_t, packet->dual_mode_sensor_packet.time, &data_ptr, &bytes_remaining);
            READ_TYPED(uint8_t, uint8_t, packet->dual_mode_sensor_packet.button, &data_ptr, &bytes_remaining);
            READ_TYPED(bool, bool, packet->dual_mode_sensor_packet.conc_led_toggle, &data_ptr, &bytes_remaining);
            break;
        }


}


/**
 *  Encode packet from local C representation into network order (LE) packet.
 * @param pointer to union representing packet to encode
 * @param pointer to buffer to receive encoded packet
 * @param size of buffer (max size available)
 * @return
 */
PacketParserReturnType
packet_encode_to_byte_array(uint8_t *data,
                            union ConcentratorPacket* packet,
                            size_t size
                            ) {
    uint8_t* data_ptr = data;
    size_t bytes_remaining = size;
    

    WRITE_TYPED(uint8_t, uint8_t, &data_ptr, packet->packet_header.source_address, &bytes_remaining);
    WRITE_ENUM(enum PacketType, uint8_t, &data_ptr, packet->packet_header.packet_type, &bytes_remaining);
    WRITE_TYPED(uint8_t, uint8_t, &data_ptr, packet->packet_header.size, &bytes_remaining);
    switch(packet->packet_header.packet_type) {
        case 0:
            break;
        case 1:
            WRITE_TYPED(uint16_t, uint16_t, &data_ptr, packet->adc_sensor_packet.adc_value, &bytes_remaining);
            break;
        case 2:
            WRITE_TYPED(uint16_t, uint16_t, &data_ptr, packet->dual_mode_sensor_packet.adc_value, &bytes_remaining);
            WRITE_TYPED(uint16_t, uint16_t, &data_ptr, packet->dual_mode_sensor_packet.battery_percentage, &bytes_remaining);
            WRITE_TYPED(uint32_t, uint32_t, &data_ptr, packet->dual_mode_sensor_packet.time, &bytes_remaining);
            WRITE_TYPED(uint8_t, uint8_t, &data_ptr, packet->dual_mode_sensor_packet.button, &bytes_remaining);
            WRITE_TYPED(bool, bool, &data_ptr, packet->dual_mode_sensor_packet.conc_led_toggle, &bytes_remaining);
            break;
        }

}



