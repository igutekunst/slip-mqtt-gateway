"""
This tool generates headers for serializing and deserializing packets
Packets are described using a yaml format

The format is not formally defined yet, but will be eventually defined and validated

"""
import yaml
import argparse
import pprint
from inflection import underscore, titleize, camelize
from mako.template import Template
from os.path import splitext
import os
import sys


c_type_map = {
    'uint8':    'uint8_t',
    'uint16':   'uint16_t',
    'uint32':   'uint32_t',
    'uint64':   'uint64_t',
    'int8':     'int8_t',
    'int16':    'int16_t',
    'int32':    'int32_t',
    'int64':    'int64_t',
    'float32':  'float',
    'float64':  'double',
    'bool':     'bool',
    'struct':   'struct',
    'enum':     'enum'
}

def make_c_identifier(name):
    return underscore(titleize(name).replace(' ', ''))

def make_c_macro_name(name):
    return underscore(titleize(name).replace(' ', '')).upper()

def make_c_typename(name):
    return camelize(titleize(name).replace(' ', ''))

def format_decl(decl_map, indent=0, type_map=c_type_map):
    # TODO support arrays
    if 'field_name' in decl_map:
        name = decl_map['field_name']
    else:
        name = make_c_identifier(decl_map['name'])
    if decl_map['type'] in ['struct', 'enum']:
        decl_type = '{} {}'.format(decl_map['type'],
                                   make_c_typename(decl_map['name']))
    else:
        decl_type = type_map[decl_map['type']]
    return '{}{} {};'.format(' '*indent, decl_type, name )

def process_packet_items(packet, parent=None, context=None):
    packet = packet.copy()
    """
    This function pre-processes a packet descriptor (packet), and transforms
    it into a state ready to be used by the template processor.

    It should maybe be included in the template itself


    For a C header file, tasks include:
        Transforming identifier names to the correct naming convention for a target e.g. "ADC Reading" -> adc_reading
        Transforming type names to match target e.g. uint32 -> uint32_t
        Inserting the header into all items in a tagged union
    :param packet:
    :return:
    """
    parent_type = parent['type'] if parent else None

    if packet['type'] == 'tagged-union':
        # We need to make an enumeration for
        # to tag which union member is currently being used
        packet['tags'] = []
        for field in packet['fields']:
            packet['tags'].append(make_c_macro_name(field['name']))

        for i, field in enumerate(packet['header']['fields']):
            if 'is_tag' in field and field['is_tag']:
                tag_enum = {
                    'type': 'enum',
                    'enumerators': packet['tags'],
                    'name': field['name']
                }
                packet['header']['fields'][i] = tag_enum

                if field['name'] in context['enums']:
                    pass
                else:
                    context['enums'][field['name']] = tag_enum

    # Special handling for structs within tagged unions
    # TODO handle tagged unions that don't have structs
    if packet['type'] in ['struct'] and parent_type == 'tagged-union':
        header = parent['header']
        packet['fields'].insert(0, header)

    if packet['type'] in ['tagged-union', 'struct']:
        items = []
        for item in packet['fields']:
            items.append(process_packet_items(item, packet, context))
        packet['fields'] = items
        if packet['name'] in context['structs']:
            pass
        else:
            context['structs'][packet['name']] = packet

    # Note, this needs to be after the recursive call to process_packet_items,
    # or we'll get infinite recursion
    if packet['type'] == 'tagged-union':
        packet['fields'].insert(0, packet['header'])

    return packet

def load_yaml(yaml_filename, schema_file=None):
    with open(yaml_filename) as f:
        return yaml.safe_load(f)


def fpath(f):
    path = os.path.dirname(os.path.abspath(__file__))
    return os.path.realpath(os.path.join(path, f))



# End likely C template



def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('yaml_file')

    args = parser.parse_args()

    data = load_yaml(args.yaml_file)

    templates = [fpath('packet_header.h.mako')]

    items = data['packet']
    context = {'structs': {}, 'enums': {}}
    packet = process_packet_items(items, context=context)
    for template_filename in templates:
        base_name, ext = splitext(template_filename)

        template = Template(filename=template_filename)

        out = template.render(packet=packet,
                              structs=context['structs'],
                              enums=context['enums'],
                              format_decl=format_decl,
                              template_filename=template_filename,
                              exe=os.path.split(sys.argv[0])[1],
                              make_c_typename=make_c_typename
                              )

        with open(base_name, 'w') as out_file:
            out_file.write(out)





if __name__ == '__main__':
   main()


"""
There are a couple stages here.

Read yaml
Generate some kind of header file, with all the structs in the native language, e.g. structs for C, classes for python, etc.
    Reformatting data to match target language (uint32 -> uint32_t)
    tagged union magic for C / Inheritance for Python, maybe

Generate algorithm for reading packets
Render algorithm into either statementse in target language, or into mini language readable by a suitable runtime


Preprocessing:
    Collect all structs that will be needed.
    Arrange as to minimize dependencies




"""
