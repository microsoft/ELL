#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     plot_model_stats.py (gallery)
##  Authors:  Lisa Ong
##
##  Requires: Python 3.x
##
####################################################################################################
import os
import sys
import argparse
import json
import matplotlib.pyplot as plt

# local helpers
import model_info_retriever as mir

class PlotModelStats:
    def __init__(self):
        self.arg_parser = argparse.ArgumentParser(
            "This script takes a path to an ELL-models model folder hierarchy and plots cost-accuracy curves\n"
            "that can be used to select the 'best' models")
        # model
        self.models_root = None
        self.models = []
        self.model_stats = []
        self.frontier_models = []

        # output
        self.output_figure = "model_speed_accuracy.png"
        self.output_format = "png"
        self.output_frontier_json_file = "frontier_models.json"
        self.output_all_models_json_file = "all_models.json"

        # plot
        self.plot_series = []
        self.plot_max_secs_per_frame = 1.5
        self.plot_min_top1_accuracy = 20
        self.plot_targets = ["pi3"]

        # reference: https://matplotlib.org/api/pyplot_api.html#matplotlib.pyplot.plot
        self.platforms_symbols = {
            "pi3": "ro", # red dot
            "pi3_64": "bs", # blue square
            "aarch64": "g^" # green triangle
        }
        self.platforms_lines = {
            "pi3": "r-", # red line
            "pi3_64": "b-", # blue line
            "aarch64": "g-" # green line
        }
        self.platforms_legend = {
            "pi3": "Raspberry Pi3/Raspbian",
            "pi3_64": "Raspberry Pi3/SUSE",
            "aarch64": "Dragonboard D410c"
        }

    def parse_command_line(self, argv):
        """Parses command line arguments"""
        # required arguments
        self.arg_parser.add_argument("models_root",
            help="the root of the model folder hierachy, e.g. c:/ELL-models/models/ILSVRC2012")

        # optional arguments
        self.arg_parser.add_argument("--output_figure", "-o",
            help="path to an output file for saving the plot",
            default=self.output_figure)
        self.arg_parser.add_argument("--output_format", "-of",
            help="path to an output format for saving the plot",
            default=self.output_format)
        self.arg_parser.add_argument("--output_stats", "-os",
            help="path to the output file for saving the pareto frontier stats",
            default=self.output_frontier_json_file)
        self.arg_parser.add_argument("--output_stats_all", "-osa",
            help="path to the output file for saving the stats for all models",
            default=self.output_all_models_json_file)
        self.arg_parser.add_argument("--plot_max_secs_per_frame", "-ps",
            help="when specified, only plot models that at least the specified speed (in seconds/frame)",
            type=float, default=self.plot_max_secs_per_frame)
        self.arg_parser.add_argument("--plot_min_top1_accuracy", "-pa",
            help="when specified, only plot models that are at least the specified accuracy (percentage)",
            type=float, default=self.plot_min_top1_accuracy)
        self.arg_parser.add_argument("--plot_targets", "-pt",
            help="list of device targets for the plot (default: pi3), valid values are ({})".format(", ".join(self.platforms_symbols.keys())),
            nargs="+", default=["pi3"])
        self.arg_parser.add_argument("--image_size", "-i",
            help="when specified, only plot models that have the specified image size(s) (default: all.)",
            nargs="+", default=["all"])
        self.arg_parser.add_argument("--model_name", "-model",
            help="when specified, only plot models that have the specified name(s) (default: all.)",
            nargs="+", default=["all"])

        args = self.arg_parser.parse_args(argv)
        if not os.path.isdir(args.models_root):
            raise FileNotFoundError("{} is not a folder".format(args.models_root))

        self.models_root = args.models_root
        self.output_figure = args.output_figure
        self.output_format = args.output_format
        self.output_frontier_json_file = args.output_stats
        self.output_all_models_json_file = args.output_stats_all
        self.plot_max_secs_per_frame = args.plot_max_secs_per_frame
        self.plot_min_top1_accuracy = args.plot_min_top1_accuracy
        self.plot_targets = dict.fromkeys(args.plot_targets)
        self.image_size = args.image_size
        self.model_name = args.model_name

    def find_models(self):
        """Finds the model files from the root folder"""
        # Walk the children of the root folder to find all available model folders
        # The hierachy is assumed to be:
        # models_root/
        #    model1/
        #    model2/
        # where model1, model2 contain model metadata files, and are also the names of the models
        children = [os.path.basename(child[0]) for child in os.walk(self.models_root)]
        self.models = [model for model in children if os.path.isdir(os.path.join(self.models_root, model))]

    def get_stats(self):
        """Collects the statistics from the models"""
        for model in self.models:
            with mir.ModelInfoRetriever(os.path.join(self.models_root, model), model) as model_data:
                try:
                    model_properties = model_data.get_model_properties()
                    if str(model_properties['image_size']) in self.image_size or self.image_size[0] == 'all':
                        if model_properties['model'] in self.model_name or self.model_name[0] == 'all':
                            accuracy = model_data.get_model_topN_accuracies()
                            speed = model_data.get_model_seconds_per_frame(self.plot_targets)                 
                            self.model_stats.append({"name": model_properties['name'], "model": model_properties['model'], \
                             "image_size": model_properties['image_size'], "accuracy" : accuracy, "secs_per_frame" : speed, \
                             "filter_size": model_properties['filter_size'], "increase_factor": model_properties['increase_factor'], \
                             "directory": model})
                except:
                    print("Could not collect stats for model '{}', skipping".format(model))

    def pareto_frontier(self, x, ytop1, ytop5, models, names, image_size, filter_size, increase_factor, directory, max_x):
        """Takes lists of x and ytop1 values, and return the sorted elements that lie on the Pareto frontier
           reference: http://oco-carbon.com/metrics/find-pareto-frontiers-in-python/
        """
        # sort the values in ascending order, and apply a limit to x
        values = sorted([[float(x[i]), float(ytop1[i]), float(ytop5[i]), models[i], names[i], image_size[i], filter_size[i], increase_factor[i], directory[i]]
                 for i in range(len(x)) if float(x[i]) < max_x], reverse=False)
        frontier = [values[0]]
        for pair in values[1:]:
            if pair[1] >= frontier[-1][1]: # look for higher values of Y...
                frontier.append(pair) # add them to the Pareto frontier
        return zip(*frontier)

    def compute_series(self):
        """Computes the series to be plotted, including the pareto frontier"""
        frontiers = []

        # transform to a list of
        # [accuracies1, costs1, platform_symbol1, accuracies2, costs2, platform_symbol2,...,
        #  frontier1, frontier2, ...]
        for platform in self.platforms_symbols.keys():

            # determine if this target is to be plotted
            add_to_plot = platform in self.plot_targets

            # not all platforms have secs_per_frame values (e.g. large models that can't link on Pi3/Raspbian)
            x = [float(stat["secs_per_frame"][platform]) for stat in self.model_stats if (platform in stat["secs_per_frame"])]
            if add_to_plot:
                self.plot_series.append(x) # plot the secs/frame

            names = [stat["name"] for stat in self.model_stats if (platform in stat["secs_per_frame"])]
            models = [stat["model"] for stat in self.model_stats if (platform in stat["secs_per_frame"])]
            image_size = [int(stat["image_size"]) for stat in self.model_stats if (platform in stat["secs_per_frame"])]
            ytop1 = [float(stat["accuracy"]["top1"]) for stat in self.model_stats if (platform in stat["secs_per_frame"])]
            ytop5 = [float(stat["accuracy"]["top5"]) for stat in self.model_stats if (platform in stat["secs_per_frame"])]
            models = [stat["model"] for stat in self.model_stats if (platform in stat["secs_per_frame"])]
            names = [stat["name"] for stat in self.model_stats if (platform in stat["secs_per_frame"])]
            image_size = [int(stat["image_size"]) for stat in self.model_stats if (platform in stat["secs_per_frame"])]
            filter_size = [int(stat["filter_size"]) for stat in self.model_stats if (platform in stat["secs_per_frame"])]
            increase_factor = [int(stat["increase_factor"]) for stat in self.model_stats if (platform in stat["secs_per_frame"])]
            directory = [stat["directory"] for stat in self.model_stats if (platform in stat["secs_per_frame"])]

            if add_to_plot:
                self.plot_series.append(ytop1) # plot the top 1 accuracy metric

            if add_to_plot:
                self.plot_series.append(self.platforms_symbols[platform]) # add the legend symbol

            # compute the pareto frontier
            # to compute pareto frontier we need x and top 1
            if x != [] and ytop1 != []:        
                fx, ftop1, ftop5, fmodel, fnames, fimsize, filsize, fincfac, fdir = self.pareto_frontier(x, ytop1, ytop5, models, names, image_size, filter_size, increase_factor, directory, max_x=self.plot_max_secs_per_frame)                
                self.frontier_models.append({ 'platform' : platform, 'frontier_models' : list(zip(fnames, fmodel, fdir, fimsize, filsize, fincfac, fx, ftop1, ftop5)) })
                if add_to_plot:
                    frontiers = frontiers + [fx, ftop1, self.platforms_lines[platform]] # plot the frontier based on top 1
            else:
                print('Could not retrieve secs per frame or top 1 accuracy metric from this model.')

        # put the frontiers after the series, so that we only need to specify legend once per platform
        if self.plot_series:
            self.plot_series = self.plot_series + frontiers

        # save frontier model stats to json
        with open(self.output_frontier_json_file, "w") as outfile:
            json.dump(self.frontier_models, outfile, ensure_ascii=False, indent=2)
        print("Saved frontier models to {}".format(self.output_frontier_json_file))

        # save all model stats to json
        with open(self.output_all_models_json_file, "w") as outfile:
            json.dump(self.model_stats, outfile, ensure_ascii=False, indent=2, sort_keys=True)
        print("Saved all models to {}".format(self.output_all_models_json_file))


    def plot(self):
        """Plots the curves from the model statistics"""

        plt.plot(*self.plot_series)
        plt.title("Model Cost vs Accuracy")

        plt.xlabel("Runtime (seconds/frame)")
        plt.ylabel("Top 1 accuracy (%)")

        # apply axis limits
        limits = plt.axis()

        xmin = 0 # speed is always positive
        xmax = min(limits[1], self.plot_max_secs_per_frame)
        ymin = max(limits[2], self.plot_min_top1_accuracy)
        ymax = min(limits[3], 100) # accuracy is only up to 100%
        plt.axis([xmin, xmax, ymin, ymax])

        legends = [self.platforms_legend[platform] for platform in self.platforms_legend.keys()
            if platform in self.plot_targets]
        plt.legend(legends, framealpha=0.6, fontsize="small")
        plt.grid(True)

        # plt.show() reclaims memory, so savefig() must be called before it
        plt.savefig(self.output_figure, format=self.output_format, dpi="figure",
            orientation="landscape")
        print("Saved plot as {} to {}".format(self.output_format, self.output_figure))

        plt.show()

    def run(self):
        """Main run method"""
        self.find_models()
        self.get_stats()
        self.compute_series()
        self.plot()

if __name__ == "__main__":
    program = PlotModelStats()
    program.parse_command_line(sys.argv[1:]) # drop the first argument (program name)
    program.run()
    