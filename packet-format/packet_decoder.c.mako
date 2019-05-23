#include <packet_header.h>
#include <stddef.h>

typedef enum {
    PACKET_PARSE_ERROR_NONE,
    PACKET_PARSER_ERROR_MALFORMED
} PacketParserReturnType;


void read_data(void* dst, uint8_t** data_ptr, size_t size) {
}

% for type,c_type in c_type_map.items():
#define read_${'{}(dst, src){}'.format(c_type, ' '*(8-len(c_type)))} read_data(dst, src, sizeof(${c_type})
% endfor

PacketParserReturnType
packet_decode_from_byte_array(union ConcentratorPacket* packet,
                              uint8_t *data,
                              size_t size
                              ) {
    uint8_t* data_ptr = data;

    ${render_field(packet)}
}

<%def name='render_field(field)'>
    % if field['type'] == 'struct':
    % for f in field['fields']:
    ${render_field(f)} \
    % endfor
    % elif field['type'] == 'tagged-union':
    // Tagged union
    % for f in field['header']['fields']:
    ${render_field(f)}
    % endfor
    % for f in field['fields']:
    ${render_field(f)}
    % endfor
    % else :
    read_${c_type_map[field['type']]}(${make_c_identifier(field['name'])}, &data_ptr); \
    % endif
</%def>
