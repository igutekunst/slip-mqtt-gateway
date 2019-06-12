#include "BinaryCodecHeader.h"
#include "BinaryCodec.h"
#include <stddef.h>
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



% if False:
% for type,c_type in c_type_map.items():
% if 'c_type' != 'enum': # enums are handled as a special case
#define read_${'{}(dst, src, bytes_remaining){}'.format(c_type, ' '*(8-len(c_type)))}  read_data(dst, src, sizeof(${c_type}), bytes_remaining)
#define write_${'{}(dst, src, bytes_remaining){}'.format(c_type, ' '*(8-len(c_type)))} write_data(dst, src, sizeof(${c_type}), bytes_remaining)
% endif
% endfor
%endif

#define READ_TYPED(c_type, packet_type, dst, src, bytes_remaining)  read_data(&(dst), src, sizeof(c_type), bytes_remaining)
#define WRITE_TYPED(c_type,packet_type, dst, src, bytes_remaining) write_data(dst, &(src), sizeof(c_type), bytes_remaining)

#define READ_ENUM(c_type, packet_type, dst, src, bytes_remaining) ${"\\"}
    uint64_t UNIQUE_NAME(scratch) = 0; memcpy(&UNIQUE_NAME(scratch), *(src), sizeof(packet_type)); dst = (c_type) UNIQUE_NAME(scratch); *(src) += sizeof(packet_type)


#define WRITE_ENUM(c_type, packet_type, dst, src, bytes_remaining) ${"\\"}
    packet_type UNIQUE_NAME(scratch) = (packet_type) src; memcpy(*dst, &UNIQUE_NAME(scratch), sizeof(packet_type)); *(dst) += sizeof(packet_type)
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
                              size_t size,
                              size_t* bytes_written
                              ) {
    uint8_t* data_ptr = data;
    size_t bytes_remaining = size;
    if (bytes_written == NULL) {
        return PACKET_PARSER_NULL_POINTER;
    }

    ${make_codec_body(parser_code, 'read')}
    *bytes_written = (size_t) (data_ptr - data);
    return PACKET_PARSE_ERROR_NONE;
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
                            size_t size,
                            size_t* bytes_written
                            ) {
    uint8_t* data_ptr = data;
    size_t bytes_remaining = size;
    if (bytes_written == NULL) {
        return PACKET_PARSER_NULL_POINTER;
    }

    ${make_codec_body(parser_code, 'write')}\
    *bytes_written = (size_t) (data_ptr - data);
    return PACKET_PARSE_ERROR_NONE;
}

<%def name='make_codec_body(code,rw)'>
<% indent = 0 %>
% for op in code:
<% F = 'ENUM' if 'full_type' in op and op['full_type'].startswith('enum') else 'TYPED' %>\
% if op['op'] == 'read':
    % if rw == 'read':
    ${' ' * indent}READ_${F}(${op['full_type']}, ${c_type_map[op['type']]}, packet->${op['dst']}, &data_ptr, &bytes_remaining);\
    % else:
    ${' ' * indent}WRITE_${F}(${op['full_type']}, ${c_type_map[op['type']]}, &data_ptr, packet->${op['dst']}, &bytes_remaining);\
    % endif
% elif op['op'] == 'switch':
    ${' ' * indent}switch(packet->${op['operand']}) {\
% elif op['op'] == 'end-switch':
    ${' ' * indent}}\
% elif op['op'] == 'return':
    ${' ' * indent}break;\
% elif op['op'] == 'label':
    ${' ' * indent}case ${op['value']}:\
% endif
<%
    o = op['op']
    if o == 'return':
        indent -= 4
    if o == 'endswitch':
        indent -= 4
    if o == 'switch':
        indent += 4
    if o == 'label':
        indent += 4
%>
% endfor
</%def>

