###############################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     tutorial_helpers.py
#  Authors:  Chris Lovett
#            Byron Changuion
#            Kern Handa
#            Chuck Jacobs
#
#  Requires: Python 3.x
#
###############################################################################

import os
import sys
import math
import platform
import cv2
import numpy as np
from itertools import product


# helper function that will find the ELL package if we need it.
def find_ell():
    try:
        import ell
    except:
        ell_root = os.getenv("ELL_ROOT")
        if ell_root and os.path.isdir(ell_root):
            sys.path += [ os.path.join(ell_root, 'tools/utilities/pythonlibs') ]
            import find_ell
            import ell
        else:
            raise Exception("Couldn't locate the ELL python module. Please refer to the ELL wiki " +
                            "for information on how to install the ELL python module.")

# Find any child directory that matches the four deployment targets (pi3,
# pi3_64, aarch64, host) or begins with "model". For all these directories,
# add it and its platform-specific build directory to Python's import lookup
# path
SCRIPT_PATH = os.path.dirname(os.path.abspath(__file__))
SEARCH_DIRS = [os.path.join(SCRIPT_PATH, d) for d in os.listdir(SCRIPT_PATH) if
               os.path.isdir(d) and (
               d in ["pi3", "pi3_64", "aarch64", "host"] or
               d.startswith("model"))]
sys.path += SEARCH_DIRS
sys.path += [os.path.join(d, "build") for d in SEARCH_DIRS]
if platform.system() == "Windows":
    sys.path += [os.path.join(d, "build", "Release") for d in SEARCH_DIRS]
else:
    sys.path += [os.path.join(d, "build") for d in SEARCH_DIRS]


class ImagePreprocessingMetadata:
    red_key = "r"
    green_key = "g"
    blue_key = "b"
    def __init__(self, color_channel_order_str=None, pixel_scale_range=None, channel_mean_dict=None, channel_std_dev_dict=None):
        self.color_channel_order_str = color_channel_order_str
        if pixel_scale_range:
            try:
                self.pixel_scale_range = [float(value) for value in pixel_scale_range]
            except ValueError:
                self.pixel_scale_range = None
        if channel_mean_dict:
            try:
                self.channel_mean_dict = {key : float(channel_mean_dict[key]) for key in channel_mean_dict}
            except ValueError:
                self.channel_mean_dict = None
        if channel_std_dev_dict:
            try:
                self.channel_std_dev_dict = {key : float(channel_std_dev_dict[key]) for key in channel_std_dev_dict}
            except ValueError:
                self.channel_std_dev_dict = None


def sigmoid(x):
    "Returns sigmoid activation applied to the input"
    if x > 0:
        return 1 / (1 + np.exp(-x))
    else:
        ex = np.exp(x)
        return ex / (1 + ex)


def softmax(x):
    "Returns softmax activation applied to the input"
    e_x = np.exp(x - np.max(x))
    return e_x / e_x.sum(axis=0)


def set_camera_resolution(camera, width, height):
    """ set the resolution of the OpenCV camera object """
    camera.set(3, width)
    camera.set(4, height)


def get_image_preprocessing_metadata(model_wrapper):
    color_order = model_wrapper.GetMetadata("model.input.expectedColorChannelOrder")

    pixel_scale_range = [model_wrapper.GetMetadata("model.input.expectedPixelRangeLow"),
                         model_wrapper.GetMetadata("model.input.expectedPixelRangeHigh")]

    channel_mean_dict = {
        ImagePreprocessingMetadata.red_key: model_wrapper.GetMetadata("model.input.redChannelMean"),
        ImagePreprocessingMetadata.green_key: model_wrapper.GetMetadata("model.input.greenChannelMean"),
        ImagePreprocessingMetadata.blue_key: model_wrapper.GetMetadata("model.input.blueChannelMean")
    }

    channel_std_dev_dict = {
        ImagePreprocessingMetadata.red_key: model_wrapper.GetMetadata("model.input.redChannelStdDev"),
        ImagePreprocessingMetadata.green_key: model_wrapper.GetMetadata("model.input.greenChannelStdDev"),
        ImagePreprocessingMetadata.blue_key: model_wrapper.GetMetadata("model.input.blueChannelStdDev")
    }

    return ImagePreprocessingMetadata(color_channel_order_str=color_order,
                                      pixel_scale_range=pixel_scale_range,
                                      channel_mean_dict=channel_mean_dict,
                                      channel_std_dev_dict=channel_std_dev_dict)


