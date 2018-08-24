////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PrintModel.cpp (print)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintModel.h"
#include "LayerInspector.h"

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
template <typename valueType>
void PrintLayerParameters(std::ostream& out, const ell::predictors::neural::Layer<valueType>& layer)
{
    bool first = true;
    std::vector<NameValue> result = InspectLayerParameters<valueType>(layer);
    for (auto ptr = result.begin(), end = result.end(); ptr != end; ptr++)
    {
        NameValue nv = *ptr;
        if (!first)
        {
            out << ", ";
        }
        out << nv.name << "=" << nv.value;
        first = false;
    }
}

template <typename valueType>
void PrintNeuralNetworkPredictorNode(std::ostream& out, const model::Node& node)
{
    const ell::nodes::NeuralNetworkPredictorNode<valueType>& predictorNode = dynamic_cast<const ell::nodes::NeuralNetworkPredictorNode<valueType>&>(node);
    auto predictor = predictorNode.GetPredictor();
    auto layers = predictor.GetLayers();
    for (auto ptr = layers.begin(), end = layers.end(); ptr != end; ptr++)
    {
        std::shared_ptr<ell::predictors::neural::Layer<valueType>> layer = *ptr;
        std::string layerName = layer->GetRuntimeTypeName();
        out << "    " << layerName << "(";
        PrintLayerParameters(out, *layer);
        out << ")" << std::endl;
    }
}

void PrintNode(const model::Node& node, std::ostream& out, bool includeNodeId)
{
    bool isFirstInputPort = true;
    auto nodeType = node.GetRuntimeTypeName();
    std::string label = nodeType;
    if (includeNodeId)
    {
        label.insert(0, "<id:" + to_string(node.GetId())  + "> ");
    }
    bool isInputNode = nodeType.find("InputNode") == 0;
    out << label << "(";
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
                out << port->GetNode()->GetId() << ".";
                out << port->GetName();
                auto start = range.GetStartIndex();
                auto size = range.Size();
                out << "[" << start << ":" << (start + size) << "]";
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
        PrintNeuralNetworkPredictorNode<float>(out, node);
    }
    if (nodeType == "NeuralNetworkPredictorNode<double>")
    {
        PrintNeuralNetworkPredictorNode<double>(out, node);
    }
    else if (const ell::nodes::NeuralNetworkLayerNodeBase<float>* layerNode = dynamic_cast<const ell::nodes::NeuralNetworkLayerNodeBase<float>*>(&node))
    {
        auto& layer = layerNode->GetBaseLayer();
        std::string layerName = layer.GetRuntimeTypeName();
        out << "    " << layerName << "(";
        PrintLayerParameters(out, layer);
        out << ")" << std::endl;
    }
    else if (const ell::nodes::NeuralNetworkLayerNodeBase<double>* layerNode = dynamic_cast<const ell::nodes::NeuralNetworkLayerNodeBase<double>*>(&node))
    {
        auto& layer = layerNode->GetBaseLayer();
        std::string layerName = layer.GetRuntimeTypeName();
        out << "    " << layerName << "(";
        PrintLayerParameters(out, layer);
        out << ")" << std::endl;
    }
}

void PrintModel(const model::Model& model, std::ostream& out, bool includeNodeId)
{
    model.Visit([&out, includeNodeId](const model::Node& node) { PrintNode(node, out, includeNodeId); });
}
}
