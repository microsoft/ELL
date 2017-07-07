////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PrintModel.cpp (print)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintModel.h"

// utilities
#include "Exception.h"
#include "OutputStreamImpostor.h"

// model
#include "InputPort.h"
#include "Model.h"
#include "NeuralNetworkPredictorNode.h"
#include "Node.h"

#include <iostream>

namespace ell
{
void PrintNode(const model::Node& node, std::ostream& out)
{
    bool isFirstInputPort = true;
    auto nodeType = node.GetRuntimeTypeName();
    bool isInputNode = nodeType.find("InputNode") == 0;
    out << "node_" << node.GetId() << " = " << nodeType << "(";
    if (isInputNode)
    {
        const auto& outputPort = node.GetOutputPorts()[0];
        out << outputPort->Size();
    }
    else
    {
        for (const auto& inputPort : node.GetInputPorts())
        {
            out << (isFirstInputPort ? "" : ", ");
            isFirstInputPort = false;

            auto elements = inputPort->GetInputElements();
            if (elements.NumRanges() > 1)
            {
                out << "{";
            }

            bool isFirstRange = true;
            for (const auto& range : elements.GetRanges())
            {
                out << (isFirstRange ? "" : ", ");
                isFirstRange = false;

                auto port = range.ReferencedPort();
                out << "node_" << port->GetNode()->GetId() << "." << port->GetName();
                if (!range.IsFullPortRange())
                {
                    auto start = range.GetStartIndex();
                    auto size = range.Size();
                    out << "[" << start << ":" << (start + size) << "]";
                }
            }

            if (elements.NumRanges() > 1)
            {
                out << "}";
            }
        }
    }

    out << ")" << std::endl;

    // model Visit doesn't look inside this node...
    if (nodeType == "NeuralNetworkPredictorNode<float>")
    {
        const ell::nodes::NeuralNetworkPredictorNode<float>& predictorNode = dynamic_cast<const ell::nodes::NeuralNetworkPredictorNode<float>&>(node);
        auto predictor = predictorNode.GetPredictor();
        auto layers = predictor.GetLayers();
        int layerId = 0;
        for (auto ptr = layers.begin(), end = layers.end(); ptr != end; ptr++)
        {
            std::shared_ptr<ell::predictors::neural::Layer<float>> layer = *ptr;
            auto shape = layer->GetLayerParameters().outputShape;
            std::string layerName = layer->GetRuntimeTypeName();
            out << "    layer_" << layerId << " = " << layerName << "(";
            out << "shape=[" << std::to_string(shape[0]) << "," << std::to_string(shape[1]) << "," << std::to_string(shape[2]) << "]";
            out << ")" << std::endl;
            layerId++;
        }
    }
}

void PrintModel(const model::Model& model, std::ostream& out)
{
    model.Visit([&out](const model::Node& node) { PrintNode(node, out); });
}
}