def prepare_image_for_model(
        image, width, height, reorder_to_rgb=False, ravel=True, preprocessing_metadata=None):
    """Prepare an image for use with a model. Typically, this involves:
        - Resize and center crop to the required width and height while
          preserving the image's aspect ratio.
          Simple resize may result in a stretched or squashed image which will
          affect the model's ability to classify images.
        - Perform image preprocessing and channel reordering. OpenCV gives the
          image in BGR order, so we may need to re-order the channels to RGB.
        - Optionally, convert the OpenCV result to a std::vector<float> for use
          with the ELL model.
    """
    if image.shape[0] > image.shape[1]:  # Tall (more rows than columns)
        row_start = int((image.shape[0] - image.shape[1]) / 2)
        row_end = row_start + image.shape[1]
        col_start = 0
        col_end = image.shape[1]
    else:  # Wide (more columns than rows)
        row_start = 0
        row_end = image.shape[0]
        col_start = int((image.shape[1] - image.shape[0]) / 2)
        col_end = col_start + image.shape[0]

    # Center crop the image maintaining aspect ratio
    cropped = image[row_start:row_end, col_start:col_end]

    # Resize to model's requirements
    resized = cv2.resize(cropped, (height, width))

    if reorder_to_rgb:
        if preprocessing_metadata is None:
            preprocessing_metadata = ImagePreprocessingMetadata()
        preprocessing_metadata.color_channel_order_str = "rgb"

    # Perform image preprocessing
    if preprocessing_metadata:
        color_order = [ImagePreprocessingMetadata.blue_key, ImagePreprocessingMetadata.green_key, ImagePreprocessingMetadata.red_key]
        if preprocessing_metadata.color_channel_order_str == "bgr":
            # OpenCV images are already in BGR order
            pass
        elif preprocessing_metadata.color_channel_order_str == "rgb":
            resized = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB)
            color_order = [ImagePreprocessingMetadata.red_key, ImagePreprocessingMetadata.green_key, ImagePreprocessingMetadata.blue_key]

        if preprocessing_metadata.pixel_scale_range:
            # OpenCV pixel values are integers in [0, 255]
            # To scale [0, 255] to [low, high]: low + (pixel / 255.0) * (high - low)
            low = preprocessing_metadata.pixel_scale_range[0]
            high = preprocessing_metadata.pixel_scale_range[1]
            resized = resized / 255.0
            resized = resized * (high - low)
            resized = resized + low

        if preprocessing_metadata.channel_mean_dict:
            # Create a mean list in the correct color channel order after re-ordering
            mean_list = np.array([preprocessing_metadata.channel_mean_dict[color_key] for color_key in color_order])
            resized = resized - mean_list

        if preprocessing_metadata.channel_std_dev_dict:
            # Create a std dev list in the correct color channel order after re-ordering
            std_dev_list = np.array([preprocessing_metadata.channel_std_dev_dict[color_key] for color_key in color_order])
            resized = resized / std_dev_list

    if ravel:
        # Return as a vector of floats
        result = resized.astype(np.float).ravel()
        return result
    return resized, (col_start, row_start), (cropped.shape[1] / width, cropped.shape[0] / height)


def get_top_n(predictions, n=5, threshold=0.20):
    """Return at most the top N predictions as a list of tuples that meet the
    threshold.
    The first of element of each tuple represents the index or class of the
    prediction and the second element represents that probability or confidence
    value.
    """
    filtered_predictions = [(i, predictions[i]) for i in
                            range(len(predictions)) if predictions[i] >=
                            threshold]
    filtered_predictions.sort(key=lambda tup: tup[1], reverse=True)
    result = filtered_predictions[:n]
    return result


def get_mean_duration(accumulated, duration, max_accumulation_entries=30):
    """Add a duration to an array and calculate the mean duration."""
    accumulated.append(duration)
    if (len(accumulated) > max_accumulation_entries):
        accumulated.pop(0)
    durations = np.array(accumulated)
    mean = np.mean(durations)
    return mean


def draw_header(image, text):
    """Helper to draw header text block onto an image"""
    draw_text_block(image, text, (0, 0), (50, 200, 50))
    return


