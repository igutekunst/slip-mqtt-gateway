#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef void* PacketHandler;

typedef enum {
    PACKET_PARSE_ERROR_NONE,
    PACKET_PARSER_ERROR_MALFORMED
} PacketParserReturnType;

typedef enum {
    PacketTypeHeader,
    PacketTypeAck,
    PacketTypeDualModeSensorPacket
} PacketType;

struct PacketHeader {
    uint8_t sourceAddress;
    PacketType packetType;
    struct {int foo} Absolute;

};

struct AdcSensorPacket {
    struct PacketHeader header;
    uint16_t adcValue;
};

struct DualModeSensorPacket {
    struct PacketHeader header;
    uint16_t adcValue;
    uint16_t batt;
    uint32_t time100MiliSec;
    uint8_t button;
    bool concLedToggle;
};

union ConcentratorPacket {
    struct PacketHeader header;
    struct AdcSensorPacket adcSensorPacket;
    struct DualModeSensorPacket dmSensorPacket;
};

typedef PacketParserReturnType (*PacketCallback) (PacketType type, union ConcentratorPacket packet);


/**
 *
 * This function is used to register callbacks when specific packets are received.
 * The number of callbacks allowed can be configured via MAX_PACKET_CALLBACKS.
 * For maximum performance, it's recommended that you only use one handler and perform a switch operation
 * as part of the "user" library. However, this functionality has the potential to simplify user code.
 *
 * @param callback Function to be called when packet with matching type(s) are specified. If multiple
 *        types are specified (using va_args), then packets matching any specified types will
 *        trigger a callback. If no packet types are specified, then any packet will match
 *
 *
 * @param ... Additional types to trigger a callback.
 * @return
 */
PacketParserReturnType packet_handler_register_handler_for_types(PacketHandler handler, PacketCallback callback, ...);
//TODO: It probably makes sense to only every support one callback anyway


PacketParserReturnType paket_handler_handle_bytes(PacketHandler handler, uint8_t bytes, size_t count);

/**
 * Next Steps:
 *
 * Determine how yaml data structure becomes C structs
 * Determine how serializer works (is it hard-coded, or does it operate on some kind of data structure)
 *  I like the idea of describing the struct in terms of a string of offsets and sizes. This
 *  could potentially be an array that is statically created using the offsetof macro
 *
 *  The algorithm would read through the packet definition linearly, consuming fields one by one, and memcopying
 *  the bytes into the source struct.
 *
 *  In the case of simple packets, this could potentially be a simple memcpy, if we allow network byte order to be LE,
 *  and the structs don't have a any padding.
 *
 *  If there is padding, the parser/packet generator will need to skip sections of memory and only copy relevant bytes.
 *
 *  If the generated parser/formatter knows the layout of the target system, it can memcpy larger blocks that are packet.
 *
 *  So maybe the parser is setup as a list of memcpys, with conditionals.. like a mini language, or maybe even generated C
 *
 *  packet_ptr = packet;
 *  memcpy(packet, raw, sizeof(header);
 *  packet_ptr += sizeof(header);
 *  if (packet->header.type == A)  {
 *      memcpy(packet->a, packet_ptr, sizeof(A);
 *      packet_ptr += sizeof(A);
 *  } else if (packet->header.type == B) {
 *      // B is not packet as well
 *      packet_ptr = read_from_packet(packet->b.char_field, packet_ptr, sizeof(packet->b.char_field);
 *      packet_ptr = read_from_packet(packet->b.int_field, packet_ptr, sizeof(packet->b.int_field);
 *  }
 *
 *  // each read_from_packet will do byte order transformation if necessary. Maybe needs 4th argument (type id)
 *
 */


