#include <packet_header.h>
#include <stddef.h>
#include <packet_decoder.h>


void read_data(void* dst, uint8_t** data_ptr, size_t size) {
    memcpy(dst, *data_ptr, size);
    *data_ptr += size;
}

% for type,c_type in c_type_map.items():
#define read_${'{}(dst, src){}'.format(c_type, ' '*(8-len(c_type)))} read_data(dst, src, sizeof(${c_type}))
% endfor

PacketParserReturnType
packet_decode_from_byte_array(union ConcentratorPacket* packet,
                              uint8_t *data,
                              size_t size
                              ) {
    uint8_t* data_ptr = data;

% for op in parser_code:
% if op['op'] == 'read':
    read_${c_type_map[op['type']]}(&(packet->${op['dst']}), &data_ptr);
% elif op['op'] == 'switch':
    switch(packet->${op['operand']}) {
% elif op['op'] == 'end-switch':
    }
% elif op['op'] == 'return':
    break;
% elif op['op'] == 'label':
    case ${op['value']}:
% endif
% endfor
    }

