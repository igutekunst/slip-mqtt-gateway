#ifndef PACKET_DECODER_H
#define PACKET_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "BinaryCodecHeader.h"
#include <stdint.h>
#include <stdlib.h>

typedef enum {
    PACKET_PARSE_ERROR_NONE,
    PACKET_PARSER_ERROR_MALFORMED,
    PACKET_PARSER_NULL_POINTER
} PacketParserReturnType;

PacketParserReturnType
packet_decode_from_byte_array(union ConcentratorPacket* packet,
                              uint8_t *data,
                              size_t size,
                              size_t* bytes_written
);

PacketParserReturnType
packet_encode_to_byte_array(uint8_t *data,
                            union ConcentratorPacket* packet,
                            size_t size,
                            size_t* bytes_written
);

#ifdef __cplusplus
}
#endif

#endif