def draw_footer(image, text):
    """Helper to draw footer text block onto an image"""
    draw_text_block(image, text, (0, image.shape[0] - 40), (200, 100, 100))
    return


def draw_text_block(image, text, block_top_left=(0, 0),
                    block_color=(50, 200, 50), block_height=40):
    """Helper to draw a filled rectangle with text onto an image"""
    FONT_SCALE = 0.7
    cv2.rectangle(
        image, block_top_left, (image.shape[1], block_top_left[1] +
                                block_height),
        block_color, cv2.FILLED)

    cv2.putText(
        image, text, (
            block_top_left[0] + int(block_height / 4), block_top_left[1] +
            int(block_height * 0.667)),
        cv2.FONT_HERSHEY_COMPLEX_SMALL, FONT_SCALE, (0, 0, 0), 1, cv2.LINE_AA)


class TiledImage:
    """ Helper class to create a tiled image out of many smaller images.
        The class calculates how many horizontal and vertical blocks are needed
        to fit the requested number of images and fills in unused blocks as
        blank. For example, to fit 4 images, the number of tiles is 2x2, to fit
        5 images, the number of tiles is 3x2, with the last tile being blank.

        `numImages` - the maximum number of images that need to be composed
        into the tiled image. Note that the actual number of tiles is equal
        to or larger than this number.

        `outputHeightAndWidth` - a list of two values giving the rows and
        columns of the output image. The output tiled image is a
        composition of sub images.
    """
    def __init__(self, numImages=2, outputHeightAndWidth=(600, 800)):
        self.composed_image_shape = self.get_composed_image_shape(numImages)
        self.number_of_tiles = (self.composed_image_shape[0] *
                                self.composed_image_shape[1])
        self.output_height_and_width = outputHeightAndWidth
        self.images = None
        self.window_name = "ELL side by side"

        # Ensure the window is resizable
        cv2.namedWindow(self.window_name, cv2.WINDOW_NORMAL)

        # The aspect ratio of the composed image is now
        # self.composed_image_shape[0] : self.composed_image_shape[1]
        # Adjust the height of the window to account for this, else images
        # will look distorted
        cv2.resizeWindow(
            self.window_name, outputHeightAndWidth[1],
            int(outputHeightAndWidth[0] * (
                self.composed_image_shape[0] / self.composed_image_shape[1])))

    def get_composed_image_shape(self, num_images):
        """Returns a tuple indicating the (rows,cols) of the required number of
        tiles to hold `num_images`.
        """
        # Split the image horizontally
        num_horizontal = math.ceil(math.sqrt(num_images))
        # Split the image vertically
        num_vertical = math.ceil(num_images / num_horizontal)

        return (num_vertical, num_horizontal)

    def resize_to_same_height(self, images):
        """Resizes a list of images to the minimum height among the images"""
        min_height = min([i.shape[0] for i in images])
        for i, image in enumerate(images):
            shape = image.shape
            height = shape[0]
            if height > min_height:
                scale = min_height / height
                new_size = (int(shape[1] * scale), int(shape[0] * scale))
                images[i] = cv2.resize(image, new_size)
        return images

    def compose(self):
        """Composes an image made by tiling all the sub-images set with
        `set_image_at`.
        """
        y_elements = []
        for vertical_index in range(self.composed_image_shape[0]):
            x_elements = []
            for horizontal_index in range(self.composed_image_shape[1]):
                current_index = (
                    vertical_index * self.composed_image_shape[1] +
                    horizontal_index)
                x_elements.append(self.images[current_index])

            # np.hstack only works if the images are the same height
            x_elements = self.resize_to_same_height(x_elements)
            horizontal_image = np.hstack(tuple(x_elements))
            y_elements.append(horizontal_image)

        composed_img = np.vstack(tuple(y_elements))

        # Draw separation lines
        y_step = int(composed_img.shape[0] / self.composed_image_shape[0])
        x_step = int(composed_img.shape[1] / self.composed_image_shape[1])
        y = y_step
        x = x_step

        for horizontal_index in range(1, self.composed_image_shape[1]):
            cv2.line(composed_img, (x, 0), (x, composed_img.shape[0]),
                     (0, 0, 0), 3)
            x += x_step

        for vertical_index in range(1, self.composed_image_shape[0]):
            cv2.line(composed_img, (0, y), (composed_img.shape[1], y),
                     (0, 0, 0), 3)
            y += y_step

        return composed_img

    def set_image_at(self, image_index, frame):
        """Sets the image at the specified index. Once all images have been
        set, the tiled image result can be retrieved with `compose`.
        """
        # Ensure self.images is initialized.
        if self.images is None:
            self.images = [None] * self.number_of_tiles
            for i in range(self.number_of_tiles):
                self.images[i] = np.zeros((frame.shape), np.uint8)

        # Update the image at the specified index
        if image_index < self.number_of_tiles:
            self.images[image_index] = frame
            return True
        return False

    def show(self):
        """Shows the final result of the tiled image. Returns True if the user
        indicates they are done viewing by pressing `Esc`.
        """
        # Compose the tiled image
        image_to_show = self.compose()
        # Show the tiled image
        cv2.imshow(self.window_name, image_to_show)


