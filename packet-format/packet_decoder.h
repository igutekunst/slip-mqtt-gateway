#ifndef PACKET_DECODER_H
#define PACKET_DECODER_H
#include <packet_header.h>

typedef enum {
    PACKET_PARSE_ERROR_NONE,
    PACKET_PARSER_ERROR_MALFORMED
} PacketParserReturnType;

PacketParserReturnType
packet_decode_from_byte_array(union ConcentratorPacket* packet,
                              uint8_t *data,
                              size_t size
);

#endif
