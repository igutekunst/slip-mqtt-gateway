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

% for type,c_type in c_type_map.items():
#define read_${'{}(dst, src){}'.format(c_type, ' '*(8-len(c_type)))}  read_data(dst, src, sizeof(${c_type}))
#define write_${'{}(dst, src){}'.format(c_type, ' '*(8-len(c_type)))} write_data(dst, src, sizeof(${c_type}))
% endfor

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
    ${make_codec_body(parser_code, 'read')}

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
    ${make_codec_body(parser_code, 'write')}

}

<%def name='make_codec_body(code,rw)'>
<% indent = 0 %>
% for op in code:
% if op['op'] == 'read':
    % if rw == 'read':
    ${' ' * indent}read_${c_type_map[op['type']]}(&(packet->${op['dst']}), &data_ptr);\
    % else:
    ${' ' * indent}write_${c_type_map[op['type']]}(&data_ptr, &(packet->${op['dst']}));\
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

