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
import copy
import random
import string


def get_var_generator():
    used_vars = set()

    def random_uniqe_variable():
        nonlocal used_vars
        letters = string.ascii_lowercase + string.digits
        potential_var = '_unique_var_' + ''.join(random.choice(letters) for i in range(8))
        while potential_var in used_vars:
            potential_var = '_unique_var_' + ''.join(random.choice(letters) for i in range(5))
        return potential_var

    return random_uniqe_variable

uniqe_var = get_var_generator()


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

class MakeVisitor(object):

    def __new__(cls, name, bases, dct):
        x = super().__new__(cls, name, bases, dct)
        x.attr = 100
        return x


class PacketVisitor(object):

    def __init__(self):
        self.stack = []

    def visit(self, packet, parent=None):
        self.stack.append(packet)
        if packet:
            if packet['type'] == 'struct':
                if hasattr(self, 'visit_struct'):
                    packet = self.visit_struct(packet, parent)

                for i, field in enumerate(packet['fields']):
                    packet['fields'][i] = self.visit(field, packet)

                if hasattr(self, 'visit_struct_post'):
                    packet = self.visit_struct_post(packet, parent)

            elif packet['type'] == 'tagged-union':
                if hasattr(self, 'visit_tagged_union'):
                    packet = self.visit_tagged_union(packet, parent)

                self.stack.append(packet['header'])
                for i, field in enumerate(packet['header']['fields']):
                    packet['header']['fields'][i] = self.visit(field, packet)
                self.stack.pop()

                if hasattr(self, 'visit_tagged_union_post_header'):
                    packet = self.visit_tagged_union_post_header(packet, parent)

                for i, field in enumerate(packet['fields']):
                    packet['fields'][i] = self.visit(field, packet)

                if hasattr(self, 'visit_tagged_union_post'):
                    packet = self.visit_tagged_union_post(packet, parent)

            elif packet['type'] in c_type_map:
                if hasattr(self, 'visit_c_type'):
                    packet = self.visit_c_type(packet, parent)
        self.stack.pop()
        return packet

class GeneratePacketParser(PacketVisitor):
    def __init__(self):
        super().__init__()
        self._indent = 0
        self._context = []
        self.code = []

    def indent(self):
        return ' ' * (2 * self._indent)

    def push_context(self):
        self._context.append({})

    def pop_context(self):
        self._context.pop()

    def context(self):
        return self._context[len(self._context) - 1]

    def get_pointer(self):
        names = [item['name'] for item in self.stack[1:]]
        names = map(make_c_identifier, names)
        return '.'.join(names)

    def visit_struct(self, packet, parent):
        if parent['type'] == 'tagged-union':
            self.code.append({
                'op': 'label',
                'operand': self.context()['operand'],
                'value':  self.context()['case'],
            })
            self.context()['case'] += 1
            self._indent += 1
        return packet

    def visit_struct_post(self, item, parent):
        if parent['type'] == 'tagged-union':
            self._indent -=1
        self.code.append({
            'op': 'return'
        })

    def visit_tagged_union(self, utem, parent):
        print(self.get_pointer())
        return utem

    def visit_tagged_union_post(self, item, parent):
        self._indent -= 1
        self.pop_context()

        self.code.append({
            'op': 'end-switch'
        })

        return item

    def visit_tagged_union_post_header(self, item, parent):
        self.push_context()
        self.context()['case'] = 0
        tag = None
        for field in item['header']['fields']:
            if 'is_tag' in field and field['is_tag']:
                tag = field
        operand = self.get_pointer() + make_c_identifier(item['header']['name']) + '.' + make_c_identifier(tag['name'])
        self.context()['operand'] = operand
        self.code.append({
           'op': 'switch',
           'operand': operand
        })
        self._indent += 1
        return item

    def visit_c_type(self, item, parent):
        full_type = c_type_map[item['type']]
        c_type = item['type']
        if 'is_tag' in item and item['is_tag']:
            full_type = 'enum {}'.format(make_c_typename(item['name']))

        self.code.append({
            'op': 'read',
            'dst':  self.get_pointer(),
            'full_type': full_type,
            'type': c_type
        })
        return item

def load_yaml(yaml_filename, schema_file=None):
    with open(yaml_filename) as f:
        return yaml.safe_load(f)


def fpath(f):
    path = os.path.dirname(os.path.abspath(__file__))
    return os.path.realpath(os.path.join(path, f))


def process_yaml(yaml_filename, template_filename = None):

    data = load_yaml(yaml_filename)

    items = data['packet']
    unparsed_packet = copy.deepcopy(data['packet'])

    context = {'structs': {}, 'enums': {}}
    packet = process_packet_items(items, context=context)

    pv = GeneratePacketParser()
    pv.visit(unparsed_packet)

    template_context = dict(packet=packet,
                            structs=context['structs'],
                            enums=context['enums'],
                            format_decl=format_decl,
                            template_filename=template_filename,
                            exe=os.path.split(sys.argv[0])[1],
                            make_c_typename=make_c_typename,
                            make_c_identifier=make_c_identifier,
                            c_type_map=c_type_map,
                            parser_code=pv.code,
                            uniqe_var=uniqe_var)
    return template_context


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--yaml-file')
    parser.add_argument('--output-dir')
    parser.add_argument('-t', '--templates', nargs='+', help='<Required> template files', required=True)

    args = parser.parse_args()

    templates = [os.path.join(os.getcwd(), t) for t in args.templates]
    for template_filename in templates:
        base_name, ext = splitext(template_filename)

        template = Template(filename=template_filename)
        template_context = process_yaml(args.yaml_file, template_filename)

        out = template.render(**template_context)

        if args.output_dir:
            filename = os.path.split(base_name)[1]
            out_file_name = os.path.join(args.output_dir, filename)
            if not os.path.exists(args.output_dir):
                os.makedirs(args.output_dir)
        else:
            out_file_name = base_name

        with open(out_file_name, 'w') as out_file:
            out_file.write(out)
            print ("output file {}".format(out_file_name))


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
