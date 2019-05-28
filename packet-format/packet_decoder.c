#include <packet_header.h>
#include <stddef.h>
#include <packet_decoder.h>
#include <string.h>


void read_data(void* dst, uint8_t** data_ptr, size_t size) {
    memcpy(dst, *data_ptr, size);
    *data_ptr += size;
}

void write_data(uint8_t** data_ptr, void* dst, size_t size) {
    memcpy(dst, *data_ptr, size);
    *data_ptr += size;
}

#define read_uint8_t(dst, src)   read_data(dst, src, sizeof(uint8_t))
#define write_uint8_t(dst, src)  write_data(dst, src, sizeof(uint8_t))
#define read_uint16_t(dst, src)  read_data(dst, src, sizeof(uint16_t))
#define write_uint16_t(dst, src) write_data(dst, src, sizeof(uint16_t))
#define read_uint32_t(dst, src)  read_data(dst, src, sizeof(uint32_t))
#define write_uint32_t(dst, src) write_data(dst, src, sizeof(uint32_t))
#define read_uint64_t(dst, src)  read_data(dst, src, sizeof(uint64_t))
#define write_uint64_t(dst, src) write_data(dst, src, sizeof(uint64_t))
#define read_int8_t(dst, src)    read_data(dst, src, sizeof(int8_t))
#define write_int8_t(dst, src)   write_data(dst, src, sizeof(int8_t))
#define read_int16_t(dst, src)   read_data(dst, src, sizeof(int16_t))
#define write_int16_t(dst, src)  write_data(dst, src, sizeof(int16_t))
#define read_int32_t(dst, src)   read_data(dst, src, sizeof(int32_t))
#define write_int32_t(dst, src)  write_data(dst, src, sizeof(int32_t))
#define read_int64_t(dst, src)   read_data(dst, src, sizeof(int64_t))
#define write_int64_t(dst, src)  write_data(dst, src, sizeof(int64_t))
#define read_float(dst, src)     read_data(dst, src, sizeof(float))
#define write_float(dst, src)    write_data(dst, src, sizeof(float))
#define read_double(dst, src)    read_data(dst, src, sizeof(double))
#define write_double(dst, src)   write_data(dst, src, sizeof(double))
#define read_bool(dst, src)      read_data(dst, src, sizeof(bool))
#define write_bool(dst, src)     write_data(dst, src, sizeof(bool))
#define read_struct(dst, src)    read_data(dst, src, sizeof(struct))
#define write_struct(dst, src)   write_data(dst, src, sizeof(struct))
#define read_enum(dst, src)      read_data(dst, src, sizeof(enum))
#define write_enum(dst, src)     write_data(dst, src, sizeof(enum))

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
    

    read_uint8_t(&(packet->packet_header.source_address), &data_ptr);
    read_uint32_t(&(packet->packet_header.packet_type), &data_ptr);
    read_uint8_t(&(packet->packet_header.size), &data_ptr);
    switch(packet->packet_header.packet_type) {
        case 0:
            break;
        case 1:
            read_uint16_t(&(packet->adc_sensor_packet.adc_value), &data_ptr);
            break;
        case 2:
            read_uint16_t(&(packet->dual_mode_sensor_packet.adc_value), &data_ptr);
            read_uint16_t(&(packet->dual_mode_sensor_packet.battery_percentage), &data_ptr);
            read_uint32_t(&(packet->dual_mode_sensor_packet.time), &data_ptr);
            read_uint8_t(&(packet->dual_mode_sensor_packet.button), &data_ptr);
            read_bool(&(packet->dual_mode_sensor_packet.conc_led_toggle), &data_ptr);
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
    

    write_uint8_t(&data_ptr, &(packet->packet_header.source_address));
    write_uint32_t(&data_ptr, &(packet->packet_header.packet_type));
    write_uint8_t(&data_ptr, &(packet->packet_header.size));
    switch(packet->packet_header.packet_type) {
        case 0:
            break;
        case 1:
            write_uint16_t(&data_ptr, &(packet->adc_sensor_packet.adc_value));
            break;
        case 2:
            write_uint16_t(&data_ptr, &(packet->dual_mode_sensor_packet.adc_value));
            write_uint16_t(&data_ptr, &(packet->dual_mode_sensor_packet.battery_percentage));
            write_uint32_t(&data_ptr, &(packet->dual_mode_sensor_packet.time));
            write_uint8_t(&data_ptr, &(packet->dual_mode_sensor_packet.button));
            write_bool(&data_ptr, &(packet->dual_mode_sensor_packet.conc_led_toggle));
            break;
        }


}



