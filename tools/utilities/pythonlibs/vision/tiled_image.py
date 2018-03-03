####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     tiled_image.py
##  Authors:  Byron Changuion
##
##  Requires: Python 3.x
##
####################################################################################################

import math

import cv2
import numpy as np

class TiledImage:
    def __init__(self, num_images=2, output_shape=(600, 800)):
        """ Helper class to create a tiled image out of many smaller images.
        The class calculates how many horizontal and vertical blocks are needed to fit the requested number of images 
        and fills in unused blocks as blank. For example, to fit 4 images, the number of tiles is 2x2, to fit 5 images,
        the number of tiles is 3x2, with the last tile being blank.
        num_images - the maximum number of images that need to be composed into the tiled image. Note that the
                    actual number of tiles is equal to or larger than this number.
        output_shape - a tuple contaiing rows and columns of the output image. The output tiled image 
                            is a composition of sub images.
        """
        self.composed_image_shape = self.get_composed_image_shape(num_images)
        self.number_of_tiles = self.composed_image_shape[0] * self.composed_image_shape[1]
        self.output_height_and_width = output_shape
        self.images = None
        self.window_name = 'ELL side by side'
        cv2.namedWindow(self.window_name, cv2.WINDOW_NORMAL) # Ensure the window is resizable
        # The aspect ratio of the composed image is now self.composed_image_shape[0] : self.composed_image_shape[1]
        # Adjust the height of the window to account for this, else images will look distorted
        cv2.resizeWindow(self.window_name, output_shape[1], int(output_shape[0] * (self.composed_image_shape[0] / self.composed_image_shape[1])))

    def get_composed_image_shape(self, num_images):
        """Returns a tuple indicating the (rows,cols) of the required number of tiles to hold num_images."""
        # Split the image horizontally
        num_horizontal = math.ceil(math.sqrt(num_images))
        # Split the image vertically
        num_vertical = math.ceil(num_images / num_horizontal)

        return (num_vertical, num_horizontal)

    def resize_to_same_height(self, images):
        min_height = min([i.shape[0] for i in images])
        for i in range(len(images)):
            shape = images[i].shape
            h = shape[0]
            if h > min_height:
                scale = min_height / h
                new_size = (int(shape[1] * scale), int(shape[0] * scale))
                images[i] = cv2.resize(images[i], new_size)
        return images

    def compose(self):
        """Composes an image made by tiling all the sub-images set with `set_image_at`. """
        y_elements = []
        for vertical_index in range(self.composed_image_shape[0]):
            x_elements = []
            for horizontal_index in range(self.composed_image_shape[1]):
                current_index = vertical_index * self.composed_image_shape[1] + horizontal_index
                x_elements.append(self.images[current_index])
            # np.hstack only works if the images are the same height 
            x_elements = self.resize_to_same_height(x_elements)
            horizontal_image = np.hstack(tuple(x_elements))
            y_elements.append(horizontal_image)
        composed_image = np.vstack(tuple(y_elements))

        # Draw separation lines
        yStep = int(composed_image.shape[0] / self.composed_image_shape[0])
        xStep = int(composed_image.shape[1] / self.composed_image_shape[1])
        y = yStep
        x = xStep
        for horizontal_index in range(1, self.composed_image_shape[1]):
            cv2.line(composed_image, (x, 0), (x, composed_image.shape[0]), (0, 0, 0), 3)
            x = x + xStep
        for vertical_index in range(1, self.composed_image_shape[0]):
            cv2.line(composed_image, (0, y), (composed_image.shape[1], y), (0, 0, 0), 3)
            y = y + yStep
        
        return composed_image

    def set_image_at(self, image_index, frame):
        """Sets the image at the specified index. Once all images have been set, the tiled image result can be retrieved with `compose`."""
        # Ensure self.images is initialized.
        if self.images is None:
            self.images = [None] * self.number_of_tiles
            for i in range(self.number_of_tiles):
                self.images[i] = np.zeros((frame.shape), np.uint8)

        # Update the image at the specified index
        if (image_index < self.number_of_tiles):
            self.images[image_index] = frame
            return True
        return False

    def show(self):
        """Shows the final result of the tiled image. Returns True if the user indicates they are done viewing by pressing `Esc`. """
        # Compose the tiled image
        image = self.compose()
        # Show the tiled image
        cv2.imshow(self.window_name, image)
