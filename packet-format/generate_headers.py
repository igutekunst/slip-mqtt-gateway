"""
This tool generates headers for serializing and deserializing packets
Packets are described using a yaml format

The format is not formally defined yet, but will be eventually defined and validated

"""
import yaml
import argparse
import pprint

def load_yaml(yaml_filename, schema_file=None):
    with open(yaml_filename) as f:
        return yaml.safe_load(f)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('yaml_file')

    args = parser.parse_args()

    yaml = load_yaml(args.yaml_file)
    pp = pprint.PrettyPrinter(indent=2)

    pp.pprint(yaml)



if __name__ == '__main__':
   main()
