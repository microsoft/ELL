####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     generate_md.py (gallery)
##  Authors:  Lisa Ong
##
##  Requires: Python 3.x
##
####################################################################################################
import os
import sys
import argparse
import re
import subprocess

# local helpers
import model_info_retriever as mir

def disable_text_wrapping(text):
    """Convert spaces to non-breaking spaces so that columns don't wrap"""
    return re.sub(r" ", "&nbsp;", text)

def _get_default_user():
    proc = subprocess.Popen(["git", "config", "user.name"],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            bufsize=0, universal_newlines=True)
    try:
        result, errors = proc.communicate()
    except subprocess.TimeoutExpired:
        proc.kill()
        result, errors = proc.communicate()

    if errors:
        return "Juan Lema"
    else:
        return " ".join(result)

class GenerateMarkdown:
    """Generates the gallery markdown for a model"""
    def __init__(self):
        self.arg_parser = argparse.ArgumentParser(
            "This script takes a model directory and generates a basic markdown file that describes the model\n")
        self.modeldir = None
        self.model = None
        self.outfile = None
        self.template = None
        self.model_data = {}
        self.printexe = None
        self.platforms = {
            "pi3" : "Raspberry Pi 3 (Raspbian) @ 700MHz",
            "pi3_64" : "Raspberry Pi 3 (OpenSUSE) @ 600MHz",
            "aarch64" : "DragonBoard 410c @ 1.2GHz"
        }
        self.user = _get_default_user()

    def parse_command_line(self, argv):
        """Parses command line arguments"""
        # required arguments
        self.arg_parser.add_argument("modeldir", help="the directory containing the model files")
        self.arg_parser.add_argument("outfile", help="path to the output filename")
        self.arg_parser.add_argument("printexe", help="path to the print executable that dumps raw model architecture")

        # optional arguments
        self.arg_parser.add_argument("--template", help="path to the input markdown template file", default="vision_model.md.in")
        self.arg_parser.add_argument("--user", help="user who trained the model (for attribution purposes)", nargs="+", default=self.user)

        args = self.arg_parser.parse_args(argv)

        self.modeldir = args.modeldir
        self.model = os.path.basename(self.modeldir)
        self.outfile = args.outfile
        self.printexe = args.printexe
        self.user = " ".join(args.user)

        if (not os.path.isfile(args.template)):
            raise FileNotFoundError("Template file not found: " + args.template)
        with open(args.template, 'r') as f:
            self.template = f.read()

        permalink = os.path.splitext(os.path.basename(self.outfile))[0]
        self._set_value("@PERMALINK@", permalink)

    def _set_value(self, key, value):
        "Replaces keys with values in the template"
        self.template = self.template.replace(key, str(value))

    def get_model_info(self):
        """Gathers information about the model"""
        with mir.ModelInfoRetriever(self.modeldir, self.model) as model_data:
            self.model_data = {
                "accuracy" : model_data.get_model_topN_accuracies(),
                "architecture" : model_data.get_model_architecture(self.printexe),
                "properties" : model_data.get_model_properties(),
                "timings" : model_data.get_model_seconds_per_frame(self.platforms.keys())
            }

    def _arch_layers_to_html(self, arch_layers):
        "Formats arch_layers to HTML table rows"
        def _format_parameters(parameters):
            pairs = ["{}={}".format(key, parameters[key]) for key in parameters.keys()]
            return ",&nbsp;".join(pairs)

        result = "\n"
        for layer in arch_layers:
            # formatted for readability
            result += "\t\t\t\t<tr class=\"arch-table\">\n"
            result += "\t\t\t\t\t<td>{}</td>\n".format(disable_text_wrapping(layer["name"]))
            result += "\t\t\t\t\t<td>&#8680;&nbsp;{}</td>\n".format(disable_text_wrapping(layer["output_shape"]))
            result += "\t\t\t\t\t<td>{}</td>\n".format(disable_text_wrapping(_format_parameters(layer["parameters"])))
            result += "\t\t\t\t</tr>\n"

        result += "\t\t\t"

        # tabs to spaces (for consistency in the generated markdown)
        result = re.sub(r"\t", "    ", result)

        return result

    def write_model_info(self):
        """Writes information about the model using the template"""
        # accuracy
        accuracy = self.model_data["accuracy"]
        self._set_value("@TOP_1_ACCURACY@", accuracy['top1'])
        self._set_value("@TOP_5_ACCURACY@", accuracy['top5'])

        # architecture
        arch_layers = self.model_data["architecture"]
        html = self._arch_layers_to_html(arch_layers)
        self._set_value("@MODEL_ARCH@", html)

        # properties
        properties = self.model_data["properties"]
        self._set_value("@IMAGE_SIZE@", properties["image_size"])
        self._set_value("@MODEL_NAME@", self.model)
        self._set_value("@MODEL_SIZE_MB@", properties["size_mb"])
        self._set_value("@NUM_CLASSES@", properties["num_classes"])

        # general info
        self._set_value("@TRAINER@", properties["trainer"])
        self._set_value("@USER@", self.user)

        # timings per platform
        timings = "<br>".join(["{}: {}s/frame".format(self.platforms[platform],
                                                      self.model_data["timings"][platform])
                               for platform in self.platforms.keys()
                               if platform in self.model_data["timings"]])
        self._set_value("@MODEL_PERFORMANCE@", timings)
        self._set_value("@pi3_SECONDS_PER_FRAME@", self.model_data["timings"].get("pi3", ""))

    def save_model_info(self):
        """Saves information about the model to file"""
        with open(self.outfile, 'w', encoding='utf-8') as of:
            of.write(self.template)
        print("Saved to: " + self.outfile)

    def run(self):
        """Main run method"""
        self.get_model_info()
        self.write_model_info()
        self.save_model_info()

if __name__ == "__main__":
    program = GenerateMarkdown()
    program.parse_command_line(sys.argv[1:]) # drop the first argument (program name)
    program.run()