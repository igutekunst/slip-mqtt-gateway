<%
import json
%>\
#ifndef PACKET_HEADER_H
#define PACKET_HEADER_H
/**
* This file is auto-generated from ${template_filename}. Please
* edit the source template if possible and then re-run  ${exe}.
*/

#include <stdint.h>
#include <stdbool.h>

% for name, enum in enums.items():
${make_enum(enum)}\
% endfor \

% for name, struct in structs.items():
${make_struct(struct)}\
% endfor \

<%def name='make_struct(struct)'>
% if struct['type'] == 'tagged-union':
union ${make_c_typename(struct['name'])} { \
% else :
struct ${make_c_typename(struct['name'])} { \
% endif
% for field in struct['fields']:
${make_decl(field)}\
% endfor

};
</%def>

<%def name='make_decl(decl)'>
    ${format_decl(decl)}\
</%def>\
#endif


<%def name='make_enum(enum)'>
enum ${make_c_typename(enum['name'])} {
% for i, enumerator in enumerate(enum['enumerators']):
    ${enumerator}\
% if i != len(enum['enumerators']) -1:
,
% else :

% endif
% endfor
};
</%def>