def play_sound(sound_file):
    """Plays the audio file that is at the fully qualified path `sound_file`"""
    system = platform.system()
    if system == "Windows":
        import winsound
        winsound.PlaySound(sound_file,
                           winsound.SND_FILENAME | winsound.SND_ASYNC)
    elif system == "Darwin":  # macOS
        from AppKit import NSSound
        from Foundation import NSURL
        cwd = os.getcwd()
        url = NSURL.URLWithString_("file://" + sound_file)
        NSSound.alloc().initWithContentsOfURL_byReference_(url, True).play()
    else:  # Linux
        import subprocess
        command = ["aplay", sound_file]
        subprocess.Popen(
            command, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            bufsize=0, universal_newlines=True)


class Region:
    """ Helper class that represents a detected object in an image.

        `category` - The category that represents the object detected.

        `probability` - The confidence that the specific object is present
        within the bounding box.

        `location` - An array representing the bounding box encompassing the
        detected object. First two values are the x and y coordinates
        representing the top, left of the bounding box, respectively. Second pair
        of values are the width and height of the bounding box, respectively.
        All values are normalized and thus need to be scaled accordingly by the
        dimensions of the image onto which the bounding box is to be applied.
    """
    def __init__(self, category, probability, location):
        self.category = category
        self.probability = probability
        self.location = np.array(location)

    def __repr__(self):
        return ('Region(category = {0.category},\n'
                'probability = {0.probability},\n'
                'location = {0.location})').format(self)

    def intersect(self, region):
        """ return the intersection between this region and the given region """
        x1, y1, w1, h1 = self.location
        x2, y2 = (x1 + w1, y1 + h1)
        x3, y3, w2, h2 = region.location
        x4, y4 = (x3 + w2, y3 + h2)
        x1, y1 = np.maximum((x1, y1), (x3, y3))
        x2, y2 = np.minimum((x2, y2), (x4, y4))
        if x2 <= x1 or y2 <= y1:
            return (0, 0, 0, 0)
        return (x1, y1, x2 - x1, y2 - y1)


def get_regions(inference_output, categories, threshold, anchor_boxes):
    """Returns an array of Region instances that represent detected objects
    and their locations.

    `inference_output` must be a tensor in the shape of the output of the
    network.

    `categories` is a list of categories that represent the type of objects to
    be detected.

    `threshold` is the minimum probability of the presence of an object and
    that the object is of a specific type, i.e., P(category|object) >=
    `threshold`.

    `anchor_boxes` is the list of anchor boxes that are used to augment the
    bounding boxes.
    """

    regions = []
    shape = inference_output.shape
    # four values for the bounding box coordinates plus one for the confidence
    # the rest of the values are the probabilities for the individual
    # categories
    box_size = 5 + len(categories)
    num_boxes = shape[2] // box_size
    for i, j, c in product(range(shape[0]),
                           range(shape[1]),
                           range(num_boxes)):
        # The coordinates and confidence values need to be "unpacked".
        # The order for each region is `[x, y, w, h, c,
        # category probabilities...]`. There are `num_boxes` regions for each
        # `i, j` coordinate pair.
        box_offset = c * box_size
        # The X and Y values need to have sigmoid activation applied
        predicted_x = sigmoid(inference_output[i, j, box_offset + 0])
        predicted_y = sigmoid(inference_output[i, j, box_offset + 1])
        # The width and height need to be exponentiated
        predicted_width = np.exp(inference_output[i, j, box_offset + 2])
        predicted_height = np.exp(inference_output[i, j, box_offset + 3])
        predicted_confidence = inference_output[i, j, box_offset + 4]

        # Get the final normalized values for the coordinates of the bounding
        # box
        x = (j + predicted_x) / shape[0]
        y = (i + predicted_y) / shape[1]
        w = predicted_width * anchor_boxes[2 * c + 0] / shape[0]
        h = predicted_height * anchor_boxes[2 * c + 1] / shape[1]
        confidence = predicted_confidence

        # The `category_scores` have not had softmax applied to them, so just
        # apply softmax and find the index of the largest value and get the
        # corresponding category label.
        category_scores = softmax(inference_output[i,
                                                   j,
                                                   box_offset + 5 :
                                                   box_offset + box_size])
        category_index = np.argmax(category_scores)
        probability = confidence * category_scores[category_index]

        # Only add to the list of returned regions if the probability is
        # greater than the threshold
        if probability > threshold:
            regions.append(Region(categories[category_index], probability,
                                  (x - (w/2), y - (h/2), w, h)))
    return regions


