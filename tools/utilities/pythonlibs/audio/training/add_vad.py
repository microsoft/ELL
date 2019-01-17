#!/usr/bin/env python3

###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     add_vad.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################

import argparse
import model_editor

if __name__ == "__main__":
    parser = argparse.ArgumentParser("Fix the given ELL model by replacing dummy reset trigger \
nodes with VoiceActivityDetectorNodes in an ELL Audio Classifier model")
    parser.add_argument("model", help="The *.ell model to edit)")
    parser.add_argument("--sample_rate", "-sr", type=int, help="The audio sample rate in Hz (default 16000)",
                        default=16000)
    parser.add_argument("--window_size", "-ws", type=int, help="The audio input window size (default 512)",
                        default=512)
    parser.add_argument("--tau_up", type=float, help="The noise floor is computed by tracking the frame power.\
 It goes up slow, with this time constant tauUp.", default=1.54)
    parser.add_argument("--tau_down", type=float, help="If the frame power is lower than the noise floor,\
 it goes down fast, with this time constant tauDown.", default=0.074326)
    parser.add_argument("--large_input", type=float, help="The exception is the case when the proportion frame\
 power/noise floor is larger than this largeInput.", default=2.400160)
    parser.add_argument("--gain_att", type=float, help="Then we switch to much slower adaptation by applying this\
 gainAtt.", default=0.002885)
    parser.add_argument("--threshold_up", type=float, help="Then we compare the energy of the current frame to the\
 noise floor. If it is thresholdUp times higher, we switch to state VOICE.", default=3.552713)
    parser.add_argument("--threshold_down", type=float, help="Then we compare the energy of the current frame to\
 the noise floor. If it is thresholdDown times lower, we switch to state NO VOICE.",
                        default=0.931252)
    parser.add_argument("--level_threshold", type=float, help="Special case is when the energy of the frame is lower\
 than levelThreshold, when we force the state to NO VOICE.", default=0.007885)

    args = parser.parse_args()
    filename = args.model
    print("Adding VAD node with sample rate {} and window size {} to {}".format(args.sample_rate, args.window_size,
                                                                                args.model))
    editor = model_editor.ModelEditor(args.model)

    changed = False
    for node in editor.find_rnns():
        changed |= editor.add_vad(node, args.sample_rate, args.window_size, args.tau_up, args.tau_down,
                                  args.large_input, args.gain_att, args.threshold_up, args.threshold_down,
                                  args.level_threshold)

    if changed:
        print("model updated")
        editor.save(filename)
    elif editor.vad_node is None:
        print("model does not contain any RNN, GRU or LSRM nodes")
    else:
        print("model unchanged")
