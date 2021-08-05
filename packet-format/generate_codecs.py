import argparse
import os
import string
import mako
from generate_headers import process_yaml


def fpath(f):
    path = os.path.dirname(os.path.abspath(__file__))
    return os.path.realpath(os.path.join(path, f))


def get_template(template_name):
    return fpath(os.path.join('templates', template_name))


def substitute_from_env(s, env):
    return string.Template(s).safe_substitute(env)


class GeneratorPlugin(object):

    def __init__(self, plugin_dir):
        self.plugin_dir = os.path.realpath(plugin_dir)
        self.data_yaml = os.path.join(self.plugin_dir, 'data.yaml')
        if not os.path.exists(self.data_yaml):
            raise Exception("Please create data.yaml")
        self._get_templates()

    def _get_templates(self):
        # TODO load yaml plugin descriptor
        self.templates = [
                          ('$base/c/core/slip.c', 'src/slip.c'),
                          ('$base/c/core/BinaryCodec.c.mako', 'src/BinaryCodec.c'),
                          ]
        env = {
            'base': fpath('templates')
        }
        if hasattr(self, 'get_templates'):
            self.templates.extend(self.get_templates())

        self.templates = [(substitute_from_env(s, env),
                           substitute_from_env(d, env)) for s, d in self.templates]
        self.templates = [(s,
                           os.path.join(self.plugin_dir, d)) for s, d in self.templates]
        print(self.templates)

    def generate(self):
        template_context = process_yaml(self.data_yaml)
        for (src, dst) in self.templates:
            if not os.path.exists(src):
                raise Exception('Template does not exist: {}'.format(os.path.relpath(src)))

            update_needed = True
            if src.endswith('.mako'):
                print("Rendering {}".format(src))
                template = mako.template.Template(filename=src)
                rendered = template.render(**template_context)
            else:
                print("Copying {}".format(src))
                with open(src) as f:
                    rendered = f.read()
            dest_dir = os.path.dirname(dst)

            # Ensure destination directory exists
            if not os.path.exists(dest_dir):
                os.makedirs(dest_dir)

            # Don't overwrite the destination file of the content is identical
            if os.path.exists(dst):
                with open(dst) as f:
                    old_content = f.read()
                    if old_content == rendered:
                        update_needed = False
            if update_needed:
                with open(dst, 'w') as f:
                    f.write(rendered)






def main():
    parser = argparse.ArgumentParser()

    # This function runs the codec template processor
    # The codec template processor looks for a binary codec
    # description file in the current directory, and uses it to generate
    # codec source files for the enabled targets

    # Targets can be specified on the command line, or via a configuration file
    # Each target/enabled plugin will create its own folder in the output folder
    plugins = [GeneratorPlugin('.')]

    for plugin in plugins:
        plugin.generate()


    # TODO need a good name for the output folder
if __name__ == '__main__':
    main()
