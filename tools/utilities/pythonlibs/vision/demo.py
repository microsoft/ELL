#!/usr/bin/env python3
####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     demo.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
####################################################################################################

import argparse
import demoHelper as d

# note: to run this in headless mode on a Linux machine run the following from your terminal window
# export DISPLAY=:0
# then add the '-save' argument to get tagged frames to be saved to disk.


def main():
    helper = d.DemoHelper()

    arg_parser = argparse.ArgumentParser(
        "Runs the given ELL model passing images from camera or static image file\n"
        "Either the ELL model file, or the compiled model's Python module must be given,\n"
        "using the --model or --compiled_model options respectively.\n"
        "Example:\n"
        "   python demo.py categories.txt --compiled_model tutorial1/pi3/model1\n"
        "   python demo.py categories.txt --model model1.ell\n"
        "This shows opencv window with image classified by the model using given labels")

    # required arguments
    arg_parser.add_argument("labels", help="path to the labels file for evaluating the model, or comma \
separated list if using more than one model")

    # options
    arg_parser.add_argument("--model_name", help="name of the compiled model's Python module", default=None)
    arg_parser.add_argument("--save", help="save images captured by the camera", action='store_true')
    arg_parser.add_argument("--threshold", type=float, help="threshold for the minimum prediction score. A lower \
threshold will show more prediction labels, but they have a higher chance of being completely wrong.",
                            default=helper.threshold)
    arg_parser.add_argument("--bgr", help="specify True if input data should be in BGR format (default False)",
                            default=helper.bgr)
    arg_parser.add_argument("--nogui", help="disable GUI to enable automated testing of a batch of images",
                            action='store_true')
    arg_parser.add_argument("--iterations", type=int, help="when used with --nogui this tests multiple iterations of\
each image to get better timing information")
    arg_parser.add_argument("--print_labels", help="print predictions instead of drawing them on the image",
                            action='store_true')

    # mutually exclusive options
    group = arg_parser.add_mutually_exclusive_group()
    group.add_argument("--camera", type=int, help="the camera id of the webcam", default=0)
    group.add_argument("--image", help="path to an image file. If set, evaluates the model using the image, instead \
of a webcam")
    group.add_argument("--folder", help="path to an image folder. If set, evaluates the model using the images found \
there")
    group.add_argument("--list", help="list of paths to images. If set, evaluates the model using the images in the \
list")

    group2 = arg_parser.add_mutually_exclusive_group()
    group2.add_argument("--model", help="path to a model file")
    group2.add_argument("--compiled_model", help="path to the compiled model's Python module")

    args = arg_parser.parse_args()

    if not args.compiled_model and not args.model:
        print("### Error: Required one of --model or --compiled_model")
        return

    # setup some options on the demo helper
    if args.save:
        helper.save_images = True
    helper.threshold = args.threshold
    if args.iterations:
        helper.iterations = args.iterations
    helper.set_input(args.camera, args.folder, args.list, args.image)

    if args.print_labels:
        helper.print_labels = True
    helper.bgr = args.bgr
    helper.nogui = args.nogui
    helper.model_name = args.model_name

    # initialize the labels and the model (either reference or compiled)
    helper.load_model(args.labels, args.model, args.compiled_model)

    lastPrediction = ""
    help_prompt = 60

    while (not helper.done()):
        # Grab next frame
        frame = helper.get_next_frame()

        # Prepare the image to send to the model.
        # This involves scaling to the required input dimension and re-ordering from BGR to RGB
        data = helper.prepare_image_for_predictor(frame)

        # Get the model to classify the image, by returning a list of probabilities for the classes it can detect
        predictions = helper.predict(data)

        # Get the (at most) top 5 predictions that meet our threshold. This is returned as a list of tuples,
        # each with the text label and the prediction score.
        top5 = helper.get_top_n_predictions(predictions, 5)

        # Turn the top5 into a text string to display
        text = ", ".join(["(" + str(int(element[1] * 100)) + "%) " + helper.get_label(element[0]) for element in top5])

        save = False
        if text != lastPrediction:
            print(text)
            save = True
            lastPrediction = text

        # Draw the text on the frame
        if helper.nogui or helper.print_labels:
            if helper.new_frame:
                if not text:
                    text = "unknown"
                if helper.image_filename:
                    text = helper.image_filename + ": " + text
                print(text)
        else:
            frameToShow = frame
            helper.draw_label(frameToShow, text)
            helper.draw_fps(frameToShow)
            if help_prompt > 0:
                help_prompt -= 1
                helper.draw_footer(frameToShow, "Press ESC to close this window")
            # Show the new frame
            helper.show_image(frameToShow, save)

    print("Last prediction: " + lastPrediction)
    helper.report_times()


if __name__ == "__main__":
    main()
