#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     add_image_preprocessing_metadata.py
#  Authors:  Mason Remy
#
#  Requires: Python 3.x
#
###################################################################################################

import argparse
import add_input_metadata

image_preprocessing_key_prefix = "model.input"

expected_color_channel_order_key = "{}.expectedColorChannelOrder".format(image_preprocessing_key_prefix)

expected_pixel_range_low_key = "{}.expectedPixelRangeLow".format(image_preprocessing_key_prefix)
expected_pixel_range_high_key = "{}.expectedPixelRangeHigh".format(image_preprocessing_key_prefix)

red_channel_mean_key = "{}.redChannelMean".format(image_preprocessing_key_prefix)
green_channel_mean_key = "{}.greenChannelMean".format(image_preprocessing_key_prefix)
blue_channel_mean_key = "{}.blueChannelMean".format(image_preprocessing_key_prefix)

red_channel_std_dev_key = "{}.redChannelStdDev".format(image_preprocessing_key_prefix)
green_channel_std_dev_key = "{}.greenChannelStdDev".format(image_preprocessing_key_prefix)
blue_channel_std_dev_key = "{}.blueChannelStdDev".format(image_preprocessing_key_prefix)

pytorch_imagenet_normalization_defaults = {
    expected_color_channel_order_key: "rgb",
    expected_pixel_range_low_key: 0.0,
    expected_pixel_range_high_key: 1.0,
    red_channel_mean_key: 0.485,
    green_channel_mean_key: 0.456,
    blue_channel_mean_key: 0.406,
    red_channel_std_dev_key: 0.229,
    green_channel_std_dev_key: 0.224,
    blue_channel_std_dev_key: 0.225
}


def add_image_preprocessing_metadata(model_path,
                                     color_order_str,
                                     scale_pixel_range,
                                     channel_means,
                                     channel_std_dev,
                                     starting_defaults={}):
    metadata_dict = starting_defaults

    if color_order_str:
        metadata_dict[expected_color_channel_order_key] = color_order_str

    if scale_pixel_range:
        metadata_dict[expected_pixel_range_low_key] = scale_pixel_range[0]
        metadata_dict[expected_pixel_range_high_key] = scale_pixel_range[1]

    # Check if both low and high range keys are in or not in the dictionary
    # Check this outside the `if args.scale_pixel_range:` block to handle default cases as well
    if (expected_pixel_range_low_key in metadata_dict) != (expected_pixel_range_high_key in metadata_dict):
        raise Exception("Either both or neither of low and high pixel ranges must be specified")

    # Assign means and standard deviations in the same order as color_order_str
    color_tag_to_mean_keys = {
        "r": red_channel_mean_key,
        "g": green_channel_mean_key,
        "b": blue_channel_mean_key
    }

    color_tag_to_std_dev_keys = {
        "r": red_channel_std_dev_key,
        "g": green_channel_std_dev_key,
        "b": blue_channel_std_dev_key
    }

    if color_order_str:
        color_order = list(color_order_str)
        for channel_idx in range(len(color_order)):
            color_tag = color_order[channel_idx]
            if channel_means:
                metadata_dict[color_tag_to_mean_keys[color_tag]] = channel_means[channel_idx]
            if channel_std_dev:
                metadata_dict[color_tag_to_std_dev_keys[color_tag]] = channel_std_dev[channel_idx]
    else:
        if channel_means is not None:
            print("Channel means ignored because color order was not specified")
        if channel_std_dev is not None:
            print("Channel std dev ignored because color order was not specified")

    add_input_metadata.add_input_metadata(model_path, metadata_dict)


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Add an image preprocessing metadata tag to the input node of the given model")
    parser.add_argument("model", help="The *.ell model to edit")
    parser.add_argument("--order", help="The order of color channels the model requires", choices=["rgb", "bgr"])
    parser.add_argument("--scale_pixel_range", help="The range to scale pixel values to", nargs=2, type=float,
                        metavar=("LOW", "HIGH"))
    parser.add_argument("--mean", help="The per-channel mean values to subtract from pixel values after scaling",
                        nargs=3, type=float)
    parser.add_argument("--stddev", help="The per-channel standard-deviation of values to divide each channel by "
                        "after scaling and mean subtraction", nargs=3, type=float)
    parser.add_argument("--pytorch_imagenet_normalization", help="Use the defaults for Imagenet normalization with "
                        "Pytorch ordering, overridden with any other arguments also specified", action="store_true")

    args = parser.parse_args()
    starting_defaults = {}
    if args.pytorch_imagenet_normalization:
        starting_defaults = pytorch_imagenet_normalization_defaults

    add_image_preprocessing_metadata(args.model,
                                     args.order,
                                     args.scale_pixel_range,
                                     args.mean,
                                     args.stddev,
                                     starting_defaults)
