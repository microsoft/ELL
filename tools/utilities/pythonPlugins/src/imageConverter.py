###############################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     imageConverter.py
#  Authors:  Chris Lovett
#
###############################################################################

import sys
import numpy as np
import cv2
import argparse

def resize_image(image, newSize) :
    if (image.shape[0] > image.shape[1]) : # Tall(more rows than cols)
        rowStart = int((image.shape[0] - image.shape[1]) / 2)
        rowEnd = rowStart + image.shape[1]
        colStart = 0
        colEnd = image.shape[1]
    else: # Wide(more cols than rows)
        rowStart = 0
        rowEnd = image.shape[0]
        colStart = int((image.shape[1] - image.shape[0]) / 2)
        colEnd = colStart + image.shape[0]

        cropped = image[rowStart:rowEnd, colStart : colEnd]
        resized = cv2.resize(cropped, newSize)
        return resized

def prepare_image_for_predictor(image, newSize, scale, bgr2rgb) :
    if newSize[0] and newSize[1]:
        resized = resize_image(image, newSize)
    if bgr2rgb != 0:
        resized = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB)
    resized = resized.astype(np.float).ravel()
    if scale != 0:
        resized *= scale
    return resized

def save_raw(name, data) :
    with open(name, 'wb') as f :
        f.write(bytearray(data))
        f.close()

    
def strtobool(arg):
    if arg.lower() in [ 't', 'true', 'yes', '1' ]:
        return True
    return False
    

def main(argv) :
    """
    This is a plugin for the debugCompiler tool.  It expects a list of string arguments as input and it returns the image
    preprocessed and prepared for input to an ELL test model.  The result has to be a list of floats.
    """
    arg_parser = argparse.ArgumentParser("imageConverter takes an image as input and converts it to an array of floating point numbers")
    arg_parser.add_argument("--bgr", help="specify True if input data should be in BGR format (default False)", default = "False")
    arg_parser.add_argument("--width", type=int, help="resize the image to this new width (default None)", default = 0)
    arg_parser.add_argument("--height", type=int, help="resize the image to this new height (default None)", default = 0)
    arg_parser.add_argument("--scale", type=float, help="scale the floating point numbers by this amount (default None)", default = 0)
    arg_parser.add_argument("--image", help="name of file containing image as png, jpg, etc (default None)", default = None, required = True)
    arg_parser.add_argument("--outputData", "-od", help="name of file to write the resulting raw floating point data (default None)", default = None)

    args = arg_parser.parse_args(argv)

    image = cv2.imread(args.image)
    if image is None:
        print("Error reading image {}".format(args.image))

    resized = prepare_image_for_predictor(image, (args.height, args.width), args.scale, args.bgr)

    if args.outputData:
        save_raw(args.outputData, resized)

    return list(resized)


if __name__ == "__main__":
    args = sys.argv
    main(args[1:])
