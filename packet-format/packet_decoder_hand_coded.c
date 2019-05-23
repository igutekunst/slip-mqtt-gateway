#include <packet_header.h>
#include <stddef.h>

typedef enum {
    PACKET_PARSE_ERROR_NONE,
    PACKET_PARSER_ERROR_MALFORMED
} PacketParserReturnType;


void read_data(void* dst, uint8_t** data_ptr, size_t size) {
}

#define read_enum(dst, src) read_data(dst, src, sizeof( PacketParserReturnType))
#define read_int8_t(dst, src) read_data(dst, src, sizeof(int8_t))
#define read_int16_t(dst, src) read_data(dst, src, sizeof(int16_t))
#define read_int32_t(dst, src) read_data(dst, src, sizeof(int32_t))
#define read_int64_t(dst, src) read_data(dst, src, sizeof(int64_t))
#define read_uint16_t(dst, src) read_data(dst, src, sizeof(uint16_t))
#define read_uint32_t(dst, src) read_data(dst, src, sizeof(uint32_t))
#define read_uint64_t(dst, src) read_data(dst, src, sizeof(uint64_t))
#define read_float(dst, src) read_data(dst, src, sizeof(float))
#define read_double(dst, src) read_data(dst, src, sizeof(double))
#define read_uint8_t(dst, src) read_data(dst, src, sizeof(uint8_t))


PacketParserReturnType
packet_decode_from_byte_array(union ConcentratorPacket* packet,
                              uint8_t *data,
                              size_t size
                              ) {
    uint8_t* data_ptr = data;

    /// Read Packet Header
    read_uint8_t(&(packet->packet_header.source_address), &data_ptr);
    read_uint8_t(&(packet->packet_header.packet_type), &data_ptr);
    read_uint8_t(&(packet->packet_header.size), &data_ptr);
    switch (packet->packet_header.packet_type) {
        case ACK_PACKET:
            // Empty packet, do nothing
            break;

        case ADC_SENSOR_PACKET:
            read_uint16_t(packet->adc_sensor_packet.adc_value, &data_ptr);
            break;

        case DUAL_MODE_SENSOR_PACKET:
            read_uint16_t(&(packet->dual_mode_sensor_packet.adc_value), &data_ptr);
            read_uint16_t(&(packet->dual_mode_sensor_packet.batt), &data_ptr);
            read_uint32_t(&(packet->dual_mode_sensor_packet.time), &data_ptr);
            read_uint8_t (&(packet->dual_mode_sensor_packet.button), &data_ptr);
            read_uint8_t (&(packet->dual_mode_sensor_packet.conc_led_toggle), &data_ptr);
            break;
    }
}