def non_max_suppression(regions, overlap_threshold, categories):
    """Given a list of `regions` (returned by a call to `get_regions`), remove
    any overlapping bounding boxes for the same object.

    `overlap_threshold` is the minimum amount of overlap needed between two
    boxes for them to be considered the same.

    `categories` is a list of categories that represent the type of objects to
    be detected.
    """

    if not regions:
        # If list of regions is empty, return an empty list
        return []

    final_regions = []
    for c in categories:
        # Only look at regions that have found the same object
        filtered_regions = [
            region for region in regions if region.category == c]
        if len(filtered_regions) < 2:
            continue

        # Get all the boxes that represent the object in question
        boxes = np.array([region.location for region in filtered_regions])

        # Calculate the areas once
        areas = boxes[:, 2] * boxes[:, 3]

        # `argsort` returns the indices in ascending order
        # We want the regions with the highest probability to be considered
        # the "main" regions
        sorted_indices = np.argsort([r.probability for r in filtered_regions])
        pick = []
        while len(sorted_indices):
            last = len(sorted_indices) - 1
            i = sorted_indices[last]
            pick.append(i)
            suppress = [last]
            for pos in range(last):
                j = sorted_indices[pos]

                intersection = filtered_regions[i].intersect(filtered_regions[j])

                overlap_width = intersection[2]
                overlap_height = intersection[3]

                # If either `overlap_width` or `overlap_height` are `<= 0`
                # then that means there's no overlap
                if overlap_width > 0 and overlap_height > 0:
                    overlap1 = overlap_width * overlap_height / areas[j]
                    overlap2 = overlap_width * overlap_height / areas[i]

                    # If there's enough overlap, we want to remove this box
                    if overlap1 > overlap_threshold and overlap2 > overlap_threshold:
                        suppress.append(pos)

            # Remove all the values that are at the list of indices of
            # `suppress` and return the resultant array
            sorted_indices = np.delete(sorted_indices, suppress)

        # Append to `final_regions` the main bounding boxes
        final_regions += [filtered_regions[i] for i in pick]

    return final_regions


def draw_regions_on_image(image, regions, offset, scale):
    """Draws a bounding box with a text label for each `Region` instance in
    `regions` on `image`."""

    for r in regions:
        # Unpack the location
        x, y, w, h = r.location
        width = image.shape[1]
        height = image.shape[0]

        # Clamp the values so that they remain within the bounds of the overall
        # image
        x_scale = scale[0]
        y_scale = scale[1]
        x1 = max(0, int(offset[0] + (x * x_scale)))
        y1 = max(0, int(offset[1] + (y * y_scale)))
        x2 = min(width, int(offset[0] + (x + w) * x_scale))
        y2 = min(height, int(offset[1] + (y + h) * y_scale))

        image = cv2.rectangle(image, (x1, y1), (x2, y2), (0, 255, 0), 3)
        image = cv2.putText(
            image, "{0} - {1:.0%}".format(r.category, r.probability),
            (x1, y1 - 13),
            cv2.FONT_HERSHEY_SIMPLEX,
            1e-3 * height,
            (0, 255, 0),
            2)
