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
import model_data_retriever

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
        self.output_json_file = "frontier_models.json"

        # plot
        self.plot_series = []
        self.plot_max_secs_per_frame = 1.5
        self.plot_min_top5_accuracy = 60

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
            help="path to the output file for saving the stats",
            default=self.output_json_file)
        self.arg_parser.add_argument("--plot_max_secs_per_frame", "-ps",
            help="when specified, only plot models that at least the specified speed (in seconds/frame)",
            type=float, default=self.plot_max_secs_per_frame)
        self.arg_parser.add_argument("--plot_min_top5_accuracy", "-pa",
            help="when specified, only plot models that are at least the specified accuracy (percentage)",
            type=float, default=self.plot_min_top5_accuracy)

        args = self.arg_parser.parse_args(argv)
        if not os.path.isdir(args.models_root):
            raise FileNotFoundError("{} is not a folder".format(args.models_root))

        self.models_root = args.models_root
        self.output_figure = args.output_figure
        self.output_format = args.output_format
        self.output_json_file = args.output_stats
        self.plot_max_secs_per_frame = args.plot_max_secs_per_frame
        self.plot_min_top5_accuracy = args.plot_min_top5_accuracy

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
            try:
                data_retriever = model_data_retriever.ModelDataRetriever(os.path.join(self.models_root, model), model)
                accuracy = data_retriever.get_model_topN_accuracies()
                speed = data_retriever.get_model_seconds_per_frame(self.platforms_symbols.keys())
                self.model_stats.append({"model": model, "accuracy" : accuracy, "secs_per_frame" : speed})
            except:
                print("Could not collect stats for model '{}', skipping".format(model))

    def pareto_frontier(self, x, y, models, max_x):
        """Takes two lists of x and y values, and return the sorted elements that lie on the Pareto frontier
           reference: http://oco-carbon.com/metrics/find-pareto-frontiers-in-python/
        """
        # sort the values in ascending order, and apply a limit to x
        values = sorted([[x[i], y[i], models[i]] for i in range(len(x)) if x[i] < max_x], reverse=False)
        frontier = [values[0]]
        for pair in values[1:]:
            if pair[1] >= frontier[-1][1]: # look for higher values of Y...
                frontier.append(pair) # add them to the Pareto frontier
        frontier_x = [pair[0] for pair in frontier]
        frontier_y = [pair[1] for pair in frontier]
        frontier_model = [pair[2] for pair in frontier]
        return frontier_x, frontier_y, frontier_model

    def compute_series(self):
        """Computes the series to be plotted, including the pareto frontier"""
        frontiers = []

        # transform to a list of [accuracies1, speeds1, platform_symbol1, accuracies2, speeds2, platform_symbol2,...]
        for platform in self.platforms_symbols.keys():
            # not all platforms have secs_per_frame values (e.g. large models that can't link on Pi3/Raspbian)
            x = [float(stat["secs_per_frame"][platform]) for stat in self.model_stats if (platform in stat["secs_per_frame"])]
            self.plot_series.append(x)

            y = [float(stat["accuracy"]["top5"]) for stat in self.model_stats if (platform in stat["secs_per_frame"])]
            self.plot_series.append(y)

            self.plot_series.append(self.platforms_symbols[platform])

            # compute the pareto frontier
            models = [stat["model"] for stat in self.model_stats if (platform in stat["secs_per_frame"])]
            fx, fy, fmodel = self.pareto_frontier(x, y, models, max_x=self.plot_max_secs_per_frame)

            frontiers.append(fx)
            frontiers.append(fy)
            frontiers.append(self.platforms_lines[platform])

            self.frontier_models.append({ 'platform' : platform, 'frontier_models' : list(zip(fmodel, fx, fy)) })

        # put the frontiers after the series, so that we only need to specify legend once per platform
        self.plot_series = self.plot_series + frontiers

        # save stats to json
        with open(self.output_json_file, "w") as outfile:
            json.dump(self.frontier_models, outfile, ensure_ascii=False, indent=2)
        print("Saved frontier models to {}".format(self.output_json_file))


    def plot(self):
        """Plots the curves from the model statistics"""

        plt.plot(*self.plot_series)
        plt.title("Model Speed vs Accuracy")

        plt.xlabel("Speed (seconds/frame)")
        plt.ylabel("Top 5 accuracy (%)")

        # apply axis limits
        limits = plt.axis()

        xmin = 0 # speed is always positive
        xmax = min(limits[1], self.plot_max_secs_per_frame)
        ymin = max(limits[2], self.plot_min_top5_accuracy)
        ymax = min(limits[3], 100) # accuracy is only up to 100%
        plt.axis([xmin, xmax, ymin, ymax])

        plt.legend(self.platforms_legend.values(), framealpha=0.6, fontsize="small")
        plt.grid(True)

        # plt.show() reclaims memory, so savefig() must be called before it
        plt.savefig(self.output_figure, format=self.output_format, dpi="figure",
            orientation="landscape")
        print("Saved plot as {} to {}".format(self.output_format, self.output_figure))

        plt.show()

    def run(self):
        self.find_models()
        self.get_stats()
        self.compute_series()
        self.plot()

if __name__ == "__main__":
    program = PlotModelStats()

    argv = sys.argv
    argv.pop(0) # when passed directly into parse_args, the first argument (program name) is not skipped
    program.parse_command_line(argv)

    program.run()