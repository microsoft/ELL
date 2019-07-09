###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     model_editor.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################

import find_ell_root  # noqa: F401
import ell


class ModelEditor:
    """ Helper class that can modify an ELL model """

    def __init__(self, filename):
        """
        Create new ModelEditor class
        """
        # load the ELL model.
        self.map = ell.model.Map(filename)
        self.model = self.map.GetModel()
        self.builder = ell.model.ModelBuilder()
        self.vad_node = None

    def add_vad(self, rnn, sample_rate, window_size, tau_up, tau_down, large_input, gain_att, threshold_up,
                threshold_down, level_threshold):
        """
        Add a VoiceActivityDetectorNode as the "resetTrigger" input to the given RNN, LSTM or GRU node.
        """
        frame_duration = float(window_size) / float(sample_rate)
        reset_port = rnn.GetInputPort("resetTrigger")
        name = reset_port.GetParentNodes().Get().GetRuntimeTypeName()
        if "VoiceActivityDetector" not in name:
            # replace dummy trigger with VAD node
            if not self.vad_node:
                input_port = rnn.GetInputPort("input")
                input_node = input_port.GetParentNodes().Get()
                port = ell.nodes.PortElements(input_node.GetOutputPort("output"))
                flatShape = ell.model.PortMemoryLayout([port.Size()])
                re_node = self.builder.AddReinterpretLayoutNode(self.model, port, flatShape)
                port = ell.nodes.PortElements(re_node.GetOutputPort("output"))
                self.vad_node = self.builder.AddVoiceActivityDetectorNode(self.model, port, sample_rate,
                                                                          frame_duration, tau_up, tau_down,
                                                                          large_input, gain_att, threshold_up,
                                                                          threshold_down, level_threshold)
            # make the vad node the "resetTrigger" input of this rnn node
            reset_output = ell.nodes.PortElements(self.vad_node.GetOutputPort("output"))
            rnn.ResetInput(reset_output, "resetTrigger")
            return True
        return False

    def find_rnns(self):
        """ Find any RNN, LSTM or GRU nodes in the model """
        result = []
        iter = self.model.GetNodes()
        while iter.IsValid():
            node = iter.Get()
            name = node.GetRuntimeTypeName()
            if "RNN" in name or "GRU" in name or "LSTM" in name:
                result += [node]
            iter.Next()
        return result

    def save(self, filename):
        self.map.Save(filename)

    def find_sink_node(self, target):
        iter = self.model.GetNodes()
        while iter.IsValid():
            node = iter.Get()
            if "SinkNode" in node.GetRuntimeTypeName():
                parent = node.GetInputPort("input").GetParentNodes().Get()
                if parent.GetId() == target.GetId():
                    return True

            iter.Next()
        return False

    def add_sink_node(self, node, functionName):
        """
        Add a SinkNode so you can get a callback with the output of the given node id.
        """
        if self.find_sink_node(node):
            print("node '{}' already has a SinkNode".format(node.GetRuntimeTypeName()))
            return False
        output_port = node.GetOutputPort("output")
        layout = output_port.GetMemoryLayout()
        self.builder.AddSinkNode(self.model, ell.nodes.PortElements(output_port), layout, functionName)
        return True

    def attach_sink(self, nameExpr, functionName):
        """
        Process the given ELL model and insert SinkNode to monitor output of the given node
        """
        iter = self.model.GetNodes()
        changed = False
        found = False
        while iter.IsValid():
            node = iter.Get()
            if nameExpr in node.GetRuntimeTypeName():
                found = True
                changed |= self.add_sink_node(node, functionName)
                break
            iter.Next()
        if not found:
            print("model does not contain any nodes matching expression: {}".format(nameExpr))
        return changed

    def get_input_node(self):
        """ return the model input node """
        iter = self.model.GetNodes()
        while iter.IsValid():
            node = iter.Get()
            if "InputNode" in node.GetRuntimeTypeName():
                return node
            iter.Next()
        return None
