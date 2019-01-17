#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     add_sink.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################

import argparse

import find_ell_root  # noqa: F401
import ell


class ModelEditor:
    """ Helper class that can modify an ELL model by inserting a SinkNode on a given node """

    def __init__(self):
        """
        Create new ModelEditor class
        """
        self.model = None
        self.builder = None

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
        size = list(output_port.GetMemoryLayout().size)
        while len(size) < 3:
            size = [1] + size
        shape = ell.math.TensorShape(size[0], size[1], size[2])
        self.builder.AddSinkNode(self.model, ell.nodes.PortElements(output_port), shape, functionName)
        return True

    def attach_sink(self, filename, nameExpr, functionName):
        """
        Process the given ELL model and insert SinkNode to monitor output of the given node
        """
        # load the ELL model.
        map = ell.model.Map(filename)
        self.model = map.GetModel()
        self.builder = ell.model.ModelBuilder()
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

        if changed:
            print("model updated")
            map.Save(filename)
        elif not found:
            print("model does not contain a node matching '{}'".format(nameExpr))
        else:
            print("model unchanged")


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Add a SinkNode to the given node in the model")
    parser.add_argument("model", help="The *.ell model to edit)")
    parser.add_argument("--node", "-n", help="A substring to match on the type of node to attach the SinkNode to",
                        required=True)
    parser.add_argument("--function", "-f", help="The name of the callback function", required=True)

    args = parser.parse_args()
    filename = args.model
    print("Adding SinkNode callback '{}' to the first node type matching '{}'".format(args.function, args.node))
    editor = ModelEditor()
    editor.attach_sink(args.model, args.node, args.function)
